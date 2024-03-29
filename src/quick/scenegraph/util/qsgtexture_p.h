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

#ifndef QSGTEXTURE_P_H
#define QSGTEXTURE_P_H

#include <QtQuick/qtquickglobal.h>
#include <private/qobject_p.h>

#include <QtGui/qopengl.h>

#include "qsgtexture.h"
#include <QtQuick/private/qsgcontext_p.h>

QT_BEGIN_NAMESPACE

class QSGTexturePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QSGTexture);
public:
    QSGTexturePrivate();

    uint wrapChanged : 1;
    uint filteringChanged : 1;

    uint horizontalWrap : 1;
    uint verticalWrap : 1;
    uint mipmapMode : 2;
    uint filterMode : 2;
};

class Q_QUICK_PRIVATE_EXPORT QSGPlainTexture : public QSGTexture
{
    Q_OBJECT
public:
    QSGPlainTexture();
    virtual ~QSGPlainTexture();

    void setOwnsTexture(bool owns) { m_owns_texture = owns; }
    bool ownsTexture() const { return m_owns_texture; }

    void setTextureId(int id);
    int textureId() const;
    void setTextureSize(const QSize &size) { m_texture_size = size; }
    QSize textureSize() const { return m_texture_size; }

    void setHasAlphaChannel(bool alpha) { m_has_alpha = alpha; }
    bool hasAlphaChannel() const { return m_has_alpha; }

    void setHasMipmaps(bool mm);
    bool hasMipmaps() const { return m_has_mipmaps; }

    void setImage(const QImage &image);
    const QImage &image() { return m_image; }

    virtual void bind();

    static QSGPlainTexture *fromImage(const QImage &image) {
        QSGPlainTexture *t = new QSGPlainTexture();
        t->setImage(image);
        return t;
    }

protected:
    QImage m_image;

    GLuint m_texture_id;
    QSize m_texture_size;
    QRectF m_texture_rect;

    uint m_has_alpha : 1;
    uint m_has_mipmaps : 1;
    uint m_dirty_texture : 1;
    uint m_dirty_bind_options : 1;
    uint m_owns_texture : 1;
    uint m_mipmaps_generated : 1;
};

QT_END_NAMESPACE

#endif // QSGTEXTURE_P_H
