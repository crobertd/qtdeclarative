/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Declarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickspriteimage_p.h"
#include "qquicksprite_p.h"
#include "qquickspriteengine_p.h"
#include <private/qsgcontext_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <qsgnode.h>
#include <qsgengine.h>
#include <qsgtexturematerial.h>
#include <qsgtexture.h>
#include <QFile>
#include <cmath>
#include <qmath.h>
#include <QDebug>

QT_BEGIN_NAMESPACE

static const char vertexShaderCode[] =
    "attribute highp vec2 vTex;\n"
    "uniform highp vec4 animData;// interpolate(bool), duration, frameCount (this anim), timestamp (this anim)\n"
    "uniform highp vec4 animPos;//sheet x,y, width/height of this anim\n"
    "uniform highp vec4 animSheetSize; //width/height of whole sheet, width/height of element\n"
    "\n"
    "uniform highp mat4 qt_Matrix;\n"
    "uniform highp float timestamp;\n"
    "\n"
    "varying highp vec4 fTexS;\n"
    "varying lowp float progress;\n"
    "\n"
    "\n"
    "void main() {\n"
    "    //Calculate frame location in texture\n"
    "    highp float frameIndex = mod((((timestamp - animData.w)*1000.)/animData.y),animData.z);\n"
    "    progress = mod((timestamp - animData.w)*1000., animData.y) / animData.y;\n"
    "\n"
    "    frameIndex = floor(frameIndex);\n"
    "    fTexS.xy = vec2(((frameIndex + vTex.x) * animPos.z / animSheetSize.x), ((animPos.y + vTex.y * animPos.w) / animSheetSize.y));\n"
    "\n"
    "    //Next frame is also passed, for interpolation\n"
    "    //### Should the next anim be precalculated to allow for interpolation there?\n"
    "    if (animData.x == 1.0 && frameIndex != animData.z - 1.)//Can't do it for the last frame though, this anim may not loop\n"
    "        frameIndex = mod(frameIndex+1., animData.z);\n"
    "    fTexS.zw = vec2(((frameIndex + vTex.x) * animPos.z / animSheetSize.x), ((animPos.y + vTex.y * animPos.w) / animSheetSize.y));\n"
    "\n"
    "    gl_Position = qt_Matrix * vec4(animSheetSize.z * vTex.x, animSheetSize.w * vTex.y, 0, 1);\n"
    "}\n";

static const char fragmentShaderCode[] =
    "uniform sampler2D texture;\n"
    "uniform lowp float qt_Opacity;\n"
    "\n"
    "varying highp vec4 fTexS;\n"
    "varying lowp float progress;\n"
    "\n"
    "void main() {\n"
    "    gl_FragColor = mix(texture2D(texture, fTexS.xy), texture2D(texture, fTexS.zw), progress) * qt_Opacity;\n"
    "}\n";

class QQuickSpriteMaterial : public QSGMaterial
{
public:
    QQuickSpriteMaterial();
    virtual ~QQuickSpriteMaterial();
    virtual QSGMaterialType *type() const { static QSGMaterialType type; return &type; }
    virtual QSGMaterialShader *createShader() const;
    virtual int compare(const QSGMaterial *other) const
    {
        return this - static_cast<const QQuickSpriteMaterial *>(other);
    }

    QSGTexture *texture;

    qreal timestamp;
    float interpolate;
    float frameDuration;
    float frameCount;
    float animT;
    float animX;
    float animY;
    float animWidth;
    float animHeight;
    float sheetWidth;
    float sheetHeight;
    float elementWidth;
    float elementHeight;
};

QQuickSpriteMaterial::QQuickSpriteMaterial()
    : timestamp(0)
    , interpolate(1.0f)
    , frameDuration(1.0f)
    , frameCount(1.0f)
    , animT(0.0f)
    , animX(0.0f)
    , animY(0.0f)
    , animWidth(1.0f)
    , animHeight(1.0f)
    , sheetWidth(1.0f)
    , sheetHeight(1.0f)
    , elementWidth(1.0f)
    , elementHeight(1.0f)
{
    setFlag(Blending, true);
}

QQuickSpriteMaterial::~QQuickSpriteMaterial()
{
    delete texture;
}

class SpriteMaterialData : public QSGMaterialShader
{
public:
    SpriteMaterialData(const char *vertexFile = 0, const char *fragmentFile = 0)
    {
    }

    void deactivate() {
        QSGMaterialShader::deactivate();

        for (int i=0; i<8; ++i) {
            program()->setAttributeArray(i, GL_FLOAT, chunkOfBytes, 1, 0);
        }
    }

    virtual void updateState(const RenderState &state, QSGMaterial *newEffect, QSGMaterial *)
    {
        QQuickSpriteMaterial *m = static_cast<QQuickSpriteMaterial *>(newEffect);
        m->texture->bind();

        program()->setUniformValue(m_opacity_id, state.opacity());
        program()->setUniformValue(m_timestamp_id, (float) m->timestamp);
        program()->setUniformValue(m_animData_id, m->interpolate, m->frameDuration, m->frameCount, m->animT);
        program()->setUniformValue(m_animPos_id, m->animX, m->animY, m->animWidth, m->animHeight);
        program()->setUniformValue(m_animSheetSize_id, m->sheetWidth, m->sheetHeight, m->elementWidth, m->elementHeight);

        if (state.isMatrixDirty())
            program()->setUniformValue(m_matrix_id, state.combinedMatrix());
    }

    virtual void initialize() {
        m_matrix_id = program()->uniformLocation("qt_Matrix");
        m_opacity_id = program()->uniformLocation("qt_Opacity");
        m_timestamp_id = program()->uniformLocation("timestamp");
        m_animData_id = program()->uniformLocation("animData");
        m_animPos_id = program()->uniformLocation("animPos");
        m_animSheetSize_id = program()->uniformLocation("animSheetSize");
    }

    virtual const char *vertexShader() const { return vertexShaderCode; }
    virtual const char *fragmentShader() const { return fragmentShaderCode; }

    virtual char const *const *attributeNames() const {
        static const char *attr[] = {
           "vTex",
            0
        };
        return attr;
    }

    int m_matrix_id;
    int m_opacity_id;
    int m_timestamp_id;
    int m_animData_id;
    int m_animPos_id;
    int m_animSheetSize_id;

    static float chunkOfBytes[1024];
};

float SpriteMaterialData::chunkOfBytes[1024];

QSGMaterialShader *QQuickSpriteMaterial::createShader() const
{
    return new SpriteMaterialData;
}

struct SpriteVertex {
    float tx;
    float ty;
};

struct SpriteVertices {
    SpriteVertex v1;
    SpriteVertex v2;
    SpriteVertex v3;
    SpriteVertex v4;
};

/*!
    \qmlclass SpriteImage QQuickSpriteImage
    \inqmlmodule QtQuick 2
    \inherits Item
    \brief The SpriteImage element draws a sprite animation

*/
/*!
    \qmlproperty bool QtQuick2::SpriteImage::running

    Whether the sprite is animating or not.

    Default is true
*/
/*!
    \qmlproperty bool QtQuick2::SpriteImage::interpolate

    If true, interpolation will occur between sprite frames to make the
    animation appear smoother.

    Default is true.
*/
/*!
    \qmlproperty list<Sprite> QtQuick2::SpriteImage::sprites

    The sprite or sprites to draw. Sprites will be scaled to the size of this element.
*/

//TODO: Implicitly size element to size of first sprite?
QQuickSpriteImage::QQuickSpriteImage(QQuickItem *parent) :
    QQuickItem(parent)
    , m_node(0)
    , m_material(0)
    , m_spriteEngine(0)
    , m_pleaseReset(false)
    , m_running(true)
    , m_interpolate(true)
{
    setFlag(ItemHasContents);
    connect(this, SIGNAL(runningChanged(bool)),
            this, SLOT(update()));
}

QDeclarativeListProperty<QQuickSprite> QQuickSpriteImage::sprites()
{
    return QDeclarativeListProperty<QQuickSprite>(this, &m_sprites, spriteAppend, spriteCount, spriteAt, spriteClear);
}

void QQuickSpriteImage::createEngine()
{
    //TODO: delay until component complete
    if (m_spriteEngine)
        delete m_spriteEngine;
    if (m_sprites.count())
        m_spriteEngine = new QQuickSpriteEngine(m_sprites, this);
    else
        m_spriteEngine = 0;
    reset();
}

static QSGGeometry::Attribute SpriteImage_Attributes[] = {
    QSGGeometry::Attribute::create(0, 2, GL_FLOAT),         // tex
};

static QSGGeometry::AttributeSet SpriteImage_AttributeSet =
{
    1, // Attribute Count
    2 * sizeof(float),
    SpriteImage_Attributes
};

QSGGeometryNode* QQuickSpriteImage::buildNode()
{
    if (!m_spriteEngine) {
        qWarning() << "SpriteImage: No sprite engine...";
        return 0;
    }

    m_material = new QQuickSpriteMaterial();

    QImage image = m_spriteEngine->assembledImage();
    if (image.isNull())
        return 0;
    m_material->texture = sceneGraphEngine()->createTextureFromImage(image);
    m_material->texture->setFiltering(QSGTexture::Linear);
    m_spriteEngine->start(0);
    m_material->interpolate = m_interpolate ? 1.0 : 0.0;
    m_material->frameCount = m_spriteEngine->spriteFrames();
    m_material->frameDuration = m_spriteEngine->spriteDuration();
    m_material->animT = 0;
    m_material->animX = m_spriteEngine->spriteX();
    m_material->animY = m_spriteEngine->spriteY();
    m_material->animWidth = m_spriteEngine->spriteWidth();
    m_material->animHeight = m_spriteEngine->spriteHeight();
    m_material->sheetWidth = image.width();
    m_material->sheetHeight = image.height();
    m_material->elementWidth = width();
    m_material->elementHeight = height();

    int vCount = 4;
    int iCount = 6;
    QSGGeometry *g = new QSGGeometry(SpriteImage_AttributeSet, vCount, iCount);
    g->setDrawingMode(GL_TRIANGLES);

    SpriteVertices *p = (SpriteVertices *) g->vertexData();

    p->v1.tx = 0;
    p->v1.ty = 0;

    p->v2.tx = 1.0;
    p->v2.ty = 0;

    p->v3.tx = 0;
    p->v3.ty = 1.0;

    p->v4.tx = 1.0;
    p->v4.ty = 1.0;

    quint16 *indices = g->indexDataAsUShort();
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 1;
    indices[4] = 3;
    indices[5] = 2;


    m_timestamp.start();
    m_node = new QSGGeometryNode();
    m_node->setGeometry(g);
    m_node->setMaterial(m_material);
    m_node->setFlag(QSGGeometryNode::OwnsMaterial);
    return m_node;
}

void QQuickSpriteImage::reset()
{
    m_pleaseReset = true;
}

QSGNode *QQuickSpriteImage::updatePaintNode(QSGNode *, UpdatePaintNodeData *)
{
    if (m_pleaseReset) {
        delete m_node;
        delete m_material;

        m_node = 0;
        m_material = 0;
        m_pleaseReset = false;
    }

    prepareNextFrame();

    if (m_running) {
        update();
        if (m_node)
            m_node->markDirty(QSGNode::DirtyMaterial);
    }

    return m_node;
}

void QQuickSpriteImage::prepareNextFrame()
{
    if (m_node == 0)
        m_node = buildNode();
    if (m_node == 0) //error creating node
        return;

    uint timeInt = m_timestamp.elapsed();
    qreal time =  timeInt / 1000.;
    m_material->timestamp = time;
    m_material->elementHeight = height();
    m_material->elementWidth = width();
    m_material->interpolate = m_interpolate;

    //Advance State
    SpriteVertices *p = (SpriteVertices *) m_node->geometry()->vertexData();
    m_spriteEngine->updateSprites(timeInt);
    int curY = m_spriteEngine->spriteY();
    if (curY != m_material->animY){
        m_material->animT = m_spriteEngine->spriteStart()/1000.0;
        m_material->frameCount = m_spriteEngine->spriteFrames();
        m_material->frameDuration = m_spriteEngine->spriteDuration();
        m_material->animX = m_spriteEngine->spriteX();
        m_material->animY = m_spriteEngine->spriteY();
        m_material->animWidth = m_spriteEngine->spriteWidth();
        m_material->animHeight = m_spriteEngine->spriteHeight();
    }
}

QT_END_NAMESPACE