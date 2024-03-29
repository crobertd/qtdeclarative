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

#ifndef QQUICKPROPERTYCHANGES_H
#define QQUICKPROPERTYCHANGES_H

#include "qquickstatechangescript_p.h"
#include <private/qqmlcustomparser_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickPropertyChangesPrivate;
class Q_QUICK_PRIVATE_EXPORT QQuickPropertyChanges : public QQuickStateOperation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickPropertyChanges)

    Q_PROPERTY(QObject *target READ object WRITE setObject)
    Q_PROPERTY(bool restoreEntryValues READ restoreEntryValues WRITE setRestoreEntryValues)
    Q_PROPERTY(bool explicit READ isExplicit WRITE setIsExplicit)
public:
    QQuickPropertyChanges();
    ~QQuickPropertyChanges();

    QObject *object() const;
    void setObject(QObject *);

    bool restoreEntryValues() const;
    void setRestoreEntryValues(bool);

    bool isExplicit() const;
    void setIsExplicit(bool);

    virtual ActionList actions();

    bool containsProperty(const QString &name) const;
    bool containsValue(const QString &name) const;
    bool containsExpression(const QString &name) const;
    void changeValue(const QString &name, const QVariant &value);
    void changeExpression(const QString &name, const QString &expression);
    void removeProperty(const QString &name);
    QVariant value(const QString &name) const;
    QString expression(const QString &name) const;

    void detachFromState();
    void attachToState();

    QVariant property(const QString &name) const;
};

class QQuickPropertyChangesParser : public QQmlCustomParser
{
public:
    QQuickPropertyChangesParser()
    : QQmlCustomParser(AcceptsAttachedProperties) {}

    void compileList(QList<QPair<QString, QVariant> > &list, const QString &pre, const QQmlCustomParserProperty &prop);

    virtual QByteArray compile(const QList<QQmlCustomParserProperty> &);
    virtual void setCustomData(QObject *, const QByteArray &);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickPropertyChanges)

QT_END_HEADER

#endif // QQUICKPROPERTYCHANGES_H
