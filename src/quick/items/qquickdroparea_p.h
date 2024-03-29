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

#ifndef QQUICKDROPAREA_P_H
#define QQUICKDROPAREA_P_H

#include "qquickitem.h"

#include <private/qqmlguard_p.h>
#include <private/qv8engine_p.h>

#include <QtGui/qevent.h>

#ifndef QT_NO_DRAGANDDROP

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickDropAreaPrivate;
class QQuickDropEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ x)
    Q_PROPERTY(qreal y READ y)
    Q_PROPERTY(QObject *source READ source)
    Q_PROPERTY(QStringList keys READ keys)
    Q_PROPERTY(Qt::DropActions supportedActions READ supportedActions)
    Q_PROPERTY(Qt::DropAction action READ action WRITE setAction RESET resetAction)
    Q_PROPERTY(bool accepted READ accepted WRITE setAccepted)
public:
    QQuickDropEvent(QQuickDropAreaPrivate *d, QDropEvent *event) : d(d), event(event) {}

    qreal x() const { return event->pos().x(); }
    qreal y() const { return event->pos().y(); }

    QObject *source();

    Qt::DropActions supportedActions() const { return event->possibleActions(); }
    Qt::DropAction action() const { return event->dropAction(); }
    void setAction(Qt::DropAction action) { event->setDropAction(action); }
    void resetAction() { event->setDropAction(event->proposedAction()); }

    QStringList keys() const;

    bool accepted() const { return event->isAccepted(); }
    void setAccepted(bool accepted) { event->setAccepted(accepted); }

    Q_INVOKABLE void accept(QQmlV8Function *);

private:
    QQuickDropAreaPrivate *d;
    QDropEvent *event;
};

class QQuickDropAreaDrag : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal x READ x NOTIFY positionChanged)
    Q_PROPERTY(qreal y READ y NOTIFY positionChanged)
    Q_PROPERTY(QObject *source READ source NOTIFY sourceChanged)
public:
    QQuickDropAreaDrag(QQuickDropAreaPrivate *d, QObject *parent = 0);
    ~QQuickDropAreaDrag();

    qreal x() const;
    qreal y() const;
    QObject *source() const;

Q_SIGNALS:
    void positionChanged();
    void sourceChanged();

private:
    QQuickDropAreaPrivate *d;

    friend class QQuickDropArea;
    friend class QQuickDropAreaPrivate;
};

class QQuickDropAreaPrivate;
class Q_AUTOTEST_EXPORT QQuickDropArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool containsDrag READ containsDrag NOTIFY containsDragChanged)
    Q_PROPERTY(QStringList keys READ keys WRITE setKeys NOTIFY keysChanged)
    Q_PROPERTY(QQuickDropAreaDrag *drag READ drag CONSTANT)

public:
    QQuickDropArea(QQuickItem *parent=0);
    ~QQuickDropArea();

    bool containsDrag() const;
    void setContainsDrag(bool drag);

    QStringList keys() const;
    void setKeys(const QStringList &keys);

    QQuickDropAreaDrag *drag();

Q_SIGNALS:
    void containsDragChanged();
    void keysChanged();
    void sourceChanged();

    void entered(QQuickDropEvent *drag);
    void exited();
    void positionChanged(QQuickDropEvent *drag);
    void dropped(QQuickDropEvent *drop);

protected:
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);

private:
    Q_DISABLE_COPY(QQuickDropArea)
    Q_DECLARE_PRIVATE(QQuickDropArea)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickDropEvent)
QML_DECLARE_TYPE(QQuickDropArea)

QT_END_HEADER

#endif // QT_NO_DRAGANDDROP

#endif // QQUICKDROPAREA_P_H
