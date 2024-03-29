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

#ifndef QQUICKPACKAGE_H
#define QQUICKPACKAGE_H

#include <qqml.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickPackagePrivate;
class QQuickPackageAttached;
class Q_AUTOTEST_EXPORT QQuickPackage : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQuickPackage)

    Q_CLASSINFO("DefaultProperty", "data")
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)

public:
    QQuickPackage(QObject *parent=0);
    virtual ~QQuickPackage();

    QQmlListProperty<QObject> data();

    QObject *part(const QString & = QString());
    bool hasPart(const QString &);

    static QQuickPackageAttached *qmlAttachedProperties(QObject *);
};

class QQuickPackageAttached : public QObject
{
Q_OBJECT
Q_PROPERTY(QString name READ name WRITE setName)
public:
    QQuickPackageAttached(QObject *parent);
    virtual ~QQuickPackageAttached();

    QString name() const;
    void setName(const QString &n);

    static QHash<QObject *, QQuickPackageAttached *> attached;
private:
    QString _name;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickPackage)
QML_DECLARE_TYPEINFO(QQuickPackage, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // QQUICKPACKAGE_H
