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

#ifndef QQUICKITEMVIEW_P_P_H
#define QQUICKITEMVIEW_P_P_H

#include "qquickitemview_p.h"
#include "qquickitemviewtransition_p.h"
#include "qquickflickable_p_p.h"
#include "qquickvisualdatamodel_p.h"
#include "qquickvisualitemmodel_p.h"
#include <private/qquickchangeset_p.h>


QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class FxViewItem
{
public:
    FxViewItem(QQuickItem *, bool own, bool trackGeometry);
    virtual ~FxViewItem();

    qreal itemX() const;
    qreal itemY() const;

    void moveTo(const QPointF &pos, bool immediate);
    void setVisible(bool visible);

    QQuickItemViewTransitioner::TransitionType scheduledTransitionType() const;
    bool transitionScheduledOrRunning() const;
    bool transitionRunning() const;
    bool isPendingRemoval() const;

    void transitionNextReposition(QQuickItemViewTransitioner *transitioner, QQuickItemViewTransitioner::TransitionType type, bool asTarget);
    bool prepareTransition(QQuickItemViewTransitioner *transitioner, const QRectF &viewBounds);
    void startTransition(QQuickItemViewTransitioner *transitioner);

    // these are positions and sizes along the current direction of scrolling/flicking
    virtual qreal position() const = 0;
    virtual qreal endPosition() const = 0;
    virtual qreal size() const = 0;
    virtual qreal sectionSize() const = 0;

    virtual bool contains(qreal x, qreal y) const = 0;

    QQuickItem *item;
    QQuickItemViewTransitionableItem *transitionableItem;
    QQuickItemViewAttached *attached;
    int index;
    bool ownItem;
    bool releaseAfterTransition;
    bool trackGeom;
};


class QQuickItemViewChangeSet
{
public:
    QQuickItemViewChangeSet();

    bool hasPendingChanges() const;
    void prepare(int currentIndex, int count);
    void reset();

    void applyChanges(const QQuickChangeSet &changeSet);

    void applyBufferedChanges(const QQuickItemViewChangeSet &other);

    int itemCount;
    int newCurrentIndex;
    QQuickChangeSet pendingChanges;
    QHash<QQuickChangeSet::MoveKey, FxViewItem *> removedItems;

    bool active : 1;
    bool currentChanged : 1;
    bool currentRemoved : 1;
};


class QQuickItemViewPrivate : public QQuickFlickablePrivate, public QQuickItemViewTransitionChangeListener, public QAnimationJobChangeListener
{
    Q_DECLARE_PUBLIC(QQuickItemView)
public:
    QQuickItemViewPrivate();
    ~QQuickItemViewPrivate();

    static inline QQuickItemViewPrivate *get(QQuickItemView *o) { return o->d_func(); }

    struct ChangeResult {
        QQmlNullableValue<qreal> visiblePos;
        bool changedFirstItem;
        qreal sizeChangesBeforeVisiblePos;
        qreal sizeChangesAfterVisiblePos;
        int countChangeBeforeVisible;
        int countChangeAfterVisibleItems;

        ChangeResult()
            : visiblePos(0), changedFirstItem(false),
              sizeChangesBeforeVisiblePos(0), sizeChangesAfterVisiblePos(0),
              countChangeBeforeVisible(0), countChangeAfterVisibleItems(0) {}

        ChangeResult(const QQmlNullableValue<qreal> &p)
            : visiblePos(p), changedFirstItem(false),
              sizeChangesBeforeVisiblePos(0), sizeChangesAfterVisiblePos(0),
              countChangeBeforeVisible(0), countChangeAfterVisibleItems(0) {}

        ChangeResult &operator+=(const ChangeResult &other) {
            if (&other == this)
                return *this;
            changedFirstItem &= other.changedFirstItem;
            sizeChangesBeforeVisiblePos += other.sizeChangesBeforeVisiblePos;
            sizeChangesAfterVisiblePos += other.sizeChangesAfterVisiblePos;
            countChangeBeforeVisible += other.countChangeBeforeVisible;
            countChangeAfterVisibleItems += other.countChangeAfterVisibleItems;
            return *this;
        }

        void reset() {
            changedFirstItem = false;
            sizeChangesBeforeVisiblePos = 0.0;
            sizeChangesAfterVisiblePos = 0.0;
            countChangeBeforeVisible = 0;
            countChangeAfterVisibleItems = 0;
        }
    };

    enum BufferMode { NoBuffer = 0x00, BufferBefore = 0x01, BufferAfter = 0x02 };
    enum MovementReason { Other, SetIndex, Mouse };

    bool isValid() const;
    qreal position() const;
    qreal size() const;
    qreal startPosition() const;
    qreal endPosition() const;
    qreal contentStartOffset() const;
    int findLastVisibleIndex(int defaultValue = -1) const;
    FxViewItem *visibleItem(int modelIndex) const;
    FxViewItem *firstVisibleItem() const;
    int findLastIndexInView() const;
    int mapFromModel(int modelIndex) const;

    virtual void init();
    virtual void clear();
    virtual void updateViewport();

    void regenerate();
    void layout();
    virtual void animationFinished(QAbstractAnimationJob *);
    void refill();
    void refill(qreal from, qreal to);
    void mirrorChange();

    FxViewItem *createItem(int modelIndex, bool asynchronous = false);
    virtual bool releaseItem(FxViewItem *item);

    QQuickItem *createHighlightItem();
    QQuickItem *createComponentItem(QQmlComponent *component, qreal zValue, bool createDefault = false);

    void updateCurrent(int modelIndex);
    void updateTrackedItem();
    void updateUnrequestedIndexes();
    void updateUnrequestedPositions();
    void updateVisibleIndex();
    void positionViewAtIndex(int index, int mode);

    qreal minExtentForAxis(const AxisData &axisData, bool forXAxis) const;
    qreal maxExtentForAxis(const AxisData &axisData, bool forXAxis) const;

    void applyPendingChanges();
    bool applyModelChanges(ChangeResult *insertionResult, ChangeResult *removalResult);
    bool applyRemovalChange(const QQuickChangeSet::Remove &removal, ChangeResult *changeResult, int *removedCount);
    void removeItem(FxViewItem *item, const QQuickChangeSet::Remove &removal, ChangeResult *removeResult);
    void repositionFirstItem(FxViewItem *prevVisibleItemsFirst, qreal prevVisibleItemsFirstPos,
            FxViewItem *prevFirstVisible, ChangeResult *insertionResult, ChangeResult *removalResult);

    void createTransitioner();
    void prepareVisibleItemTransitions();
    void prepareRemoveTransitions(QHash<QQuickChangeSet::MoveKey, FxViewItem *> *removedItems);
    bool prepareNonVisibleItemTransition(FxViewItem *item, const QRectF &viewBounds);
    virtual void viewItemTransitionFinished(QQuickItemViewTransitionableItem *item);

    int findMoveKeyIndex(QQuickChangeSet::MoveKey key, const QVector<QQuickChangeSet::Remove> &changes) const;

    void checkVisible() const;
    void showVisibleItems() const;

    void markExtentsDirty() {
        if (layoutOrientation() == Qt::Vertical)
            vData.markExtentsDirty();
        else
            hData.markExtentsDirty();
    }

    bool hasPendingChanges() const {
        return currentChanges.hasPendingChanges()
                || bufferedChanges.hasPendingChanges()
                ||runDelayedRemoveTransition;
    }

    void refillOrLayout() {
        if (hasPendingChanges())
            layout();
        else
            refill();
    }

    void forceLayoutPolish() {
        Q_Q(QQuickItemView);
        forceLayout = true;
        q->polish();
    }

    QQmlGuard<QQuickVisualModel> model;
    QVariant modelVariant;
    int itemCount;
    int buffer;
    int bufferMode;
    Qt::LayoutDirection layoutDirection;
    QQuickItemView::VerticalLayoutDirection verticalLayoutDirection;

    MovementReason moveReason;

    QList<FxViewItem *> visibleItems;
    int visibleIndex;
    int currentIndex;
    FxViewItem *currentItem;
    FxViewItem *trackedItem;
    QHash<QQuickItem*,int> unrequestedItems;
    int requestedIndex;
    QQuickItemViewChangeSet currentChanges;
    QQuickItemViewChangeSet bufferedChanges;
    QPauseAnimationJob bufferPause;

    QQmlComponent *highlightComponent;
    FxViewItem *highlight;
    int highlightRange;     // enum value
    qreal highlightRangeStart;
    qreal highlightRangeEnd;
    int highlightMoveDuration;

    QQmlComponent *headerComponent;
    FxViewItem *header;
    QQmlComponent *footerComponent;
    FxViewItem *footer;

    struct MovedItem {
        FxViewItem *item;
        QQuickChangeSet::MoveKey moveKey;
        MovedItem(FxViewItem *i, QQuickChangeSet::MoveKey k)
            : item(i), moveKey(k) {}
    };
    QQuickItemViewTransitioner *transitioner;
    QList<FxViewItem *> releasePendingTransition;

    mutable qreal minExtent;
    mutable qreal maxExtent;

    bool ownModel : 1;
    bool wrap : 1;
    bool inLayout : 1;
    bool inViewportMoved : 1;
    bool forceLayout : 1;
    bool currentIndexCleared : 1;
    bool haveHighlightRange : 1;
    bool autoHighlight : 1;
    bool highlightRangeStartValid : 1;
    bool highlightRangeEndValid : 1;
    bool fillCacheBuffer : 1;
    bool inRequest : 1;
    bool runDelayedRemoveTransition : 1;

protected:
    virtual Qt::Orientation layoutOrientation() const = 0;
    virtual bool isContentFlowReversed() const = 0;

    virtual qreal positionAt(int index) const = 0;
    virtual qreal endPositionAt(int index) const = 0;
    virtual qreal originPosition() const = 0;
    virtual qreal lastPosition() const = 0;

    virtual qreal headerSize() const = 0;
    virtual qreal footerSize() const = 0;
    virtual bool showHeaderForIndex(int index) const = 0;
    virtual bool showFooterForIndex(int index) const = 0;
    virtual void updateHeader() = 0;
    virtual void updateFooter() = 0;

    virtual void createHighlight() = 0;
    virtual void updateHighlight() = 0;
    virtual void resetHighlightPosition() = 0;

    virtual void setPosition(qreal pos) = 0;
    virtual void fixupPosition() = 0;

    virtual bool addVisibleItems(qreal fillFrom, qreal fillTo, qreal bufferFrom, qreal bufferTo, bool doBuffer) = 0;
    virtual bool removeNonVisibleItems(qreal bufferFrom, qreal bufferTo) = 0;
    virtual void visibleItemsChanged() {}

    virtual FxViewItem *newViewItem(int index, QQuickItem *item) = 0;
    virtual void repositionItemAt(FxViewItem *item, int index, qreal sizeBuffer) = 0;
    virtual void repositionPackageItemAt(QQuickItem *item, int index) = 0;
    virtual void resetFirstItemPosition(qreal pos = 0.0) = 0;
    virtual void adjustFirstItem(qreal forwards, qreal backwards, int changeBeforeVisible) = 0;

    virtual void layoutVisibleItems(int fromModelIndex = 0) = 0;
    virtual void changedVisibleIndex(int newIndex) = 0;

    virtual bool applyInsertionChange(const QQuickChangeSet::Insert &insert, ChangeResult *changeResult,
                QList<FxViewItem *> *newItems, QList<MovedItem> *movingIntoView) = 0;

    virtual bool needsRefillForAddedOrRemovedIndex(int) const { return false; }
    virtual void translateAndTransitionItemsAfter(int afterIndex, const ChangeResult &insertionResult, const ChangeResult &removalResult) = 0;

    virtual void initializeViewItem(FxViewItem *) {}
    virtual void initializeCurrentItem() {}
    virtual void updateSectionCriteria() {}
    virtual void updateSections() {}

    virtual void itemGeometryChanged(QQuickItem *item, const QRectF &newGeometry, const QRectF &oldGeometry);
};


QT_END_NAMESPACE

QT_END_HEADER

#endif // QQUICKITEMVIEW_P_P_H
