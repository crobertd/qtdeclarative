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


#ifndef DEFAULT_RECTANGLENODE_H
#define DEFAULT_RECTANGLENODE_H

#include <private/qsgadaptationlayer_p.h>

#include <QtQuick/qsgvertexcolormaterial.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGContext;

class SmoothColorMaterial : public QSGMaterial
{
public:
    SmoothColorMaterial();

    int compare(const QSGMaterial *other) const;

protected:
    virtual QSGMaterialType *type() const;
    virtual QSGMaterialShader *createShader() const;
};

class QSGDefaultRectangleNode : public QSGRectangleNode
{
public:
    QSGDefaultRectangleNode();

    virtual void setRect(const QRectF &rect);
    virtual void setColor(const QColor &color);
    virtual void setPenColor(const QColor &color);
    virtual void setPenWidth(qreal width);
    virtual void setGradientStops(const QGradientStops &stops);
    virtual void setRadius(qreal radius);
    virtual void setAntialiasing(bool antialiasing);
    virtual void setAligned(bool aligned);
    virtual void update();

private:
    void updateGeometry();
    void updateGradientTexture();

    QSGVertexColorMaterial m_material;
    SmoothColorMaterial m_smoothMaterial;

    QRectF m_rect;
    QGradientStops m_gradient_stops;
    QColor m_color;
    QColor m_border_color;
    qreal m_radius;
    qreal m_pen_width;

    uint m_aligned : 1;
    uint m_antialiasing : 1;
    uint m_gradient_is_opaque : 1;
    uint m_dirty_geometry : 1;

    QSGGeometry m_geometry;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif
