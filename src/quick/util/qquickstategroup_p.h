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

#ifndef QQUICKSTATEGROUP_H
#define QQUICKSTATEGROUP_H

#include "qquickstate_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickStateGroupPrivate;
class Q_QUICK_PRIVATE_EXPORT QQuickStateGroup : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_DECLARE_PRIVATE(QQuickStateGroup)

    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QQmlListProperty<QQuickState> states READ statesProperty DESIGNABLE false)
    Q_PROPERTY(QQmlListProperty<QQuickTransition> transitions READ transitionsProperty DESIGNABLE false)

public:
    QQuickStateGroup(QObject * = 0);
    virtual ~QQuickStateGroup();

    QString state() const;
    void setState(const QString &);

    QQmlListProperty<QQuickState> statesProperty();
    QList<QQuickState *> states() const;

    QQmlListProperty<QQuickTransition> transitionsProperty();

    QQuickState *findState(const QString &name) const;
    void removeState(QQuickState *state);

    virtual void classBegin();
    virtual void componentComplete();
Q_SIGNALS:
    void stateChanged(const QString &);

private:
    friend class QQuickState;
    friend class QQuickStatePrivate;
    bool updateAutoState();
    void stateAboutToComplete();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickStateGroup)

QT_END_HEADER

#endif // QQUICKSTATEGROUP_H
