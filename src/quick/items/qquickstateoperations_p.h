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

#ifndef QQUICKSTATEOPERATIONS_P_H
#define QQUICKSTATEOPERATIONS_P_H

#include "qquickitem.h"
#include "qquickanchors_p.h"

#include <QtQuick/private/qquickstate_p.h>

#include <QtQml/qqmlscriptstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickParentChangePrivate;
class Q_AUTOTEST_EXPORT QQuickParentChange : public QQuickStateOperation, public QQuickActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickParentChange)

    Q_PROPERTY(QQuickItem *target READ object WRITE setObject)
    Q_PROPERTY(QQuickItem *parent READ parent WRITE setParent)
    Q_PROPERTY(QQmlScriptString x READ x WRITE setX)
    Q_PROPERTY(QQmlScriptString y READ y WRITE setY)
    Q_PROPERTY(QQmlScriptString width READ width WRITE setWidth)
    Q_PROPERTY(QQmlScriptString height READ height WRITE setHeight)
    Q_PROPERTY(QQmlScriptString scale READ scale WRITE setScale)
    Q_PROPERTY(QQmlScriptString rotation READ rotation WRITE setRotation)
public:
    QQuickParentChange(QObject *parent=0);
    ~QQuickParentChange();

    QQuickItem *object() const;
    void setObject(QQuickItem *);

    QQuickItem *parent() const;
    void setParent(QQuickItem *);

    QQuickItem *originalParent() const;

    QQmlScriptString x() const;
    void setX(QQmlScriptString x);
    bool xIsSet() const;

    QQmlScriptString y() const;
    void setY(QQmlScriptString y);
    bool yIsSet() const;

    QQmlScriptString width() const;
    void setWidth(QQmlScriptString width);
    bool widthIsSet() const;

    QQmlScriptString height() const;
    void setHeight(QQmlScriptString height);
    bool heightIsSet() const;

    QQmlScriptString scale() const;
    void setScale(QQmlScriptString scale);
    bool scaleIsSet() const;

    QQmlScriptString rotation() const;
    void setRotation(QQmlScriptString rotation);
    bool rotationIsSet() const;

    virtual ActionList actions();

    virtual void saveOriginals();
    //virtual void copyOriginals(QQuickActionEvent*);
    virtual void execute(Reason reason = ActualChange);
    virtual bool isReversable();
    virtual void reverse(Reason reason = ActualChange);
    virtual EventType type() const;
    virtual bool override(QQuickActionEvent*other);
    virtual void rewind();
    virtual void saveCurrentValues();
};

class QQuickAnchorChanges;
class QQuickAnchorSetPrivate;
class Q_AUTOTEST_EXPORT QQuickAnchorSet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlScriptString left READ left WRITE setLeft RESET resetLeft)
    Q_PROPERTY(QQmlScriptString right READ right WRITE setRight RESET resetRight)
    Q_PROPERTY(QQmlScriptString horizontalCenter READ horizontalCenter WRITE setHorizontalCenter RESET resetHorizontalCenter)
    Q_PROPERTY(QQmlScriptString top READ top WRITE setTop RESET resetTop)
    Q_PROPERTY(QQmlScriptString bottom READ bottom WRITE setBottom RESET resetBottom)
    Q_PROPERTY(QQmlScriptString verticalCenter READ verticalCenter WRITE setVerticalCenter RESET resetVerticalCenter)
    Q_PROPERTY(QQmlScriptString baseline READ baseline WRITE setBaseline RESET resetBaseline)

public:
    QQuickAnchorSet(QObject *parent=0);
    virtual ~QQuickAnchorSet();

    QQmlScriptString left() const;
    void setLeft(const QQmlScriptString &edge);
    void resetLeft();

    QQmlScriptString right() const;
    void setRight(const QQmlScriptString &edge);
    void resetRight();

    QQmlScriptString horizontalCenter() const;
    void setHorizontalCenter(const QQmlScriptString &edge);
    void resetHorizontalCenter();

    QQmlScriptString top() const;
    void setTop(const QQmlScriptString &edge);
    void resetTop();

    QQmlScriptString bottom() const;
    void setBottom(const QQmlScriptString &edge);
    void resetBottom();

    QQmlScriptString verticalCenter() const;
    void setVerticalCenter(const QQmlScriptString &edge);
    void resetVerticalCenter();

    QQmlScriptString baseline() const;
    void setBaseline(const QQmlScriptString &edge);
    void resetBaseline();

    QQuickAnchors::Anchors usedAnchors() const;

private:
    friend class QQuickAnchorChanges;
    Q_DISABLE_COPY(QQuickAnchorSet)
    Q_DECLARE_PRIVATE(QQuickAnchorSet)
};

class QQuickAnchorChangesPrivate;
class Q_AUTOTEST_EXPORT QQuickAnchorChanges : public QQuickStateOperation, public QQuickActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickAnchorChanges)

    Q_PROPERTY(QQuickItem *target READ object WRITE setObject)
    Q_PROPERTY(QQuickAnchorSet *anchors READ anchors CONSTANT)

public:
    QQuickAnchorChanges(QObject *parent=0);
    ~QQuickAnchorChanges();

    virtual ActionList actions();

    QQuickAnchorSet *anchors();

    QQuickItem *object() const;
    void setObject(QQuickItem *);

    virtual void execute(Reason reason = ActualChange);
    virtual bool isReversable();
    virtual void reverse(Reason reason = ActualChange);
    virtual EventType type() const;
    virtual bool override(QQuickActionEvent*other);
    virtual bool changesBindings();
    virtual void saveOriginals();
    virtual bool needsCopy() { return true; }
    virtual void copyOriginals(QQuickActionEvent*);
    virtual void clearBindings();
    virtual void rewind();
    virtual void saveCurrentValues();

    QList<QQuickAction> additionalActions();
    virtual void saveTargetValues();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickParentChange)
QML_DECLARE_TYPE(QQuickAnchorSet)
QML_DECLARE_TYPE(QQuickAnchorChanges)

QT_END_HEADER

#endif // QQUICKSTATEOPERATIONS_P_H

