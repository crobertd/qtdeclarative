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

#ifndef QQMLLIST_H
#define QQMLLIST_H

#include <QtQml/qtqmlglobal.h>
#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QObject;
struct QMetaObject;

#ifndef QQMLLISTPROPERTY
#define QQMLLISTPROPERTY
template<typename T>
class QQmlListProperty {
public:
    typedef void (*AppendFunction)(QQmlListProperty<T> *, T*);
    typedef int (*CountFunction)(QQmlListProperty<T> *);
    typedef T *(*AtFunction)(QQmlListProperty<T> *, int);
    typedef void (*ClearFunction)(QQmlListProperty<T> *);

    QQmlListProperty()
        : object(0), data(0), append(0), count(0), at(0), clear(0), dummy1(0), dummy2(0) {}
    QQmlListProperty(QObject *o, QList<T *> &list)
        : object(o), data(&list), append(qlist_append), count(qlist_count), at(qlist_at),
          clear(qlist_clear), dummy1(0), dummy2(0) {}
    QQmlListProperty(QObject *o, void *d, AppendFunction a, CountFunction c, AtFunction t,
                    ClearFunction r )
        : object(o), data(d), append(a), count(c), at(t), clear(r), dummy1(0), dummy2(0) {}
    QQmlListProperty(QObject *o, void *d, CountFunction c, AtFunction t)
        : object(o), data(d), append(0), count(c), at(t), clear(0), dummy1(0), dummy2(0) {}
    bool operator==(const QQmlListProperty &o) const {
        return object == o.object &&
               data == o.data &&
               append == o.append &&
               count == o.count &&
               at == o.at &&
               clear == o.clear;
    }

    QObject *object;
    void *data;

    AppendFunction append;

    CountFunction count;
    AtFunction at;

    ClearFunction clear;

    void *dummy1;
    void *dummy2;

private:
    static void qlist_append(QQmlListProperty *p, T *v) {
        reinterpret_cast<QList<T *> *>(p->data)->append(v);
    }
    static int qlist_count(QQmlListProperty *p) {
        return reinterpret_cast<QList<T *> *>(p->data)->count();
    }
    static T *qlist_at(QQmlListProperty *p, int idx) {
        return reinterpret_cast<QList<T *> *>(p->data)->at(idx);
    }
    static void qlist_clear(QQmlListProperty *p) {
        return reinterpret_cast<QList<T *> *>(p->data)->clear();
    }
};
#endif

class QQmlEngine;
class QQmlListReferencePrivate;
class Q_QML_EXPORT QQmlListReference
{
public:
    QQmlListReference();
    QQmlListReference(QObject *, const char *property, QQmlEngine * = 0);
    QQmlListReference(const QQmlListReference &);
    QQmlListReference &operator=(const QQmlListReference &);
    ~QQmlListReference();

    bool isValid() const;

    QObject *object() const;
    const QMetaObject *listElementType() const;

    bool canAppend() const;
    bool canAt() const;
    bool canClear() const;
    bool canCount() const;

    bool isManipulable() const;
    bool isReadable() const;

    bool append(QObject *) const;
    QObject *at(int) const;
    bool clear() const;
    int count() const;

private:
    friend class QQmlListReferencePrivate;
    QQmlListReferencePrivate* d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QQmlListReference)

QT_END_HEADER

#endif // QQMLLIST_H
