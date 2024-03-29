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

#include "qquickrepeater_p.h"
#include "qquickrepeater_p_p.h"
#include "qquickvisualdatamodel_p.h"

#include <private/qqmlglobal_p.h>
#include <private/qquicklistaccessor_p.h>
#include <private/qquickchangeset_p.h>

QT_BEGIN_NAMESPACE

QQuickRepeaterPrivate::QQuickRepeaterPrivate()
    : model(0), ownModel(false), inRequest(false), dataSourceIsObject(false), itemCount(0), createFrom(-1)
{
}

QQuickRepeaterPrivate::~QQuickRepeaterPrivate()
{
    if (ownModel)
        delete model;
}

/*!
    \qmltype Repeater
    \instantiates QQuickRepeater
    \inqmlmodule QtQuick 2
    \ingroup qtquick-models
    \ingroup qtquick-positioning
    \inherits Item
    \brief Instantiates a number of Item-based components using a provided model

    The Repeater type is used to create a large number of
    similar items. Like other view types, a Repeater has a \l model and a \l delegate:
    for each entry in the model, the delegate is instantiated
    in a context seeded with data from the model. A Repeater item is usually
    enclosed in a positioner type such as \l Row or \l Column to visually
    position the multiple delegate items created by the Repeater.

    The following Repeater creates three instances of a \l Rectangle item within
    a \l Row:

    \snippet qml/repeaters/repeater.qml import
    \codeline
    \snippet qml/repeaters/repeater.qml simple

    \image repeater-simple.png

    A Repeater's \l model can be any of the supported \l {qml-data-models}{data models}.
    Additionally, like delegates for other views, a Repeater delegate can access
    its index within the repeater, as well as the model data relevant to the
    delegate. See the \l delegate property documentation for details.

    Items instantiated by the Repeater are inserted, in order, as
    children of the Repeater's parent.  The insertion starts immediately after
    the repeater's position in its parent stacking list.  This allows
    a Repeater to be used inside a layout. For example, the following Repeater's
    items are stacked between a red rectangle and a blue rectangle:

    \snippet qml/repeaters/repeater.qml layout

    \image repeater.png


    \note A Repeater item owns all items it instantiates. Removing or dynamically destroying
    an item created by a Repeater results in unpredictable behavior.


    \section2 Considerations when using Repeater

    The Repeater type creates all of its delegate items when the repeater is first
    created. This can be inefficient if there are a large number of delegate items and
    not all of the items are required to be visible at the same time. If this is the case,
    consider using other view types like ListView (which only creates delegate items
    when they are scrolled into view) or use the \l {Dynamic Object Creation} methods to
    create items as they are required.

    Also, note that Repeater is \l {Item}-based, and can only repeat \l {Item}-derived objects.
    For example, it cannot be used to repeat QtObjects:
    \code
    //bad code
    Item {
        Can't repeat QtObject as it doesn't derive from Item.
        Repeater {
            model: 10
            QtObject {}
        }
    }
    \endcode
 */

/*!
    \qmlsignal QtQuick2::Repeater::onItemAdded(int index, Item item)

    This handler is called when an item is added to the repeater. The \a index
    parameter holds the index at which the item has been inserted within the
    repeater, and the \a item parameter holds the \l Item that has been added.
*/

/*!
    \qmlsignal QtQuick2::Repeater::onItemRemoved(int index, Item item)

    This handler is called when an item is removed from the repeater. The \a index
    parameter holds the index at which the item was removed from the repeater,
    and the \a item parameter holds the \l Item that was removed.

    Do not keep a reference to \a item if it was created by this repeater, as
    in these cases it will be deleted shortly after the handler is called.
*/
QQuickRepeater::QQuickRepeater(QQuickItem *parent)
  : QQuickItem(*(new QQuickRepeaterPrivate), parent)
{
}

QQuickRepeater::~QQuickRepeater()
{
}

/*!
    \qmlproperty any QtQuick2::Repeater::model

    The model providing data for the repeater.

    This property can be set to any of the supported \l {qml-data-models}{data models}:

    \list
    \li A number that indicates the number of delegates to be created by the repeater
    \li A model (e.g. a ListModel item, or a QAbstractItemModel subclass)
    \li A string list
    \li An object list
    \endlist

    The type of model affects the properties that are exposed to the \l delegate.

    \sa {qml-data-models}{Data Models}
*/
QVariant QQuickRepeater::model() const
{
    Q_D(const QQuickRepeater);

    if (d->dataSourceIsObject) {
        QObject *o = d->dataSourceAsObject;
        return QVariant::fromValue(o);
    }

    return d->dataSource;
}

void QQuickRepeater::setModel(const QVariant &model)
{
    Q_D(QQuickRepeater);
    if (d->dataSource == model)
        return;

    clear();
    if (d->model) {
        disconnect(d->model, SIGNAL(modelUpdated(QQuickChangeSet,bool)),
                this, SLOT(modelUpdated(QQuickChangeSet,bool)));
        disconnect(d->model, SIGNAL(createdItem(int,QQuickItem*)), this, SLOT(createdItem(int,QQuickItem*)));
        disconnect(d->model, SIGNAL(initItem(int,QQuickItem*)), this, SLOT(initItem(int,QQuickItem*)));
//        disconnect(d->model, SIGNAL(destroyingItem(QQuickItem*)), this, SLOT(destroyingItem(QQuickItem*)));
    }
    d->dataSource = model;
    QObject *object = qvariant_cast<QObject*>(model);
    d->dataSourceAsObject = object;
    d->dataSourceIsObject = object != 0;
    QQuickVisualModel *vim = 0;
    if (object && (vim = qobject_cast<QQuickVisualModel *>(object))) {
        if (d->ownModel) {
            delete d->model;
            d->ownModel = false;
        }
        d->model = vim;
    } else {
        if (!d->ownModel) {
            d->model = new QQuickVisualDataModel(qmlContext(this));
            d->ownModel = true;
            if (isComponentComplete())
                static_cast<QQuickVisualDataModel *>(d->model)->componentComplete();
        }
        if (QQuickVisualDataModel *dataModel = qobject_cast<QQuickVisualDataModel*>(d->model))
            dataModel->setModel(model);
    }
    if (d->model) {
        connect(d->model, SIGNAL(modelUpdated(QQuickChangeSet,bool)),
                this, SLOT(modelUpdated(QQuickChangeSet,bool)));
        connect(d->model, SIGNAL(createdItem(int,QQuickItem*)), this, SLOT(createdItem(int,QQuickItem*)));
        connect(d->model, SIGNAL(initItem(int,QQuickItem*)), this, SLOT(initItem(int,QQuickItem*)));
//        connect(d->model, SIGNAL(destroyingItem(QQuickItem*)), this, SLOT(destroyingItem(QQuickItem*)));
        regenerate();
    }
    emit modelChanged();
    emit countChanged();
}

/*!
    \qmlproperty Component QtQuick2::Repeater::delegate
    \default

    The delegate provides a template defining each item instantiated by the repeater.

    Delegates are exposed to a read-only \c index property that indicates the index
    of the delegate within the repeater. For example, the following \l Text delegate
    displays the index of each repeated item:

    \table
    \row
    \li \snippet qml/repeaters/repeater.qml index
    \li \image repeater-index.png
    \endtable

    If the \l model is a \l{QStringList-based model}{string list} or
    \l{QObjectList-based model}{object list}, the delegate is also exposed to
    a read-only \c modelData property that holds the string or object data. For
    example:

    \table
    \row
    \li \snippet qml/repeaters/repeater.qml modeldata
    \li \image repeater-modeldata.png
    \endtable

    If the \l model is a model object (such as a \l ListModel) the delegate
    can access all model roles as named properties, in the same way that delegates
    do for view classes like ListView.

    \sa {QML Data Models}
 */
QQmlComponent *QQuickRepeater::delegate() const
{
    Q_D(const QQuickRepeater);
    if (d->model) {
        if (QQuickVisualDataModel *dataModel = qobject_cast<QQuickVisualDataModel*>(d->model))
            return dataModel->delegate();
    }

    return 0;
}

void QQuickRepeater::setDelegate(QQmlComponent *delegate)
{
    Q_D(QQuickRepeater);
    if (QQuickVisualDataModel *dataModel = qobject_cast<QQuickVisualDataModel*>(d->model))
       if (delegate == dataModel->delegate())
           return;

    if (!d->ownModel) {
        d->model = new QQuickVisualDataModel(qmlContext(this));
        d->ownModel = true;
    }
    if (QQuickVisualDataModel *dataModel = qobject_cast<QQuickVisualDataModel*>(d->model)) {
        dataModel->setDelegate(delegate);
        regenerate();
        emit delegateChanged();
    }
}

/*!
    \qmlproperty int QtQuick2::Repeater::count

    This property holds the number of items in the repeater.
*/
int QQuickRepeater::count() const
{
    Q_D(const QQuickRepeater);
    if (d->model)
        return d->model->count();
    return 0;
}

/*!
    \qmlmethod Item QtQuick2::Repeater::itemAt(index)

    Returns the \l Item that has been created at the given \a index, or \c null
    if no item exists at \a index.
*/
QQuickItem *QQuickRepeater::itemAt(int index) const
{
    Q_D(const QQuickRepeater);
    if (index >= 0 && index < d->deletables.count())
        return d->deletables[index];
    return 0;
}

void QQuickRepeater::componentComplete()
{
    Q_D(QQuickRepeater);
    if (d->model && d->ownModel)
        static_cast<QQuickVisualDataModel *>(d->model)->componentComplete();
    QQuickItem::componentComplete();
    regenerate();
    if (d->model && d->model->count())
        emit countChanged();
}

void QQuickRepeater::itemChange(ItemChange change, const ItemChangeData &value)
{
    QQuickItem::itemChange(change, value);
    if (change == ItemParentHasChanged) {
        regenerate();
    }
}

void QQuickRepeater::clear()
{
    Q_D(QQuickRepeater);
    bool complete = isComponentComplete();

    if (d->model) {
        for (int i = 0; i < d->deletables.count(); ++i) {
            QQuickItem *item = d->deletables.at(i);
            if (complete)
                emit itemRemoved(i, item);
            d->model->release(item);
        }
    }
    d->deletables.clear();
    d->itemCount = 0;
}

void QQuickRepeater::regenerate()
{
    Q_D(QQuickRepeater);
    if (!isComponentComplete())
        return;

    clear();

    if (!d->model || !d->model->count() || !d->model->isValid() || !parentItem() || !isComponentComplete())
        return;

    d->itemCount = count();
    d->deletables.resize(d->itemCount);
    d->createFrom = 0;
    d->createItems();
}

void QQuickRepeaterPrivate::createItems()
{
    Q_Q(QQuickRepeater);
    if (createFrom == -1)
        return;
    inRequest = true;
    for (int ii = createFrom; ii < itemCount; ++ii) {
        if (!deletables.at(ii)) {
            QQuickItem *item = model->item(ii, false);
            if (!item) {
                createFrom = ii;
                break;
            }
            deletables[ii] = item;
            item->setParentItem(q->parentItem());
            if (ii > 0 && deletables.at(ii-1)) {
                item->stackAfter(deletables.at(ii-1));
            } else {
                QQuickItem *after = q;
                for (int si = ii+1; si < itemCount; ++si) {
                    if (deletables.at(si)) {
                        after = deletables.at(si);
                        break;
                    }
                }
                item->stackBefore(after);
            }
            emit q->itemAdded(ii, item);
        }
    }
    inRequest = false;
}

void QQuickRepeater::createdItem(int, QQuickItem *)
{
    Q_D(QQuickRepeater);
    if (!d->inRequest)
        d->createItems();
}

void QQuickRepeater::initItem(int, QQuickItem *item)
{
    item->setParentItem(parentItem());
}

void QQuickRepeater::modelUpdated(const QQuickChangeSet &changeSet, bool reset)
{
    Q_D(QQuickRepeater);

    if (!isComponentComplete())
        return;

    if (reset) {
        regenerate();
        if (changeSet.difference() != 0)
            emit countChanged();
        return;
    }

    int difference = 0;
    QHash<int, QVector<QPointer<QQuickItem> > > moved;
    foreach (const QQuickChangeSet::Remove &remove, changeSet.removes()) {
        int index = qMin(remove.index, d->deletables.count());
        int count = qMin(remove.index + remove.count, d->deletables.count()) - index;
        if (remove.isMove()) {
            moved.insert(remove.moveId, d->deletables.mid(index, count));
            d->deletables.erase(
                    d->deletables.begin() + index,
                    d->deletables.begin() + index + count);
        } else while (count--) {
            QQuickItem *item = d->deletables.at(index);
            d->deletables.remove(index);
            emit itemRemoved(index, item);
            if (item)
                d->model->release(item);
            --d->itemCount;
        }

        difference -= remove.count;
    }

    d->createFrom = -1;
    foreach (const QQuickChangeSet::Insert &insert, changeSet.inserts()) {
        int index = qMin(insert.index, d->deletables.count());
        if (insert.isMove()) {
            QVector<QPointer<QQuickItem> > items = moved.value(insert.moveId);
            d->deletables = d->deletables.mid(0, index) + items + d->deletables.mid(index);
            QQuickItem *stackBefore = index + items.count() < d->deletables.count()
                    ? d->deletables.at(index + items.count())
                    : this;
            for (int i = index; i < index + items.count(); ++i)
                d->deletables.at(i)->stackBefore(stackBefore);
        } else for (int i = 0; i < insert.count; ++i) {
            int modelIndex = index + i;
            ++d->itemCount;
            d->deletables.insert(modelIndex, 0);
            if (d->createFrom == -1)
                d->createFrom = modelIndex;
        }
        difference += insert.count;
    }

    d->createItems();

    if (difference != 0)
        emit countChanged();
}

QT_END_NAMESPACE
