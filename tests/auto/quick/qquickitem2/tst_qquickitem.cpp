/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <qtest.h>
#include <QtTest/QSignalSpy>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlcontext.h>
#include <QtQuick/qquickview.h>
#include <QtGui/private/qinputmethod_p.h>
#include <QtQuick/private/qquickrectangle_p.h>
#include <QtQuick/private/qquicktextinput_p.h>
#include <private/qquickitem_p.h>
#include "../../shared/util.h"
#include "../shared/visualtestutil.h"
#include "../../shared/platforminputcontext.h"

using namespace QQuickVisualTestUtil;

class tst_QQuickItem : public QQmlDataTest
{
    Q_OBJECT
public:
    tst_QQuickItem();

private slots:
    void initTestCase();
    void cleanup();

    void keys();
    void keysProcessingOrder();
    void keysim();
    void keyNavigation();
    void keyNavigation_RightToLeft();
    void keyNavigation_skipNotVisible();
    void keyNavigation_implicitSetting();
    void layoutMirroring();
    void layoutMirroringIllegalParent();
    void smooth();
    void antialiasing();
    void clip();
    void mapCoordinates();
    void mapCoordinates_data();
    void mapCoordinatesRect();
    void mapCoordinatesRect_data();
    void propertyChanges();
    void transforms();
    void transforms_data();
    void childrenRect();
    void childrenRectBug();
    void childrenRectBug2();
    void childrenRectBug3();

    void childrenProperty();
    void resourcesProperty();

    void transformCrash();
    void implicitSize();
    void qtbug_16871();
    void visibleChildren();
    void parentLoop();
    void contains_data();
    void contains();
    void childAt();

private:
    QQmlEngine engine;
};

class KeysTestObject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool processLast READ processLast NOTIFY processLastChanged)

public:
    KeysTestObject() : mKey(0), mModifiers(0), mForwardedKey(0), mLast(false), mNativeScanCode(0) {}

    void reset() {
        mKey = 0;
        mText = QString();
        mModifiers = 0;
        mForwardedKey = 0;
        mNativeScanCode = 0;
    }

    bool processLast() const { return mLast; }
    void setProcessLast(bool b) {
        if (b != mLast) {
            mLast = b;
            emit processLastChanged();
        }
    }

public slots:
    void keyPress(int key, QString text, int modifiers) {
        mKey = key;
        mText = text;
        mModifiers = modifiers;
    }
    void keyRelease(int key, QString text, int modifiers) {
        mKey = key;
        mText = text;
        mModifiers = modifiers;
    }
    void forwardedKey(int key) {
        mForwardedKey = key;
    }
    void specialKey(int key, QString text, quint32 nativeScanCode) {
        mKey = key;
        mText = text;
        mNativeScanCode = nativeScanCode;
    }

signals:
    void processLastChanged();

public:
    int mKey;
    QString mText;
    int mModifiers;
    int mForwardedKey;
    bool mLast;
    quint32 mNativeScanCode;

private:
};

class KeyTestItem : public QQuickItem
{
    Q_OBJECT
public:
    KeyTestItem(QQuickItem *parent=0) : QQuickItem(parent), mKey(0) {}

protected:
    void keyPressEvent(QKeyEvent *e) {
        mKey = e->key();

        if (e->key() == Qt::Key_A)
            e->accept();
        else
            e->ignore();
    }

    void keyReleaseEvent(QKeyEvent *e) {
        if (e->key() == Qt::Key_B)
            e->accept();
        else
            e->ignore();
    }

public:
    int mKey;
};

QML_DECLARE_TYPE(KeyTestItem);

class HollowTestItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool circle READ isCircle WRITE setCircle)
    Q_PROPERTY(qreal holeRadius READ holeRadius WRITE setHoleRadius)

public:
    HollowTestItem(QQuickItem *parent = 0)
        : QQuickItem(parent),
          m_isPressed(false),
          m_isHovered(false),
          m_isCircle(false),
          m_holeRadius(50)
    {
        setAcceptHoverEvents(true);
        setAcceptedMouseButtons(Qt::LeftButton);
    }

    bool isPressed() const { return m_isPressed; }
    bool isHovered() const { return m_isHovered; }

    bool isCircle() const { return m_isCircle; }
    void setCircle(bool circle) { m_isCircle = circle; }

    qreal holeRadius() const { return m_holeRadius; }
    void setHoleRadius(qreal radius) { m_holeRadius = radius; }

    bool contains(const QPointF &point) const {
        const qreal w = width();
        const qreal h = height();
        const qreal r = m_holeRadius;

        // check boundaries
        if (!QRectF(0, 0, w, h).contains(point))
            return false;

        // square shape
        if (!m_isCircle)
            return !QRectF(w / 2 - r, h / 2 - r, r * 2, r * 2).contains(point);

        // circle shape
        const qreal dx = point.x() - (w / 2);
        const qreal dy = point.y() - (h / 2);
        const qreal dd = (dx * dx) + (dy * dy);
        const qreal outerRadius = qMin<qreal>(w / 2, h / 2);
        return dd > (r * r) && dd <= outerRadius * outerRadius;
    }

protected:
    void hoverEnterEvent(QHoverEvent *) { m_isHovered = true; }
    void hoverLeaveEvent(QHoverEvent *) { m_isHovered = false; }
    void mousePressEvent(QMouseEvent *) { m_isPressed = true; }
    void mouseReleaseEvent(QMouseEvent *) { m_isPressed = false; }

private:
    bool m_isPressed;
    bool m_isHovered;
    bool m_isCircle;
    qreal m_holeRadius;
};

QML_DECLARE_TYPE(HollowTestItem);


tst_QQuickItem::tst_QQuickItem()
{
}

void tst_QQuickItem::initTestCase()
{
    QQmlDataTest::initTestCase();
    qmlRegisterType<KeyTestItem>("Test",1,0,"KeyTestItem");
    qmlRegisterType<HollowTestItem>("Test", 1, 0, "HollowTestItem");
}

void tst_QQuickItem::cleanup()
{
    QInputMethodPrivate *inputMethodPrivate = QInputMethodPrivate::get(qApp->inputMethod());
    inputMethodPrivate->testContext = 0;
}

void tst_QQuickItem::keys()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    KeysTestObject *testObject = new KeysTestObject;
    window->rootContext()->setContextProperty("keysTestObject", testObject);

    window->rootContext()->setContextProperty("enableKeyHanding", QVariant(true));
    window->rootContext()->setContextProperty("forwardeeVisible", QVariant(true));

    window->setSource(testFileUrl("keystest.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QVERIFY(window->rootObject());
    QCOMPARE(window->rootObject()->property("isEnabled").toBool(), true);

    QKeyEvent key(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_A));
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyRelease, Qt::Key_A, Qt::ShiftModifier, "A", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_A));
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::ShiftModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Return));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_Return));
    QCOMPARE(testObject->mText, QLatin1String("Return"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_0, Qt::NoModifier, "0", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_0));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_0));
    QCOMPARE(testObject->mText, QLatin1String("0"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_9, Qt::NoModifier, "9", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_9));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_9));
    QCOMPARE(testObject->mText, QLatin1String("9"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Tab));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_Tab));
    QCOMPARE(testObject->mText, QLatin1String("Tab"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Backtab));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_Backtab));
    QCOMPARE(testObject->mText, QLatin1String("Backtab"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_VolumeUp, Qt::NoModifier, 1234, 0, 0);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_VolumeUp));
    QCOMPARE(testObject->mForwardedKey, int(Qt::Key_VolumeUp));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(testObject->mNativeScanCode == 1234);
    QVERIFY(key.isAccepted());

    testObject->reset();

    window->rootContext()->setContextProperty("forwardeeVisible", QVariant(false));
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mForwardedKey, 0);
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    window->rootContext()->setContextProperty("enableKeyHanding", QVariant(false));
    QCOMPARE(window->rootObject()->property("isEnabled").toBool(), false);

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, 0);
    QVERIFY(!key.isAccepted());

    window->rootContext()->setContextProperty("enableKeyHanding", QVariant(true));
    QCOMPARE(window->rootObject()->property("isEnabled").toBool(), true);

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_Return));
    QVERIFY(key.isAccepted());

    delete window;
    delete testObject;
}

void tst_QQuickItem::keysProcessingOrder()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    KeysTestObject *testObject = new KeysTestObject;
    window->rootContext()->setContextProperty("keysTestObject", testObject);

    window->setSource(testFileUrl("keyspriority.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    KeyTestItem *testItem = qobject_cast<KeyTestItem*>(window->rootObject());
    QVERIFY(testItem);

    QCOMPARE(testItem->property("priorityTest").toInt(), 0);

    QKeyEvent key(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_A));
    QCOMPARE(testObject->mText, QLatin1String("A"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(key.isAccepted());

    testObject->reset();

    testObject->setProcessLast(true);

    QCOMPARE(testItem->property("priorityTest").toInt(), 1);

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier, "A", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, 0);
    QVERIFY(key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyPress, Qt::Key_B, Qt::NoModifier, "B", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, int(Qt::Key_B));
    QCOMPARE(testObject->mText, QLatin1String("B"));
    QVERIFY(testObject->mModifiers == Qt::NoModifier);
    QVERIFY(!key.isAccepted());

    testObject->reset();

    key = QKeyEvent(QEvent::KeyRelease, Qt::Key_B, Qt::NoModifier, "B", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QCOMPARE(testObject->mKey, 0);
    QVERIFY(key.isAccepted());

    delete window;
    delete testObject;
}

void tst_QQuickItem::keysim()
{
    PlatformInputContext platformInputContext;
    QInputMethodPrivate *inputMethodPrivate = QInputMethodPrivate::get(qApp->inputMethod());
    inputMethodPrivate->testContext = &platformInputContext;

    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    window->setSource(testFileUrl("keysim.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QVERIFY(window->rootObject());
    QVERIFY(window->rootObject()->hasFocus() && window->rootObject()->hasActiveFocus());

    QQuickTextInput *input = window->rootObject()->findChild<QQuickTextInput*>();
    QVERIFY(input);

    QInputMethodEvent ev("Hello world!", QList<QInputMethodEvent::Attribute>());
    QGuiApplication::sendEvent(qGuiApp->focusObject(), &ev);

    QEXPECT_FAIL("", "QTBUG-24280", Continue);
    QCOMPARE(input->text(), QLatin1String("Hello world!"));

    delete window;
}

QQuickItemPrivate *childPrivate(QQuickItem *rootItem, const char * itemString)
{
    QQuickItem *item = findItem<QQuickItem>(rootItem, QString(QLatin1String(itemString)));
    QQuickItemPrivate* itemPrivate = QQuickItemPrivate::get(item);
    return itemPrivate;
}

QVariant childProperty(QQuickItem *rootItem, const char * itemString, const char * property)
{
    QQuickItem *item = findItem<QQuickItem>(rootItem, QString(QLatin1String(itemString)));
    return item->property(property);
}

bool anchorsMirrored(QQuickItem *rootItem, const char * itemString)
{
    QQuickItem *item = findItem<QQuickItem>(rootItem, QString(QLatin1String(itemString)));
    QQuickItemPrivate* itemPrivate = QQuickItemPrivate::get(item);
    return itemPrivate->anchors()->mirrored();
}

void tst_QQuickItem::layoutMirroring()
{
    QQuickView *window = new QQuickView(0);
    window->setSource(testFileUrl("layoutmirroring.qml"));
    window->show();

    QQuickItem *rootItem = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(rootItem);
    QQuickItemPrivate *rootPrivate = QQuickItemPrivate::get(rootItem);
    QVERIFY(rootPrivate);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->effectiveLayoutMirror, true);

    QCOMPARE(anchorsMirrored(rootItem, "mirrored1"), true);
    QCOMPARE(anchorsMirrored(rootItem, "mirrored2"), true);
    QCOMPARE(anchorsMirrored(rootItem, "notMirrored1"), false);
    QCOMPARE(anchorsMirrored(rootItem, "notMirrored2"), false);
    QCOMPARE(anchorsMirrored(rootItem, "inheritedMirror1"), true);
    QCOMPARE(anchorsMirrored(rootItem, "inheritedMirror2"), true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritedLayoutMirror, true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->isMirrorImplicit, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->isMirrorImplicit, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->isMirrorImplicit, true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->inheritMirrorFromParent, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->inheritMirrorFromParent, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritMirrorFromParent, true);

    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritMirrorFromItem, true);
    QCOMPARE(childPrivate(rootItem, "mirrored2")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored2")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritMirrorFromItem, false);

    // load dynamic content using Loader that needs to inherit mirroring
    rootItem->setProperty("state", "newContent");
    QCOMPARE(childPrivate(rootItem, "notMirrored3")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->effectiveLayoutMirror, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->inheritedLayoutMirror, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->isMirrorImplicit, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->isMirrorImplicit, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritMirrorFromParent, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror3")->inheritMirrorFromParent, true);

    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritMirrorFromItem, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored3")->inheritMirrorFromItem, false);

    // disable inheritance
    rootItem->setProperty("childrenInherit", false);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->effectiveLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->effectiveLayoutMirror, false);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritedLayoutMirror, false);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritedLayoutMirror, false);

    // re-enable inheritance
    rootItem->setProperty("childrenInherit", true);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->effectiveLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->effectiveLayoutMirror, false);

    QCOMPARE(childPrivate(rootItem, "inheritedMirror1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "inheritedMirror2")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "mirrored1")->inheritedLayoutMirror, true);
    QCOMPARE(childPrivate(rootItem, "notMirrored1")->inheritedLayoutMirror, true);

    //
    // dynamic parenting
    //
    QQuickItem *parentItem1 = new QQuickItem();
    QQuickItemPrivate::get(parentItem1)->effectiveLayoutMirror = true; // LayoutMirroring.enabled: true
    QQuickItemPrivate::get(parentItem1)->isMirrorImplicit = false;
    QQuickItemPrivate::get(parentItem1)->inheritMirrorFromItem = true; // LayoutMirroring.childrenInherit: true
    QQuickItemPrivate::get(parentItem1)->resolveLayoutMirror();

    // inherit in constructor
    QQuickItem *childItem1 = new QQuickItem(parentItem1);
    QCOMPARE(QQuickItemPrivate::get(childItem1)->effectiveLayoutMirror, true);
    QCOMPARE(QQuickItemPrivate::get(childItem1)->inheritMirrorFromParent, true);

    // inherit through a parent change
    QQuickItem *childItem2 = new QQuickItem();
    QCOMPARE(QQuickItemPrivate::get(childItem2)->effectiveLayoutMirror, false);
    QCOMPARE(QQuickItemPrivate::get(childItem2)->inheritMirrorFromParent, false);
    childItem2->setParentItem(parentItem1);
    QCOMPARE(QQuickItemPrivate::get(childItem2)->effectiveLayoutMirror, true);
    QCOMPARE(QQuickItemPrivate::get(childItem2)->inheritMirrorFromParent, true);

    // stop inherting through a parent change
    QQuickItem *parentItem2 = new QQuickItem();
    QQuickItemPrivate::get(parentItem2)->effectiveLayoutMirror = true; // LayoutMirroring.enabled: true
    QQuickItemPrivate::get(parentItem2)->resolveLayoutMirror();
    childItem2->setParentItem(parentItem2);
    QCOMPARE(QQuickItemPrivate::get(childItem2)->effectiveLayoutMirror, false);
    QCOMPARE(QQuickItemPrivate::get(childItem2)->inheritMirrorFromParent, false);

    delete parentItem1;
    delete parentItem2;
}

void tst_QQuickItem::layoutMirroringIllegalParent()
{
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0; QtObject { LayoutMirroring.enabled: true; LayoutMirroring.childrenInherit: true }", QUrl::fromLocalFile(""));
    QTest::ignoreMessage(QtWarningMsg, "file::1:21: QML QtObject: LayoutDirection attached property only works with Items");
    QObject *object = component.create();
    QVERIFY(object != 0);
}

void tst_QQuickItem::keyNavigation()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    window->setSource(testFileUrl("keynavigationtest.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QQuickItem *item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    QVariant result;
    QVERIFY(QMetaObject::invokeMethod(window->rootObject(), "verify",
            Q_RETURN_ARG(QVariant, result)));
    QVERIFY(result.toBool());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // down
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // left
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // up
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete window;
}

void tst_QQuickItem::keyNavigation_RightToLeft()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    window->setSource(testFileUrl("keynavigationtest.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QQuickItem *rootItem = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(rootItem);
    QQuickItemPrivate* rootItemPrivate = QQuickItemPrivate::get(rootItem);

    rootItemPrivate->effectiveLayoutMirror = true; // LayoutMirroring.mirror: true
    rootItemPrivate->isMirrorImplicit = false;
    rootItemPrivate->inheritMirrorFromItem = true; // LayoutMirroring.inherit: true
    rootItemPrivate->resolveLayoutMirror();

    QEvent wa(QEvent::WindowActivate);
    QGuiApplication::sendEvent(window, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QGuiApplication::sendEvent(window, &fe);

    QQuickItem *item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    QVariant result;
    QVERIFY(QMetaObject::invokeMethod(window->rootObject(), "verify",
            Q_RETURN_ARG(QVariant, result)));
    QVERIFY(result.toBool());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // left
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete window;
}

void tst_QQuickItem::keyNavigation_skipNotVisible()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    window->setSource(testFileUrl("keynavigationtest.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QQuickItem *item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // Set item 2 to not visible
    item = findItem<QQuickItem>(window->rootObject(), "item2");
    QVERIFY(item);
    item->setVisible(false);
    QVERIFY(!item->isVisible());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    //Set item 3 to not visible
    item = findItem<QQuickItem>(window->rootObject(), "item3");
    QVERIFY(item);
    item->setVisible(false);
    QVERIFY(!item->isVisible());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete window;
}

void tst_QQuickItem::keyNavigation_implicitSetting()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(240,320));

    window->setSource(testFileUrl("keynavigationtest_implicit.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QEvent wa(QEvent::WindowActivate);
    QGuiApplication::sendEvent(window, &wa);
    QFocusEvent fe(QEvent::FocusIn);
    QGuiApplication::sendEvent(window, &fe);

    QQuickItem *item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    QVariant result;
    QVERIFY(QMetaObject::invokeMethod(window->rootObject(), "verify",
            Q_RETURN_ARG(QVariant, result)));
    QVERIFY(result.toBool());

    // right
    QKeyEvent key(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item1
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // down
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // move to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // left
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // up
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item2");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // tab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item1");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // back to item4
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item4");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    // backtab
    key = QKeyEvent(QEvent::KeyPress, Qt::Key_Backtab, Qt::NoModifier, "", false, 1);
    QGuiApplication::sendEvent(window, &key);
    QVERIFY(key.isAccepted());

    item = findItem<QQuickItem>(window->rootObject(), "item3");
    QVERIFY(item);
    QVERIFY(item->hasActiveFocus());

    delete window;
}

void tst_QQuickItem::smooth()
{
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0; Item { smooth: false; }", QUrl::fromLocalFile(""));
    QQuickItem *item = qobject_cast<QQuickItem*>(component.create());
    QSignalSpy spy(item, SIGNAL(smoothChanged(bool)));

    QVERIFY(item);
    QVERIFY(!item->smooth());

    item->setSmooth(true);
    QVERIFY(item->smooth());
    QCOMPARE(spy.count(),1);
    QList<QVariant> arguments = spy.first();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0).toBool() == true);

    item->setSmooth(true);
    QCOMPARE(spy.count(),1);

    item->setSmooth(false);
    QVERIFY(!item->smooth());
    QCOMPARE(spy.count(),2);
    item->setSmooth(false);
    QCOMPARE(spy.count(),2);

    delete item;
}

void tst_QQuickItem::antialiasing()
{
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0; Item { antialiasing: false; }", QUrl::fromLocalFile(""));
    QQuickItem *item = qobject_cast<QQuickItem*>(component.create());
    QSignalSpy spy(item, SIGNAL(antialiasingChanged(bool)));

    QVERIFY(item);
    QVERIFY(!item->antialiasing());

    item->setAntialiasing(true);
    QVERIFY(item->antialiasing());
    QCOMPARE(spy.count(),1);
    QList<QVariant> arguments = spy.first();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0).toBool() == true);

    item->setAntialiasing(true);
    QCOMPARE(spy.count(),1);

    item->setAntialiasing(false);
    QVERIFY(!item->antialiasing());
    QCOMPARE(spy.count(),2);
    item->setAntialiasing(false);
    QCOMPARE(spy.count(),2);

    delete item;
}

void tst_QQuickItem::clip()
{
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0\nItem { clip: false\n }", QUrl::fromLocalFile(""));
    QQuickItem *item = qobject_cast<QQuickItem*>(component.create());
    QSignalSpy spy(item, SIGNAL(clipChanged(bool)));

    QVERIFY(item);
    QVERIFY(!item->clip());

    item->setClip(true);
    QVERIFY(item->clip());

    QList<QVariant> arguments = spy.first();
    QVERIFY(arguments.count() == 1);
    QVERIFY(arguments.at(0).toBool() == true);

    QCOMPARE(spy.count(),1);
    item->setClip(true);
    QCOMPARE(spy.count(),1);

    item->setClip(false);
    QVERIFY(!item->clip());
    QCOMPARE(spy.count(),2);
    item->setClip(false);
    QCOMPARE(spy.count(),2);

    delete item;
}

void tst_QQuickItem::mapCoordinates()
{
    QFETCH(int, x);
    QFETCH(int, y);

    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(300, 300));
    window->setSource(testFileUrl("mapCoordinates.qml"));
    window->show();
    qApp->processEvents();

    QQuickItem *root = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(root != 0);
    QQuickItem *a = findItem<QQuickItem>(window->rootObject(), "itemA");
    QVERIFY(a != 0);
    QQuickItem *b = findItem<QQuickItem>(window->rootObject(), "itemB");
    QVERIFY(b != 0);

    QVariant result;

    QVERIFY(QMetaObject::invokeMethod(root, "mapAToB",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QQuickItem*>(a)->mapToItem(b, QPointF(x, y)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAFromB",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QQuickItem*>(a)->mapFromItem(b, QPointF(x, y)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAToNull",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QQuickItem*>(a)->mapToScene(QPointF(x, y)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAFromNull",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QCOMPARE(result.value<QPointF>(), qobject_cast<QQuickItem*>(a)->mapFromScene(QPointF(x, y)));

    QString warning1 = testFileUrl("mapCoordinates.qml").toString() + ":48:5: QML Item: mapToItem() given argument \"1122\" which is neither null nor an Item";
    QString warning2 = testFileUrl("mapCoordinates.qml").toString() + ":48:5: QML Item: mapFromItem() given argument \"1122\" which is neither null nor an Item";

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QVERIFY(QMetaObject::invokeMethod(root, "checkMapAToInvalid",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QVERIFY(result.toBool());

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QVERIFY(QMetaObject::invokeMethod(root, "checkMapAFromInvalid",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y)));
    QVERIFY(result.toBool());

    delete window;
}

void tst_QQuickItem::mapCoordinates_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");

    for (int i=-20; i<=20; i+=10)
        QTest::newRow(QTest::toString(i)) << i << i;
}

void tst_QQuickItem::mapCoordinatesRect()
{
    QFETCH(int, x);
    QFETCH(int, y);
    QFETCH(int, width);
    QFETCH(int, height);

    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(300, 300));
    window->setSource(testFileUrl("mapCoordinatesRect.qml"));
    window->show();
    qApp->processEvents();

    QQuickItem *root = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(root != 0);
    QQuickItem *a = findItem<QQuickItem>(window->rootObject(), "itemA");
    QVERIFY(a != 0);
    QQuickItem *b = findItem<QQuickItem>(window->rootObject(), "itemB");
    QVERIFY(b != 0);

    QVariant result;

    QVERIFY(QMetaObject::invokeMethod(root, "mapAToB",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y), Q_ARG(QVariant, width), Q_ARG(QVariant, height)));
    QCOMPARE(result.value<QRectF>(), qobject_cast<QQuickItem*>(a)->mapRectToItem(b, QRectF(x, y, width, height)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAFromB",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y), Q_ARG(QVariant, width), Q_ARG(QVariant, height)));
    QCOMPARE(result.value<QRectF>(), qobject_cast<QQuickItem*>(a)->mapRectFromItem(b, QRectF(x, y, width, height)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAToNull",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y), Q_ARG(QVariant, width), Q_ARG(QVariant, height)));
    QCOMPARE(result.value<QRectF>(), qobject_cast<QQuickItem*>(a)->mapRectToScene(QRectF(x, y, width, height)));

    QVERIFY(QMetaObject::invokeMethod(root, "mapAFromNull",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y), Q_ARG(QVariant, width), Q_ARG(QVariant, height)));
    QCOMPARE(result.value<QRectF>(), qobject_cast<QQuickItem*>(a)->mapRectFromScene(QRectF(x, y, width, height)));

    QString warning1 = testFileUrl("mapCoordinatesRect.qml").toString() + ":48:5: QML Item: mapToItem() given argument \"1122\" which is neither null nor an Item";
    QString warning2 = testFileUrl("mapCoordinatesRect.qml").toString() + ":48:5: QML Item: mapFromItem() given argument \"1122\" which is neither null nor an Item";

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QVERIFY(QMetaObject::invokeMethod(root, "checkMapAToInvalid",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y), Q_ARG(QVariant, width), Q_ARG(QVariant, height)));
    QVERIFY(result.toBool());

    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QVERIFY(QMetaObject::invokeMethod(root, "checkMapAFromInvalid",
            Q_RETURN_ARG(QVariant, result), Q_ARG(QVariant, x), Q_ARG(QVariant, y), Q_ARG(QVariant, width), Q_ARG(QVariant, height)));
    QVERIFY(result.toBool());

    delete window;
}

void tst_QQuickItem::mapCoordinatesRect_data()
{
    QTest::addColumn<int>("x");
    QTest::addColumn<int>("y");
    QTest::addColumn<int>("width");
    QTest::addColumn<int>("height");

    for (int i=-20; i<=20; i+=5)
        QTest::newRow(QTest::toString(i)) << i << i << i << i;
}

void tst_QQuickItem::transforms_data()
{
    QTest::addColumn<QByteArray>("qml");
    QTest::addColumn<QTransform>("transform");
    QTest::newRow("translate") << QByteArray("Translate { x: 10; y: 20 }")
        << QTransform(1,0,0,0,1,0,10,20,1);
    QTest::newRow("rotation") << QByteArray("Rotation { angle: 90 }")
        << QTransform(0,1,0,-1,0,0,0,0,1);
    QTest::newRow("scale") << QByteArray("Scale { xScale: 1.5; yScale: -2  }")
        << QTransform(1.5,0,0,0,-2,0,0,0,1);
    QTest::newRow("sequence") << QByteArray("[ Translate { x: 10; y: 20 }, Scale { xScale: 1.5; yScale: -2  } ]")
        << QTransform(1,0,0,0,1,0,10,20,1) * QTransform(1.5,0,0,0,-2,0,0,0,1);
}

void tst_QQuickItem::transforms()
{
    QFETCH(QByteArray, qml);
    QFETCH(QTransform, transform);
    QQmlComponent component(&engine);
    component.setData("import QtQuick 2.0\nItem { transform: "+qml+"}", QUrl::fromLocalFile(""));
    QQuickItem *item = qobject_cast<QQuickItem*>(component.create());
    QVERIFY(item);
    QCOMPARE(item->itemTransform(0,0), transform);
}

void tst_QQuickItem::childrenProperty()
{
    QQmlComponent component(&engine, testFileUrl("childrenProperty.qml"));

    QObject *o = component.create();
    QVERIFY(o != 0);

    QCOMPARE(o->property("test1").toBool(), true);
    QCOMPARE(o->property("test2").toBool(), true);
    QCOMPARE(o->property("test3").toBool(), true);
    QCOMPARE(o->property("test4").toBool(), true);
    QCOMPARE(o->property("test5").toBool(), true);
    delete o;
}

void tst_QQuickItem::resourcesProperty()
{
    QQmlComponent component(&engine, testFileUrl("resourcesProperty.qml"));

    QObject *o = component.create();
    QVERIFY(o != 0);

    QCOMPARE(o->property("test1").toBool(), true);
    QCOMPARE(o->property("test2").toBool(), true);
    QCOMPARE(o->property("test3").toBool(), true);
    QCOMPARE(o->property("test4").toBool(), true);
    QCOMPARE(o->property("test5").toBool(), true);
    delete o;
}

void tst_QQuickItem::propertyChanges()
{
    QQuickView *window = new QQuickView(0);
    window->setBaseSize(QSize(300, 300));
    window->setSource(testFileUrl("propertychanges.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QQuickItem *item = findItem<QQuickItem>(window->rootObject(), "item");
    QQuickItem *parentItem = findItem<QQuickItem>(window->rootObject(), "parentItem");

    QVERIFY(item);
    QVERIFY(parentItem);

    QSignalSpy parentSpy(item, SIGNAL(parentChanged(QQuickItem *)));
    QSignalSpy widthSpy(item, SIGNAL(widthChanged()));
    QSignalSpy heightSpy(item, SIGNAL(heightChanged()));
    QSignalSpy baselineOffsetSpy(item, SIGNAL(baselineOffsetChanged(qreal)));
    QSignalSpy childrenRectSpy(parentItem, SIGNAL(childrenRectChanged(QRectF)));
    QSignalSpy focusSpy(item, SIGNAL(focusChanged(bool)));
    QSignalSpy wantsFocusSpy(parentItem, SIGNAL(activeFocusChanged(bool)));
    QSignalSpy childrenChangedSpy(parentItem, SIGNAL(childrenChanged()));
    QSignalSpy xSpy(item, SIGNAL(xChanged()));
    QSignalSpy ySpy(item, SIGNAL(yChanged()));

    item->setParentItem(parentItem);
    item->setWidth(100.0);
    item->setHeight(200.0);
    item->setFocus(true);
    item->setBaselineOffset(10.0);

    QCOMPARE(item->parentItem(), parentItem);
    QCOMPARE(parentSpy.count(),1);
    QList<QVariant> parentArguments = parentSpy.first();
    QVERIFY(parentArguments.count() == 1);
    QCOMPARE(item->parentItem(), qvariant_cast<QQuickItem *>(parentArguments.at(0)));
    QCOMPARE(childrenChangedSpy.count(),1);

    item->setParentItem(parentItem);
    QCOMPARE(childrenChangedSpy.count(),1);

    QCOMPARE(item->width(), 100.0);
    QCOMPARE(widthSpy.count(),1);

    QCOMPARE(item->height(), 200.0);
    QCOMPARE(heightSpy.count(),1);

    QCOMPARE(item->baselineOffset(), 10.0);
    QCOMPARE(baselineOffsetSpy.count(),1);
    QList<QVariant> baselineOffsetArguments = baselineOffsetSpy.first();
    QVERIFY(baselineOffsetArguments.count() == 1);
    QCOMPARE(item->baselineOffset(), baselineOffsetArguments.at(0).toReal());

    QCOMPARE(parentItem->childrenRect(), QRectF(0.0,0.0,100.0,200.0));
    QCOMPARE(childrenRectSpy.count(),1);
    QList<QVariant> childrenRectArguments = childrenRectSpy.at(0);
    QVERIFY(childrenRectArguments.count() == 1);
    QCOMPARE(parentItem->childrenRect(), childrenRectArguments.at(0).toRectF());

    QCOMPARE(item->hasActiveFocus(), true);
    QCOMPARE(focusSpy.count(),1);
    QList<QVariant> focusArguments = focusSpy.first();
    QVERIFY(focusArguments.count() == 1);
    QCOMPARE(focusArguments.at(0).toBool(), true);

    QCOMPARE(parentItem->hasActiveFocus(), false);
    QCOMPARE(parentItem->hasFocus(), false);
    QCOMPARE(wantsFocusSpy.count(),0);

    item->setX(10.0);
    QCOMPARE(item->x(), 10.0);
    QCOMPARE(xSpy.count(), 1);

    item->setY(10.0);
    QCOMPARE(item->y(), 10.0);
    QCOMPARE(ySpy.count(), 1);

    delete window;
}

void tst_QQuickItem::childrenRect()
{
    QQuickView *window = new QQuickView(0);
    window->setSource(testFileUrl("childrenRect.qml"));
    window->setBaseSize(QSize(240,320));
    window->show();

    QQuickItem *o = window->rootObject();
    QQuickItem *item = o->findChild<QQuickItem*>("testItem");
    QCOMPARE(item->width(), qreal(0));
    QCOMPARE(item->height(), qreal(0));

    o->setProperty("childCount", 1);
    QCOMPARE(item->width(), qreal(10));
    QCOMPARE(item->height(), qreal(20));

    o->setProperty("childCount", 5);
    QCOMPARE(item->width(), qreal(50));
    QCOMPARE(item->height(), qreal(100));

    o->setProperty("childCount", 0);
    QCOMPARE(item->width(), qreal(0));
    QCOMPARE(item->height(), qreal(0));

    delete o;
    delete window;
}

// QTBUG-11383
void tst_QQuickItem::childrenRectBug()
{
    QQuickView *window = new QQuickView(0);

    QString warning = testFileUrl("childrenRectBug.qml").toString() + ":7:5: QML Item: Binding loop detected for property \"height\"";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning));

    window->setSource(testFileUrl("childrenRectBug.qml"));
    window->show();

    QQuickItem *o = window->rootObject();
    QQuickItem *item = o->findChild<QQuickItem*>("theItem");
    QCOMPARE(item->width(), qreal(200));
    QCOMPARE(item->height(), qreal(100));
    QCOMPARE(item->x(), qreal(100));

    delete window;
}

// QTBUG-11465
void tst_QQuickItem::childrenRectBug2()
{
    QQuickView *window = new QQuickView(0);

    QString warning1 = testFileUrl("childrenRectBug2.qml").toString() + ":7:5: QML Item: Binding loop detected for property \"width\"";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));

    QString warning2 = testFileUrl("childrenRectBug2.qml").toString() + ":7:5: QML Item: Binding loop detected for property \"height\"";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning2));

    window->setSource(testFileUrl("childrenRectBug2.qml"));
    window->show();

    QQuickRectangle *rect = qobject_cast<QQuickRectangle*>(window->rootObject());
    QVERIFY(rect);
    QQuickItem *item = rect->findChild<QQuickItem*>("theItem");
    QCOMPARE(item->width(), qreal(100));
    QCOMPARE(item->height(), qreal(110));
    QCOMPARE(item->x(), qreal(130));

    QQuickItemPrivate *rectPrivate = QQuickItemPrivate::get(rect);
    rectPrivate->setState("row");
    QCOMPARE(item->width(), qreal(210));
    QCOMPARE(item->height(), qreal(50));
    QCOMPARE(item->x(), qreal(75));

    delete window;
}

// QTBUG-12722
void tst_QQuickItem::childrenRectBug3()
{
    QQuickView *window = new QQuickView(0);
    window->setSource(testFileUrl("childrenRectBug3.qml"));
    window->show();

    //don't crash on delete
    delete window;
}

// QTBUG-13893
void tst_QQuickItem::transformCrash()
{
    QQuickView *window = new QQuickView(0);
    window->setSource(testFileUrl("transformCrash.qml"));
    window->show();

    delete window;
}

void tst_QQuickItem::implicitSize()
{
    QQuickView *window = new QQuickView(0);
    window->setSource(testFileUrl("implicitsize.qml"));
    window->show();

    QQuickItem *item = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(item);
    QCOMPARE(item->width(), qreal(80));
    QCOMPARE(item->height(), qreal(60));

    QCOMPARE(item->implicitWidth(), qreal(200));
    QCOMPARE(item->implicitHeight(), qreal(100));

    QMetaObject::invokeMethod(item, "resetSize");

    QCOMPARE(item->width(), qreal(200));
    QCOMPARE(item->height(), qreal(100));

    QMetaObject::invokeMethod(item, "changeImplicit");

    QCOMPARE(item->implicitWidth(), qreal(150));
    QCOMPARE(item->implicitHeight(), qreal(80));
    QCOMPARE(item->width(), qreal(150));
    QCOMPARE(item->height(), qreal(80));

    QMetaObject::invokeMethod(item, "assignImplicitBinding");

    QCOMPARE(item->implicitWidth(), qreal(150));
    QCOMPARE(item->implicitHeight(), qreal(80));
    QCOMPARE(item->width(), qreal(150));
    QCOMPARE(item->height(), qreal(80));

    QMetaObject::invokeMethod(item, "increaseImplicit");

    QCOMPARE(item->implicitWidth(), qreal(200));
    QCOMPARE(item->implicitHeight(), qreal(100));
    QCOMPARE(item->width(), qreal(175));
    QCOMPARE(item->height(), qreal(90));

    QMetaObject::invokeMethod(item, "changeImplicit");

    QCOMPARE(item->implicitWidth(), qreal(150));
    QCOMPARE(item->implicitHeight(), qreal(80));
    QCOMPARE(item->width(), qreal(150));
    QCOMPARE(item->height(), qreal(80));

    QMetaObject::invokeMethod(item, "assignUndefinedBinding");

    QCOMPARE(item->implicitWidth(), qreal(150));
    QCOMPARE(item->implicitHeight(), qreal(80));
    QCOMPARE(item->width(), qreal(150));
    QCOMPARE(item->height(), qreal(80));

    QMetaObject::invokeMethod(item, "increaseImplicit");

    QCOMPARE(item->implicitWidth(), qreal(200));
    QCOMPARE(item->implicitHeight(), qreal(100));
    QCOMPARE(item->width(), qreal(175));
    QCOMPARE(item->height(), qreal(90));

    QMetaObject::invokeMethod(item, "changeImplicit");

    QCOMPARE(item->implicitWidth(), qreal(150));
    QCOMPARE(item->implicitHeight(), qreal(80));
    QCOMPARE(item->width(), qreal(150));
    QCOMPARE(item->height(), qreal(80));

    delete window;
}

void tst_QQuickItem::qtbug_16871()
{
    QQmlComponent component(&engine, testFileUrl("qtbug_16871.qml"));
    QObject *o = component.create();
    QVERIFY(o != 0);
    delete o;
}


void tst_QQuickItem::visibleChildren()
{
    QQuickView *window = new QQuickView(0);
    window->setSource(testFileUrl("visiblechildren.qml"));
    window->show();

    QQuickItem *root = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(root);

    QCOMPARE(root->property("test1_1").toBool(), true);
    QCOMPARE(root->property("test1_2").toBool(), true);
    QCOMPARE(root->property("test1_3").toBool(), true);
    QCOMPARE(root->property("test1_4").toBool(), true);

    QMetaObject::invokeMethod(root, "hideFirstAndLastRowChild");
    QCOMPARE(root->property("test2_1").toBool(), true);
    QCOMPARE(root->property("test2_2").toBool(), true);
    QCOMPARE(root->property("test2_3").toBool(), true);
    QCOMPARE(root->property("test2_4").toBool(), true);

    QMetaObject::invokeMethod(root, "showLastRowChildsLastChild");
    QCOMPARE(root->property("test3_1").toBool(), true);
    QCOMPARE(root->property("test3_2").toBool(), true);
    QCOMPARE(root->property("test3_3").toBool(), true);
    QCOMPARE(root->property("test3_4").toBool(), true);

    QMetaObject::invokeMethod(root, "showLastRowChild");
    QCOMPARE(root->property("test4_1").toBool(), true);
    QCOMPARE(root->property("test4_2").toBool(), true);
    QCOMPARE(root->property("test4_3").toBool(), true);
    QCOMPARE(root->property("test4_4").toBool(), true);

    QString warning1 = testFileUrl("visiblechildren.qml").toString() + ":87: TypeError: Cannot read property 'visibleChildren' of null";
    QTest::ignoreMessage(QtWarningMsg, qPrintable(warning1));
    QMetaObject::invokeMethod(root, "tryWriteToReadonlyVisibleChildren");

    QMetaObject::invokeMethod(root, "reparentVisibleItem3");
    QCOMPARE(root->property("test6_1").toBool(), true);
    QCOMPARE(root->property("test6_2").toBool(), true);
    QCOMPARE(root->property("test6_3").toBool(), true);
    QCOMPARE(root->property("test6_4").toBool(), true);

    QMetaObject::invokeMethod(root, "reparentImlicitlyInvisibleItem4_1");
    QCOMPARE(root->property("test7_1").toBool(), true);
    QCOMPARE(root->property("test7_2").toBool(), true);
    QCOMPARE(root->property("test7_3").toBool(), true);
    QCOMPARE(root->property("test7_4").toBool(), true);

    // FINALLY TEST THAT EVERYTHING IS AS EXPECTED
    QCOMPARE(root->property("test8_1").toBool(), true);
    QCOMPARE(root->property("test8_2").toBool(), true);
    QCOMPARE(root->property("test8_3").toBool(), true);
    QCOMPARE(root->property("test8_4").toBool(), true);
    QCOMPARE(root->property("test8_5").toBool(), true);

    delete window;
}

void tst_QQuickItem::parentLoop()
{
    QQuickView *window = new QQuickView(0);

    QTest::ignoreMessage(QtWarningMsg, "QQuickItem::setParentItem: Parent is already part of this items subtree.");
    window->setSource(testFileUrl("parentLoop.qml"));

    QQuickItem *root = qobject_cast<QQuickItem*>(window->rootObject());
    QVERIFY(root);

    QQuickItem *item1 = root->findChild<QQuickItem*>("item1");
    QVERIFY(item1);
    QCOMPARE(item1->parentItem(), root);

    QQuickItem *item2 = root->findChild<QQuickItem*>("item2");
    QVERIFY(item2);
    QCOMPARE(item2->parentItem(), item1);

    delete window;
}

void tst_QQuickItem::contains_data()
{
    QTest::addColumn<bool>("circleTest");
    QTest::addColumn<bool>("insideTarget");
    QTest::addColumn<QList<QPoint> >("points");

    QList<QPoint> points;

    points << QPoint(176, 176)
           << QPoint(176, 226)
           << QPoint(226, 176)
           << QPoint(226, 226)
           << QPoint(150, 200)
           << QPoint(200, 150)
           << QPoint(200, 250)
           << QPoint(250, 200);
    QTest::newRow("hollow square: testing points inside") << false << true << points;

    points.clear();
    points << QPoint(162, 162)
           << QPoint(162, 242)
           << QPoint(242, 162)
           << QPoint(242, 242)
           << QPoint(200, 200)
           << QPoint(175, 200)
           << QPoint(200, 175)
           << QPoint(200, 228)
           << QPoint(228, 200)
           << QPoint(200, 122)
           << QPoint(122, 200)
           << QPoint(200, 280)
           << QPoint(280, 200);
    QTest::newRow("hollow square: testing points outside") << false << false << points;

    points.clear();
    points << QPoint(174, 174)
           << QPoint(174, 225)
           << QPoint(225, 174)
           << QPoint(225, 225)
           << QPoint(165, 200)
           << QPoint(200, 165)
           << QPoint(200, 235)
           << QPoint(235, 200);
    QTest::newRow("hollow circle: testing points inside") << true << true << points;

    points.clear();
    points << QPoint(160, 160)
           << QPoint(160, 240)
           << QPoint(240, 160)
           << QPoint(240, 240)
           << QPoint(200, 200)
           << QPoint(185, 185)
           << QPoint(185, 216)
           << QPoint(216, 185)
           << QPoint(216, 216)
           << QPoint(145, 200)
           << QPoint(200, 145)
           << QPoint(255, 200)
           << QPoint(200, 255);
    QTest::newRow("hollow circle: testing points outside") << true << false << points;
}

void tst_QQuickItem::contains()
{
    QFETCH(bool, circleTest);
    QFETCH(bool, insideTarget);
    QFETCH(QList<QPoint>, points);

    QQuickView *window = new QQuickView(0);
    window->rootContext()->setContextProperty("circleShapeTest", circleTest);
    window->setBaseSize(QSize(400, 400));
    window->setSource(testFileUrl("hollowTestItem.qml"));
    window->show();
    window->requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(window));
    QVERIFY(QGuiApplication::focusWindow() == window);

    QQuickItem *root = qobject_cast<QQuickItem *>(window->rootObject());
    QVERIFY(root);

    HollowTestItem *hollowItem = root->findChild<HollowTestItem *>("hollowItem");
    QVERIFY(hollowItem);

    foreach (const QPoint &point, points) {
        // check mouse hover
        QTest::mouseMove(window, point);
        QTest::qWait(10);
        QCOMPARE(hollowItem->isHovered(), insideTarget);

        // check mouse press
        QTest::mousePress(window, Qt::LeftButton, 0, point);
        QTest::qWait(10);
        QCOMPARE(hollowItem->isPressed(), insideTarget);

        // check mouse release
        QTest::mouseRelease(window, Qt::LeftButton, 0, point);
        QTest::qWait(10);
        QCOMPARE(hollowItem->isPressed(), false);
    }

    delete window;
}

void tst_QQuickItem::childAt()
{
    QQuickItem parent;

    QQuickItem child1;
    child1.setX(0);
    child1.setY(0);
    child1.setWidth(100);
    child1.setHeight(100);
    child1.setParentItem(&parent);

    QQuickItem child2;
    child2.setX(50);
    child2.setY(50);
    child2.setWidth(100);
    child2.setHeight(100);
    child2.setParentItem(&parent);

    QQuickItem child3;
    child3.setX(0);
    child3.setY(200);
    child3.setWidth(50);
    child3.setHeight(50);
    child3.setParentItem(&parent);

    QCOMPARE(parent.childAt(0, 0), &child1);
    QCOMPARE(parent.childAt(0, 100), &child1);
    QCOMPARE(parent.childAt(25, 25), &child1);
    QCOMPARE(parent.childAt(25, 75), &child1);
    QCOMPARE(parent.childAt(75, 25), &child1);
    QCOMPARE(parent.childAt(75, 75), &child2);
    QCOMPARE(parent.childAt(150, 150), &child2);
    QCOMPARE(parent.childAt(25, 200), &child3);
    QCOMPARE(parent.childAt(0, 150), static_cast<QQuickItem *>(0));
    QCOMPARE(parent.childAt(300, 300), static_cast<QQuickItem *>(0));
}


QTEST_MAIN(tst_QQuickItem)

#include "tst_qquickitem.moc"
