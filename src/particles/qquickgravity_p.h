/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQuick module of the Qt Toolkit.
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

#ifndef GRAVITYAFFECTOR_H
#define GRAVITYAFFECTOR_H
#include "qquickparticleaffector_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickGravityAffector : public QQuickParticleAffector
{
    Q_OBJECT
    Q_PROPERTY(qreal magnitude READ magnitude WRITE setMagnitude NOTIFY magnitudeChanged)
    Q_PROPERTY(qreal acceleration READ magnitude WRITE setAcceleration NOTIFY magnitudeChanged)
    Q_PROPERTY(qreal angle READ angle WRITE setAngle NOTIFY angleChanged)
public:
    explicit QQuickGravityAffector(QQuickItem *parent = 0);
    qreal magnitude() const
    {
        return m_magnitude;
    }

    qreal angle() const
    {
        return m_angle;
    }
protected:
    virtual bool affectParticle(QQuickParticleData *d, qreal dt);
signals:

    void magnitudeChanged(qreal arg);

    void angleChanged(qreal arg);

public slots:
void setAcceleration(qreal arg)
{
    qWarning() << "Gravity::acceleration has been renamed Gravity::magnitude";
    if (m_magnitude != arg) {
        m_magnitude = arg;
        m_needRecalc = true;
        emit magnitudeChanged(arg);
    }
}

void setMagnitude(qreal arg)
{
    if (m_magnitude != arg) {
        m_magnitude = arg;
        m_needRecalc = true;
        emit magnitudeChanged(arg);
    }
}

void setAngle(qreal arg)
{
    if (m_angle != arg) {
        m_angle = arg;
        m_needRecalc = true;
        emit angleChanged(arg);
    }
}

private:
    qreal m_magnitude;
    qreal m_angle;

    bool m_needRecalc;
    qreal m_dx;
    qreal m_dy;
};

QT_END_NAMESPACE
QT_END_HEADER
#endif // GRAVITYAFFECTOR_H
