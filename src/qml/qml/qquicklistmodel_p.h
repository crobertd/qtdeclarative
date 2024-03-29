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

#ifndef QQUICKLISTMODEL_H
#define QQUICKLISTMODEL_H

#include <qqml.h>
#include <private/qqmlcustomparser_p.h>

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/qabstractitemmodel.h>

#include <private/qv8engine_p.h>
#include <private/qpodvector_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QQuickListModelWorkerAgent;
class ListModel;
class ListLayout;

class Q_QML_PRIVATE_EXPORT QQuickListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool dynamicRoles READ dynamicRoles WRITE setDynamicRoles)

public:
    QQuickListModel(QObject *parent=0);
    ~QQuickListModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int,QByteArray> roleNames() const;

    QVariant data(int index, int role) const;
    int count() const;

    Q_INVOKABLE void clear();
    Q_INVOKABLE void remove(QQmlV8Function *args);
    Q_INVOKABLE void append(QQmlV8Function *args);
    Q_INVOKABLE void insert(QQmlV8Function *args);
    Q_INVOKABLE QQmlV8Handle get(int index) const;
    Q_INVOKABLE void set(int index, const QQmlV8Handle &);
    Q_INVOKABLE void setProperty(int index, const QString& property, const QVariant& value);
    Q_INVOKABLE void move(int from, int to, int count);
    Q_INVOKABLE void sync();

    QQuickListModelWorkerAgent *agent();

    bool dynamicRoles() const { return m_dynamicRoles; }
    void setDynamicRoles(bool enableDynamicRoles);

Q_SIGNALS:
    void countChanged();

private:
    friend class QQuickListModelParser;
    friend class QQuickListModelWorkerAgent;
    friend class ModelObject;
    friend class ModelNodeMetaObject;
    friend class ListModel;
    friend class ListElement;
    friend class DynamicRoleModelNode;
    friend class DynamicRoleModelNodeMetaObject;

    // Constructs a flat list model for a worker agent
    QQuickListModel(QQuickListModel *orig, QQuickListModelWorkerAgent *agent);
    QQuickListModel(const QQuickListModel *owner, ListModel *data, QV8Engine *eng, QObject *parent=0);

    QV8Engine *engine() const;

    inline bool canMove(int from, int to, int n) const { return !(from+n > count() || to+n > count() || from < 0 || to < 0 || n < 0); }

    QQuickListModelWorkerAgent *m_agent;
    mutable QV8Engine *m_engine;
    bool m_mainThread;
    bool m_primary;

    bool m_dynamicRoles;

    ListLayout *m_layout;
    ListModel *m_listModel;

    QVector<class DynamicRoleModelNode *> m_modelObjects;
    QVector<QString> m_roles;
    int m_uid;

    struct ElementSync
    {
        ElementSync() : src(0), target(0) {}

        DynamicRoleModelNode *src;
        DynamicRoleModelNode *target;
    };

    int getUid() const { return m_uid; }

    static void sync(QQuickListModel *src, QQuickListModel *target, QHash<int, QQuickListModel *> *targetModelHash);
    static QQuickListModel *createWithOwner(QQuickListModel *newOwner);

    void emitItemsChanged(int index, int count, const QVector<int> &roles);
    void emitItemsRemoved(int index, int count);
    void emitItemsInserted(int index, int count);
    void emitItemsMoved(int from, int to, int n);
};

// ### FIXME
class QQuickListElement : public QObject
{
Q_OBJECT
};

class QQuickListModelParser : public QQmlCustomParser
{
public:
    QQuickListModelParser() : QQmlCustomParser(QQmlCustomParser::AcceptsSignalHandlers) {}
    QByteArray compile(const QList<QQmlCustomParserProperty> &);
    void setCustomData(QObject *, const QByteArray &);

private:
    struct ListInstruction
    {
        enum { Push, Pop, Value, Set } type;
        int dataIdx;
    };
    struct ListModelData
    {
        int dataOffset;
        int instrCount;
        ListInstruction *instructions() const;
    };
    bool compileProperty(const QQmlCustomParserProperty &prop, QList<ListInstruction> &instr, QByteArray &data);

    bool definesEmptyList(const QString &);

    QString listElementTypeName;

    struct DataStackElement
    {
        DataStackElement() : model(0), elementIndex(0) {}

        QString name;
        ListModel *model;
        int elementIndex;
    };
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickListModel)
QML_DECLARE_TYPE(QQuickListElement)

QT_END_HEADER

#endif // QQUICKLISTMODEL_H
