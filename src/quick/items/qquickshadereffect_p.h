/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QQUICKSHADEREFFECT_P_H
#define QQUICKSHADEREFFECT_P_H

#include <QtQuick/qquickitem.h>

#include <QtQuick/qsgmaterial.h>
#include <private/qtquickglobal_p.h>
#include <private/qsgadaptationlayer_p.h>
#include <private/qquickshadereffectnode_p.h>
#include "qquickshadereffectmesh_p.h"

#include <QtCore/qpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

const char *qtPositionAttributeName();
const char *qtTexCoordAttributeName();

class QSGContext;
class QSignalMapper;
class QQuickCustomMaterialShader;

// Common class for QQuickShaderEffect and QQuickCustomParticle.
struct Q_QUICK_PRIVATE_EXPORT QQuickShaderEffectCommon
{
    typedef QQuickShaderEffectMaterialKey Key;
    typedef QQuickShaderEffectMaterial::UniformData UniformData;

    ~QQuickShaderEffectCommon();
    void disconnectPropertySignals(QQuickItem *item, Key::ShaderType shaderType);
    void connectPropertySignals(QQuickItem *item, Key::ShaderType shaderType);
    void updateParseLog(bool ignoreAttributes);
    void lookThroughShaderCode(QQuickItem *item, Key::ShaderType shaderType, const QByteArray &code);
    void updateShader(QQuickItem *item, Key::ShaderType shaderType);
    void updateMaterial(QQuickShaderEffectNode *node, QQuickShaderEffectMaterial *material,
                        bool updateUniforms, bool updateUniformValues, bool updateTextureProviders);
    void updateWindow(QQuickWindow *window);

    // Called by slots in QQuickShaderEffect:
    void sourceDestroyed(QObject *object);
    void propertyChanged(QQuickItem *item, int mappedId, bool *textureProviderChanged);

    Key source;
    QVector<QByteArray> attributes;
    QVector<UniformData> uniformData[Key::ShaderTypeCount];
    QVector<QSignalMapper *> signalMappers[Key::ShaderTypeCount];
    QString parseLog;
};


class Q_QUICK_PRIVATE_EXPORT QQuickShaderEffect : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QByteArray fragmentShader READ fragmentShader WRITE setFragmentShader NOTIFY fragmentShaderChanged)
    Q_PROPERTY(QByteArray vertexShader READ vertexShader WRITE setVertexShader NOTIFY vertexShaderChanged)
    Q_PROPERTY(bool blending READ blending WRITE setBlending NOTIFY blendingChanged)
    Q_PROPERTY(QVariant mesh READ mesh WRITE setMesh NOTIFY meshChanged)
    Q_PROPERTY(CullMode cullMode READ cullMode WRITE setCullMode NOTIFY cullModeChanged)
    Q_PROPERTY(QString log READ log NOTIFY logChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_ENUMS(CullMode)
    Q_ENUMS(Status)

public:
    enum CullMode
    {
        NoCulling = QQuickShaderEffectMaterial::NoCulling,
        BackFaceCulling = QQuickShaderEffectMaterial::BackFaceCulling,
        FrontFaceCulling = QQuickShaderEffectMaterial::FrontFaceCulling
    };

    enum Status
    {
        Compiled,
        Uncompiled,
        Error
    };

    QQuickShaderEffect(QQuickItem *parent = 0);
    ~QQuickShaderEffect();

    QByteArray fragmentShader() const { return m_common.source.sourceCode[Key::FragmentShader]; }
    void setFragmentShader(const QByteArray &code);

    QByteArray vertexShader() const { return m_common.source.sourceCode[Key::VertexShader]; }
    void setVertexShader(const QByteArray &code);

    bool blending() const { return m_blending; }
    void setBlending(bool enable);

    QVariant mesh() const;
    void setMesh(const QVariant &mesh);

    CullMode cullMode() const { return m_cullMode; }
    void setCullMode(CullMode face);

    QString log() const { return m_log; }
    Status status() const { return m_status; }

    QString parseLog();

    virtual bool event(QEvent *);

Q_SIGNALS:
    void fragmentShaderChanged();
    void vertexShaderChanged();
    void blendingChanged();
    void meshChanged();
    void cullModeChanged();
    void logChanged();
    void statusChanged();

protected:
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    virtual void componentComplete();
    virtual void itemChange(ItemChange change, const ItemChangeData &value);

private Q_SLOTS:
    void updateGeometry();
    void updateLogAndStatus(const QString &log, int status);
    void sourceDestroyed(QObject *object);
    void propertyChanged(int mappedId);

private:
    friend class QQuickCustomMaterialShader;
    friend class QQuickShaderEffectNode;

    typedef QQuickShaderEffectMaterialKey Key;
    typedef QQuickShaderEffectMaterial::UniformData UniformData;

    QSize m_meshResolution;
    QQuickShaderEffectMesh *m_mesh;
    QQuickGridMesh m_defaultMesh;
    CullMode m_cullMode;
    QString m_log;
    Status m_status;

    QQuickShaderEffectCommon m_common;

    uint m_blending : 1;
    uint m_dirtyUniforms : 1;
    uint m_dirtyUniformValues : 1;
    uint m_dirtyTextureProviders : 1;
    uint m_dirtyProgram : 1;
    uint m_dirtyParseLog : 1;
    uint m_dirtyMesh : 1;
    uint m_dirtyGeometry : 1;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QQUICKSHADEREFFECT_P_H
