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

#include "qquicklistaccessor_p.h"

#include <private/qqmlmetatype_p.h>

#include <QtCore/qstringlist.h>
#include <QtCore/qdebug.h>

// ### Remove me
#include <private/qqmlengine_p.h>

QT_BEGIN_NAMESPACE

QQuickListAccessor::QQuickListAccessor()
: m_type(Invalid)
{
}

QQuickListAccessor::~QQuickListAccessor()
{
}

QVariant QQuickListAccessor::list() const
{
    return d;
}

void QQuickListAccessor::setList(const QVariant &v, QQmlEngine *engine)
{
    d = v;

    QQmlEnginePrivate *enginePrivate = engine?QQmlEnginePrivate::get(engine):0;

    if (!d.isValid()) {
        m_type = Invalid;
    } else if (d.userType() == QVariant::StringList) {
        m_type = StringList;
    } else if (d.userType() == QMetaType::QVariantList) {
        m_type = VariantList;
    } else if (d.canConvert(QVariant::Int)) {
        m_type = Integer;
    } else if ((!enginePrivate && QQmlMetaType::isQObject(d.userType())) ||
               (enginePrivate && enginePrivate->isQObject(d.userType()))) {
        QObject *data = enginePrivate?enginePrivate->toQObject(v):QQmlMetaType::toQObject(v);
        d = QVariant::fromValue(data);
        m_type = Instance;
    } else if (d.userType() == qMetaTypeId<QQmlListReference>()) {
        m_type = ListProperty;
    } else {
        m_type = Instance;
    }
}

int QQuickListAccessor::count() const
{
    switch(m_type) {
    case StringList:
        return qvariant_cast<QStringList>(d).count();
    case VariantList:
        return qvariant_cast<QVariantList>(d).count();
    case ListProperty:
        return ((QQmlListReference *)d.constData())->count();
    case Instance:
        return 1;
    case Integer:
        return d.toInt();
    default:
    case Invalid:
        return 0;
    }
}

QVariant QQuickListAccessor::at(int idx) const
{
    Q_ASSERT(idx >= 0 && idx < count());
    switch(m_type) {
    case StringList:
        return QVariant::fromValue(qvariant_cast<QStringList>(d).at(idx));
    case VariantList:
        return qvariant_cast<QVariantList>(d).at(idx);
    case ListProperty:
        return QVariant::fromValue(((QQmlListReference *)d.constData())->at(idx));
    case Instance:
        return d;
    case Integer:
        return QVariant(idx);
    default:
    case Invalid:
        return QVariant();
    }
}

bool QQuickListAccessor::isValid() const
{
    return m_type != Invalid;
}

QT_END_NAMESPACE
