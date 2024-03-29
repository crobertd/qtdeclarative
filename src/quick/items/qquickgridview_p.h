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

#ifndef QQUICKGRIDVIEW_P_H
#define QQUICKGRIDVIEW_P_H

#include "qquickitemview_p.h"

#include <private/qqmlguard_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickVisualModel;
class QQuickGridViewAttached;
class QQuickGridViewPrivate;
class Q_AUTOTEST_EXPORT QQuickGridView : public QQuickItemView
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickGridView)

    Q_PROPERTY(Flow flow READ flow WRITE setFlow NOTIFY flowChanged)
    Q_PROPERTY(qreal cellWidth READ cellWidth WRITE setCellWidth NOTIFY cellWidthChanged)
    Q_PROPERTY(qreal cellHeight READ cellHeight WRITE setCellHeight NOTIFY cellHeightChanged)

    Q_PROPERTY(SnapMode snapMode READ snapMode WRITE setSnapMode NOTIFY snapModeChanged)

    Q_ENUMS(SnapMode)
    Q_ENUMS(Flow)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    enum Flow {
        FlowLeftToRight = LeftToRight,
        FlowTopToBottom = TopToBottom
    };

    QQuickGridView(QQuickItem *parent=0);
    ~QQuickGridView();

    virtual void setHighlightFollowsCurrentItem(bool);
    virtual void setHighlightMoveDuration(int);

    Flow flow() const;
    void setFlow(Flow);

    qreal cellWidth() const;
    void setCellWidth(qreal);

    qreal cellHeight() const;
    void setCellHeight(qreal);

    enum SnapMode { NoSnap, SnapToRow, SnapOneRow };
    SnapMode snapMode() const;
    void setSnapMode(SnapMode mode);

    static QQuickGridViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void moveCurrentIndexUp();
    void moveCurrentIndexDown();
    void moveCurrentIndexLeft();
    void moveCurrentIndexRight();

Q_SIGNALS:
    void cellWidthChanged();
    void cellHeightChanged();
    void highlightMoveDurationChanged();
    void flowChanged();
    void snapModeChanged();

protected:
    virtual void viewportMoved(Qt::Orientations);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual void initItem(int index, QQuickItem *item);
};

class QQuickGridViewAttached : public QQuickItemViewAttached
{
    Q_OBJECT
public:
    QQuickGridViewAttached(QObject *parent)
        : QQuickItemViewAttached(parent), m_view(0) {}
    ~QQuickGridViewAttached() {}

    Q_PROPERTY(QQuickGridView *view READ view NOTIFY viewChanged)
    QQuickGridView *view() { return m_view; }
    void setView(QQuickGridView *view) {
        if (view != m_view) {
            m_view = view;
            emit viewChanged();
        }
    }

Q_SIGNALS:
    void viewChanged();

public:
    QQmlGuard<QQuickGridView> m_view;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickGridView)
QML_DECLARE_TYPEINFO(QQuickGridView, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // QQUICKGRIDVIEW_P_H
