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

#ifndef QQUICKCANVASCONTEXT_P_H
#define QQUICKCANVASCONTEXT_P_H

#include <QtQuick/qquickitem.h>
#include <private/qv8engine_p.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickCanvasItem;
class QSGDynamicTexture;

class QQuickCanvasContextPrivate;
class QQuickCanvasContext : public QObject
{
    Q_OBJECT

public:
    QQuickCanvasContext(QObject *parent = 0);
    ~QQuickCanvasContext();

    virtual QStringList contextNames() const = 0;

    // Init (ignore options if necessary)
    virtual void init(QQuickCanvasItem *canvasItem, const QVariantMap &args) = 0;

    virtual void prepare(const QSize& canvasSize, const QSize& tileSize, const QRect& canvasWindow, const QRect& dirtyRect, bool smooth, bool antialiasing);
    virtual void flush();

    virtual void setV8Engine(QV8Engine *engine) = 0;
    virtual v8::Handle<v8::Object> v8value() const = 0;

    virtual QSGDynamicTexture *texture() const = 0;

    virtual QImage toImage(const QRectF& bounds) = 0;

Q_SIGNALS:
    void textureChanged();
};

QT_END_NAMESPACE

QT_END_HEADER

#endif //QQUICKCANVASCONTEXT_P_H
