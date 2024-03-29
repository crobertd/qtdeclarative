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

#ifndef QQUICKITEMVIEW_P_H
#define QQUICKITEMVIEW_P_H

#include "qquickflickable_p.h"

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickChangeSet;

class QQuickItemViewPrivate;

class Q_AUTOTEST_EXPORT QQuickItemView : public QQuickFlickable
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QQuickItem *currentItem READ currentItem NOTIFY currentItemChanged)

    Q_PROPERTY(bool keyNavigationWraps READ isWrapEnabled WRITE setWrapEnabled NOTIFY keyNavigationWrapsChanged)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer NOTIFY cacheBufferChanged)

    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged)
    Q_PROPERTY(Qt::LayoutDirection effectiveLayoutDirection READ effectiveLayoutDirection NOTIFY effectiveLayoutDirectionChanged)
    Q_PROPERTY(VerticalLayoutDirection verticalLayoutDirection READ verticalLayoutDirection WRITE setVerticalLayoutDirection NOTIFY verticalLayoutDirectionChanged)

    Q_PROPERTY(QQmlComponent *header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QQuickItem *headerItem READ headerItem NOTIFY headerItemChanged)
    Q_PROPERTY(QQmlComponent *footer READ footer WRITE setFooter NOTIFY footerChanged)
    Q_PROPERTY(QQuickItem *footerItem READ footerItem NOTIFY footerItemChanged)

    Q_PROPERTY(QQuickTransition *populate READ populateTransition WRITE setPopulateTransition NOTIFY populateTransitionChanged)
    Q_PROPERTY(QQuickTransition *add READ addTransition WRITE setAddTransition NOTIFY addTransitionChanged)
    Q_PROPERTY(QQuickTransition *addDisplaced READ addDisplacedTransition WRITE setAddDisplacedTransition NOTIFY addDisplacedTransitionChanged)
    Q_PROPERTY(QQuickTransition *move READ moveTransition WRITE setMoveTransition NOTIFY moveTransitionChanged)
    Q_PROPERTY(QQuickTransition *moveDisplaced READ moveDisplacedTransition WRITE setMoveDisplacedTransition NOTIFY moveDisplacedTransitionChanged)
    Q_PROPERTY(QQuickTransition *remove READ removeTransition WRITE setRemoveTransition NOTIFY removeTransitionChanged)
    Q_PROPERTY(QQuickTransition *removeDisplaced READ removeDisplacedTransition WRITE setRemoveDisplacedTransition NOTIFY removeDisplacedTransitionChanged)
    Q_PROPERTY(QQuickTransition *displaced READ displacedTransition WRITE setDisplacedTransition NOTIFY displacedTransitionChanged)

    Q_PROPERTY(QQmlComponent *highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)
    Q_PROPERTY(QQuickItem *highlightItem READ highlightItem NOTIFY highlightItemChanged)
    Q_PROPERTY(bool highlightFollowsCurrentItem READ highlightFollowsCurrentItem WRITE setHighlightFollowsCurrentItem NOTIFY highlightFollowsCurrentItemChanged)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode NOTIFY highlightRangeModeChanged)
    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin NOTIFY preferredHighlightBeginChanged RESET resetPreferredHighlightBegin)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd NOTIFY preferredHighlightEndChanged RESET resetPreferredHighlightEnd)
    Q_PROPERTY(int highlightMoveDuration READ highlightMoveDuration WRITE setHighlightMoveDuration NOTIFY highlightMoveDurationChanged)

    Q_ENUMS(HighlightRangeMode)
    Q_ENUMS(PositionMode)
    Q_ENUMS(VerticalLayoutDirection)
    Q_ENUMS(LayoutDirection)

public:
    // this holds all layout enum values so they can be referred to by other enums
    // to ensure consistent values - e.g. QML references to GridView.TopToBottom flow
    // and GridView.TopToBottom vertical layout direction should have same value
    enum LayoutDirection {
        LeftToRight = Qt::LeftToRight,
        RightToLeft = Qt::RightToLeft,
        VerticalTopToBottom,
        VerticalBottomToTop
    };

    enum VerticalLayoutDirection {
        TopToBottom = VerticalTopToBottom,
        BottomToTop = VerticalBottomToTop
    };

    QQuickItemView(QQuickFlickablePrivate &dd, QQuickItem *parent = 0);
    ~QQuickItemView();

    QVariant model() const;
    void setModel(const QVariant &);

    QQmlComponent *delegate() const;
    void setDelegate(QQmlComponent *);

    int count() const;

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QQuickItem *currentItem() const;

    bool isWrapEnabled() const;
    void setWrapEnabled(bool);

    int cacheBuffer() const;
    void setCacheBuffer(int);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection);
    Qt::LayoutDirection effectiveLayoutDirection() const;

    VerticalLayoutDirection verticalLayoutDirection() const;
    void setVerticalLayoutDirection(VerticalLayoutDirection layoutDirection);

    QQmlComponent *footer() const;
    void setFooter(QQmlComponent *);
    QQuickItem *footerItem() const;

    QQmlComponent *header() const;
    void setHeader(QQmlComponent *);
    QQuickItem *headerItem() const;

    QQuickTransition *populateTransition() const;
    void setPopulateTransition(QQuickTransition *transition);

    QQuickTransition *addTransition() const;
    void setAddTransition(QQuickTransition *transition);

    QQuickTransition *addDisplacedTransition() const;
    void setAddDisplacedTransition(QQuickTransition *transition);

    QQuickTransition *moveTransition() const;
    void setMoveTransition(QQuickTransition *transition);

    QQuickTransition *moveDisplacedTransition() const;
    void setMoveDisplacedTransition(QQuickTransition *transition);

    QQuickTransition *removeTransition() const;
    void setRemoveTransition(QQuickTransition *transition);

    QQuickTransition *removeDisplacedTransition() const;
    void setRemoveDisplacedTransition(QQuickTransition *transition);

    QQuickTransition *displacedTransition() const;
    void setDisplacedTransition(QQuickTransition *transition);

    QQmlComponent *highlight() const;
    void setHighlight(QQmlComponent *);

    QQuickItem *highlightItem() const;

    bool highlightFollowsCurrentItem() const;
    virtual void setHighlightFollowsCurrentItem(bool);

    enum HighlightRangeMode { NoHighlightRange, ApplyRange, StrictlyEnforceRange };
    HighlightRangeMode highlightRangeMode() const;
    void setHighlightRangeMode(HighlightRangeMode mode);

    qreal preferredHighlightBegin() const;
    void setPreferredHighlightBegin(qreal);
    void resetPreferredHighlightBegin();

    qreal preferredHighlightEnd() const;
    void setPreferredHighlightEnd(qreal);
    void resetPreferredHighlightEnd();

    int highlightMoveDuration() const;
    virtual void setHighlightMoveDuration(int);

    enum PositionMode { Beginning, Center, End, Visible, Contain, SnapPosition };

    Q_INVOKABLE void positionViewAtIndex(int index, int mode);
    Q_INVOKABLE int indexAt(qreal x, qreal y) const;
    Q_INVOKABLE QQuickItem *itemAt(qreal x, qreal y) const;
    Q_INVOKABLE void positionViewAtBeginning();
    Q_INVOKABLE void positionViewAtEnd();

    virtual void setContentX(qreal pos);
    virtual void setContentY(qreal pos);
    virtual qreal originX() const;
    virtual qreal originY() const;

signals:
    void modelChanged();
    void delegateChanged();
    void countChanged();
    void currentIndexChanged();
    void currentItemChanged();

    void keyNavigationWrapsChanged();
    void cacheBufferChanged();

    void layoutDirectionChanged();
    void effectiveLayoutDirectionChanged();
    void verticalLayoutDirectionChanged();

    void headerChanged();
    void footerChanged();
    void headerItemChanged();
    void footerItemChanged();

    void populateTransitionChanged();
    void addTransitionChanged();
    void addDisplacedTransitionChanged();
    void moveTransitionChanged();
    void moveDisplacedTransitionChanged();
    void removeTransitionChanged();
    void removeDisplacedTransitionChanged();
    void displacedTransitionChanged();

    void highlightChanged();
    void highlightItemChanged();
    void highlightFollowsCurrentItemChanged();
    void highlightRangeModeChanged();
    void preferredHighlightBeginChanged();
    void preferredHighlightEndChanged();
    void highlightMoveDurationChanged();

protected:
    virtual void updatePolish();
    virtual void componentComplete();
    virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    virtual qreal minYExtent() const;
    virtual qreal maxYExtent() const;
    virtual qreal minXExtent() const;
    virtual qreal maxXExtent() const;

protected slots:
    void destroyRemoved();
    void createdItem(int index, QQuickItem *item);
    virtual void initItem(int index, QQuickItem *item);
    void modelUpdated(const QQuickChangeSet &changeSet, bool reset);
    void destroyingItem(QQuickItem *item);
    void animStopped();
    void trackedPositionChanged();

private:
    Q_DECLARE_PRIVATE(QQuickItemView)
};


class Q_AUTOTEST_EXPORT QQuickItemViewAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY currentItemChanged)
    Q_PROPERTY(bool delayRemove READ delayRemove WRITE setDelayRemove NOTIFY delayRemoveChanged)

    Q_PROPERTY(QString section READ section NOTIFY sectionChanged)
    Q_PROPERTY(QString previousSection READ prevSection NOTIFY prevSectionChanged)
    Q_PROPERTY(QString nextSection READ nextSection NOTIFY nextSectionChanged)

public:
    QQuickItemViewAttached(QObject *parent)
        : QObject(parent), m_isCurrent(false), m_delayRemove(false) {}
    ~QQuickItemViewAttached() {}

    bool isCurrentItem() const { return m_isCurrent; }
    void setIsCurrentItem(bool c) {
        if (m_isCurrent != c) {
            m_isCurrent = c;
            emit currentItemChanged();
        }
    }

    bool delayRemove() const { return m_delayRemove; }
    void setDelayRemove(bool delay) {
        if (m_delayRemove != delay) {
            m_delayRemove = delay;
            emit delayRemoveChanged();
        }
    }

    QString section() const { return m_section; }
    void setSection(const QString &sect) {
        if (m_section != sect) {
            m_section = sect;
            emit sectionChanged();
        }
    }

    QString prevSection() const { return m_prevSection; }
    void setPrevSection(const QString &sect) {
        if (m_prevSection != sect) {
            m_prevSection = sect;
            emit prevSectionChanged();
        }
    }

    QString nextSection() const { return m_nextSection; }
    void setNextSection(const QString &sect) {
        if (m_nextSection != sect) {
            m_nextSection = sect;
            emit nextSectionChanged();
        }
    }

    void emitAdd() { emit add(); }
    void emitRemove() { emit remove(); }

signals:
    void currentItemChanged();
    void delayRemoveChanged();

    void add();
    void remove();

    void sectionChanged();
    void prevSectionChanged();
    void nextSectionChanged();

public:
    bool m_isCurrent : 1;
    bool m_delayRemove : 1;

    // current only used by list view
    mutable QString m_section;
    QString m_prevSection;
    QString m_nextSection;
};


QT_END_NAMESPACE

QT_END_HEADER

#endif // QQUICKITEMVIEW_P_H

