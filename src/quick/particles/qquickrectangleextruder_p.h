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

#ifndef RECTANGLEEXTRUDER_H
#define RECTANGLEEXTRUDER_H

#include "qquickparticleextruder_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickRectangleExtruder : public QQuickParticleExtruder
{
    Q_OBJECT
    Q_PROPERTY(bool fill READ fill WRITE setFill NOTIFY fillChanged)

public:
    explicit QQuickRectangleExtruder(QObject *parent = 0);
    virtual QPointF extrude(const QRectF &);
    virtual bool contains(const QRectF &bounds, const QPointF &point);
    bool fill() const
    {
        return m_fill;
    }

signals:

    void fillChanged(bool arg);

public slots:

    void setFill(bool arg)
    {
        if (m_fill != arg) {
            m_fill = arg;
            emit fillChanged(arg);
        }
    }
protected:
    bool m_fill;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // RectangleEXTRUDER_H