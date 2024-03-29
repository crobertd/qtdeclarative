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

#ifndef QQUICKCHANGESET_P_H
#define QQUICKCHANGESET_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qdebug.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QQuickChangeSet
{
public:
    struct MoveKey
    {
        MoveKey() : moveId(-1), offset(0) {}
        MoveKey(int moveId, int offset) : moveId(moveId), offset(offset) {}
        int moveId;
        int offset;
    };

    struct Change
    {
        Change() : index(0), count(0), moveId(-1) {}
        Change(int index, int count, int moveId = -1, int offset = 0)
            : index(index), count(count), moveId(moveId), offset(offset) {}

        int index;
        int count;
        int moveId;
        int offset;

        bool isMove() const { return moveId >= 0; }

        MoveKey moveKey(int index) const {
            return MoveKey(moveId, index - Change::index + offset); }

        int start() const { return index; }
        int end() const { return index + count; }
    };


    struct Insert : public Change
    {
        Insert() {}
        Insert(int index, int count, int moveId = -1, int offset = 0)
            : Change(index, count, moveId, offset) {}
    };

    struct Remove : public Change
    {
        Remove() {}
        Remove(int index, int count, int moveId = -1, int offset = 0)
            : Change(index, count, moveId, offset) {}
    };

    QQuickChangeSet();
    QQuickChangeSet(const QQuickChangeSet &changeSet);
    ~QQuickChangeSet();

    QQuickChangeSet &operator =(const QQuickChangeSet &changeSet);

    const QVector<Remove> &removes() const { return m_removes; }
    const QVector<Insert> &inserts() const { return m_inserts; }
    const QVector<Change> &changes() const { return m_changes; }

    void insert(int index, int count);
    void remove(int index, int count);
    void move(int from, int to, int count, int moveId);
    void change(int index, int count);

    void insert(const QVector<Insert> &inserts);
    void remove(const QVector<Remove> &removes, QVector<Insert> *inserts = 0);
    void move(const QVector<Remove> &removes, const QVector<Insert> &inserts);
    void change(const QVector<Change> &changes);
    void apply(const QQuickChangeSet &changeSet);

    bool isEmpty() const { return m_removes.empty() && m_inserts.empty() && m_changes.isEmpty(); }

    void clear()
    {
        m_removes.clear();
        m_inserts.clear();
        m_changes.clear();
        m_difference = 0;
    }

    int difference() const { return m_difference; }

private:
    void remove(QVector<Remove> *removes, QVector<Insert> *inserts);
    void change(QVector<Change> *changes);

    QVector<Remove> m_removes;
    QVector<Insert> m_inserts;
    QVector<Change> m_changes;
    int m_difference;
};

Q_DECLARE_TYPEINFO(QQuickChangeSet::Change, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QQuickChangeSet::Remove, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QQuickChangeSet::Insert, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QQuickChangeSet::MoveKey, Q_PRIMITIVE_TYPE);

inline uint qHash(const QQuickChangeSet::MoveKey &key) { return qHash(qMakePair(key.moveId, key.offset)); }
inline bool operator ==(const QQuickChangeSet::MoveKey &l, const QQuickChangeSet::MoveKey &r) {
    return l.moveId == r.moveId && l.offset == r.offset; }

Q_AUTOTEST_EXPORT QDebug operator <<(QDebug debug, const QQuickChangeSet::Remove &remove);
Q_AUTOTEST_EXPORT QDebug operator <<(QDebug debug, const QQuickChangeSet::Insert &insert);
Q_AUTOTEST_EXPORT QDebug operator <<(QDebug debug, const QQuickChangeSet::Change &change);
Q_AUTOTEST_EXPORT QDebug operator <<(QDebug debug, const QQuickChangeSet &change);

QT_END_NAMESPACE

#endif
