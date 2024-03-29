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

#ifndef QJSVALUE_H
#define QJSVALUE_H

#include <QtQml/qtqmlglobal.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qsharedpointer.h>
#include <QtCore/qshareddata.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QJSValue;
class QJSEngine;
class QVariant;
class QObject;
struct QMetaObject;
class QDateTime;

typedef QList<QJSValue> QJSValueList;

class QJSValuePrivate;
struct QScriptValuePrivatePointerDeleter;
template <class T> class QScriptPassPointer;

class Q_QML_EXPORT QJSValue
{
public:
    enum SpecialValue {
        NullValue,
        UndefinedValue
    };

public:
    QJSValue(SpecialValue value = UndefinedValue);
    ~QJSValue();
    QJSValue(const QJSValue &other);

    QJSValue(bool value);
    QJSValue(int value);
    QJSValue(uint value);
    QJSValue(double value);
    QJSValue(const QString &value);
    QJSValue(const QLatin1String &value);
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN QJSValue(const char *str);
#endif

    QJSValue &operator=(const QJSValue &other);

    bool isBool() const;
    bool isNumber() const;
    bool isNull() const;
    bool isString() const;
    bool isUndefined() const;
    bool isVariant() const;
    bool isQObject() const;
    bool isObject() const;
    bool isDate() const;
    bool isRegExp() const;
    bool isArray() const;
    bool isError() const;

    QString toString() const;
    double toNumber() const;
    qint32 toInt() const;
    quint32 toUInt() const;
    bool toBool() const;
    QVariant toVariant() const;
    QObject *toQObject() const;
    QDateTime toDateTime() const;

    bool equals(const QJSValue &other) const;
    bool strictlyEquals(const QJSValue &other) const;

    QJSValue prototype() const;
    void setPrototype(const QJSValue &prototype);

    QJSValue property(const QString &name) const;
    void setProperty(const QString &name, const QJSValue &value);

    bool hasProperty(const QString &name) const;
    bool hasOwnProperty(const QString &name) const;

    QJSValue property(quint32 arrayIndex) const;
    void setProperty(quint32 arrayIndex, const QJSValue &value);

    bool deleteProperty(const QString &name);

    bool isCallable() const;
    QJSValue call(const QJSValueList &args = QJSValueList());
    QJSValue callWithInstance(const QJSValue &instance, const QJSValueList &args = QJSValueList());
    QJSValue callAsConstructor(const QJSValueList &args = QJSValueList());

#ifdef QT_DEPRECATED
    QT_DEPRECATED QJSEngine *engine() const;
#endif

private:
    // force compile error, prevent QJSValue(bool) to be called

    QJSValue(void *) Q_DECL_EQ_DELETE;

    QJSValue(QJSValuePrivate*);
    QJSValue(QScriptPassPointer<QJSValuePrivate>);

private:
    QExplicitlySharedDataPointer<QJSValuePrivate> d_ptr;

    Q_DECLARE_PRIVATE(QJSValue)
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QJSValue)

QT_END_HEADER

#endif
