/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtSG module of the Qt Toolkit.
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

#ifndef QQUICKPINCHAREA_H
#define QQUICKPINCHAREA_H

#include "qquickitem.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QQuickPinch : public QObject
{
    Q_OBJECT

    Q_ENUMS(Axis)
    Q_PROPERTY(QQuickItem *target READ target WRITE setTarget RESET resetTarget)
    Q_PROPERTY(qreal minimumScale READ minimumScale WRITE setMinimumScale NOTIFY minimumScaleChanged)
    Q_PROPERTY(qreal maximumScale READ maximumScale WRITE setMaximumScale NOTIFY maximumScaleChanged)
    Q_PROPERTY(qreal minimumRotation READ minimumRotation WRITE setMinimumRotation NOTIFY minimumRotationChanged)
    Q_PROPERTY(qreal maximumRotation READ maximumRotation WRITE setMaximumRotation NOTIFY maximumRotationChanged)
    Q_PROPERTY(Axis dragAxis READ axis WRITE setAxis NOTIFY dragAxisChanged)
    Q_PROPERTY(qreal minimumX READ xmin WRITE setXmin NOTIFY minimumXChanged)
    Q_PROPERTY(qreal maximumX READ xmax WRITE setXmax NOTIFY maximumXChanged)
    Q_PROPERTY(qreal minimumY READ ymin WRITE setYmin NOTIFY minimumYChanged)
    Q_PROPERTY(qreal maximumY READ ymax WRITE setYmax NOTIFY maximumYChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)

public:
    QQuickPinch();

    QQuickItem *target() const { return m_target; }
    void setTarget(QQuickItem *target) {
        if (target == m_target)
            return;
        m_target = target;
        emit targetChanged();
    }
    void resetTarget() {
        if (!m_target)
            return;
        m_target = 0;
        emit targetChanged();
    }

    qreal minimumScale() const { return m_minScale; }
    void setMinimumScale(qreal s) {
        if (s == m_minScale)
            return;
        m_minScale = s;
        emit minimumScaleChanged();
    }
    qreal maximumScale() const { return m_maxScale; }
    void setMaximumScale(qreal s) {
        if (s == m_maxScale)
            return;
        m_maxScale = s;
        emit maximumScaleChanged();
    }

    qreal minimumRotation() const { return m_minRotation; }
    void setMinimumRotation(qreal r) {
        if (r == m_minRotation)
            return;
        m_minRotation = r;
        emit minimumRotationChanged();
    }
    qreal maximumRotation() const { return m_maxRotation; }
    void setMaximumRotation(qreal r) {
        if (r == m_maxRotation)
            return;
        m_maxRotation = r;
        emit maximumRotationChanged();
    }

    enum Axis { NoDrag=0x00, XAxis=0x01, YAxis=0x02, XAndYAxis=0x03, XandYAxis=XAndYAxis };
    Axis axis() const { return m_axis; }
    void setAxis(Axis a) {
        if (a == m_axis)
            return;
        m_axis = a;
        emit dragAxisChanged();
    }

    qreal xmin() const { return m_xmin; }
    void setXmin(qreal x) {
        if (x == m_xmin)
            return;
        m_xmin = x;
        emit minimumXChanged();
    }
    qreal xmax() const { return m_xmax; }
    void setXmax(qreal x) {
        if (x == m_xmax)
            return;
        m_xmax = x;
        emit maximumXChanged();
    }
    qreal ymin() const { return m_ymin; }
    void setYmin(qreal y) {
        if (y == m_ymin)
            return;
        m_ymin = y;
        emit minimumYChanged();
    }
    qreal ymax() const { return m_ymax; }
    void setYmax(qreal y) {
        if (y == m_ymax)
            return;
        m_ymax = y;
        emit maximumYChanged();
    }

    bool active() const { return m_active; }
    void setActive(bool a) {
        if (a == m_active)
            return;
        m_active = a;
        emit activeChanged();
    }

signals:
    void targetChanged();
    void minimumScaleChanged();
    void maximumScaleChanged();
    void minimumRotationChanged();
    void maximumRotationChanged();
    void dragAxisChanged();
    void minimumXChanged();
    void maximumXChanged();
    void minimumYChanged();
    void maximumYChanged();
    void activeChanged();

private:
    QQuickItem *m_target;
    qreal m_minScale;
    qreal m_maxScale;
    qreal m_minRotation;
    qreal m_maxRotation;
    Axis m_axis;
    qreal m_xmin;
    qreal m_xmax;
    qreal m_ymin;
    qreal m_ymax;
    bool m_active;
};

class Q_AUTOTEST_EXPORT QQuickPinchEvent : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPointF center READ center)
    Q_PROPERTY(QPointF startCenter READ startCenter)
    Q_PROPERTY(QPointF previousCenter READ previousCenter)
    Q_PROPERTY(qreal scale READ scale)
    Q_PROPERTY(qreal previousScale READ previousScale)
    Q_PROPERTY(qreal angle READ angle)
    Q_PROPERTY(qreal previousAngle READ previousAngle)
    Q_PROPERTY(qreal rotation READ rotation)
    Q_PROPERTY(QPointF point1 READ point1)
    Q_PROPERTY(QPointF startPoint1 READ startPoint1)
    Q_PROPERTY(QPointF point2 READ point2)
    Q_PROPERTY(QPointF startPoint2 READ startPoint2)
    Q_PROPERTY(int pointCount READ pointCount)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)

public:
    QQuickPinchEvent(QPointF c, qreal s, qreal a, qreal r)
        : QObject(), m_center(c), m_scale(s), m_angle(a), m_rotation(r)
        , m_pointCount(0), m_accepted(true) {}

    QPointF center() const { return m_center; }
    QPointF startCenter() const { return m_startCenter; }
    void setStartCenter(QPointF c) { m_startCenter = c; }
    QPointF previousCenter() const { return m_lastCenter; }
    void setPreviousCenter(QPointF c) { m_lastCenter = c; }
    qreal scale() const { return m_scale; }
    qreal previousScale() const { return m_lastScale; }
    void setPreviousScale(qreal s) { m_lastScale = s; }
    qreal angle() const { return m_angle; }
    qreal previousAngle() const { return m_lastAngle; }
    void setPreviousAngle(qreal a) { m_lastAngle = a; }
    qreal rotation() const { return m_rotation; }
    QPointF point1() const { return m_point1; }
    void setPoint1(QPointF p) { m_point1 = p; }
    QPointF startPoint1() const { return m_startPoint1; }
    void setStartPoint1(QPointF p) { m_startPoint1 = p; }
    QPointF point2() const { return m_point2; }
    void setPoint2(QPointF p) { m_point2 = p; }
    QPointF startPoint2() const { return m_startPoint2; }
    void setStartPoint2(QPointF p) { m_startPoint2 = p; }
    int pointCount() const { return m_pointCount; }
    void setPointCount(int count) { m_pointCount = count; }

    bool accepted() const { return m_accepted; }
    void setAccepted(bool a) { m_accepted = a; }

private:
    QPointF m_center;
    QPointF m_startCenter;
    QPointF m_lastCenter;
    qreal m_scale;
    qreal m_lastScale;
    qreal m_angle;
    qreal m_lastAngle;
    qreal m_rotation;
    QPointF m_point1;
    QPointF m_point2;
    QPointF m_startPoint1;
    QPointF m_startPoint2;
    int m_pointCount;
    bool m_accepted;
};


class QQuickMouseEvent;
class QQuickPinchAreaPrivate;
class Q_AUTOTEST_EXPORT QQuickPinchArea : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QQuickPinch *pinch READ pinch CONSTANT)

public:
    QQuickPinchArea(QQuickItem *parent=0);
    ~QQuickPinchArea();

    bool isEnabled() const;
    void setEnabled(bool);

    QQuickPinch *pinch();

Q_SIGNALS:
    void enabledChanged();
    void pinchStarted(QQuickPinchEvent *pinch);
    void pinchUpdated(QQuickPinchEvent *pinch);
    void pinchFinished(QQuickPinchEvent *pinch);

protected:
    virtual bool childMouseEventFilter(QQuickItem *i, QEvent *e);
    virtual void touchEvent(QTouchEvent *event);

    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    virtual void itemChange(ItemChange change, const ItemChangeData& value);

private:
    void updatePinch();
    void handlePress();
    void handleRelease();

private:
    Q_DISABLE_COPY(QQuickPinchArea)
    Q_DECLARE_PRIVATE(QQuickPinchArea)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickPinch)
QML_DECLARE_TYPE(QQuickPinchEvent)
QML_DECLARE_TYPE(QQuickPinchArea)

QT_END_HEADER

#endif // QQUICKPINCHAREA_H

