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

#ifndef QQUICKCANVASITEM_P_H
#define QQUICKCANVASITEM_P_H

#include <QtQuick/qquickitem.h>
#include <private/qv8engine_p.h>
#include <QtCore/QThread>
#include <QtGui/QImage>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQuickCanvasContext;

class QQuickCanvasItemPrivate;
class QSGTexture;
class QQuickPixmap;

class QQuickCanvasPixmap : public QQmlRefCount
{
public:
    QQuickCanvasPixmap(const QImage& image, QQuickWindow *window);
    QQuickCanvasPixmap(QQuickPixmap *pixmap, QQuickWindow *window);
    ~QQuickCanvasPixmap();

    QSGTexture *texture();
    QImage image();

    qreal width() const;
    qreal height() const;
    bool isValid() const;
    QQuickPixmap *pixmap() const { return m_pixmap;}

private:
    QQuickPixmap *m_pixmap;
    QImage m_image;
    QSGTexture *m_texture;
    QQuickWindow *m_window;
};

class QQuickCanvasItem : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(RenderTarget)
    Q_ENUMS(RenderStrategy)

    Q_PROPERTY(bool available READ isAvailable NOTIFY availableChanged)
    Q_PROPERTY(QString contextType READ contextType WRITE setContextType NOTIFY contextTypeChanged)
    Q_PROPERTY(QQmlV8Handle context READ context NOTIFY contextChanged)
    Q_PROPERTY(QSizeF canvasSize READ canvasSize WRITE setCanvasSize NOTIFY canvasSizeChanged)
    Q_PROPERTY(QSize tileSize READ tileSize WRITE setTileSize NOTIFY tileSizeChanged)
    Q_PROPERTY(QRectF canvasWindow READ canvasWindow WRITE setCanvasWindow NOTIFY canvasWindowChanged)
    Q_PROPERTY(RenderTarget renderTarget READ renderTarget WRITE setRenderTarget NOTIFY renderTargetChanged)
    Q_PROPERTY(RenderStrategy renderStrategy READ renderStrategy WRITE setRenderStrategy NOTIFY renderStrategyChanged)

public:
    enum RenderTarget {
        Image,
        FramebufferObject
    };

    enum RenderStrategy {
        Immediate,
        Threaded,
        Cooperative
    };

    QQuickCanvasItem(QQuickItem *parent = 0);
    ~QQuickCanvasItem();

    bool isAvailable() const;

    QString contextType() const;
    void setContextType(const QString &contextType);

    QQmlV8Handle context() const;

    QSizeF canvasSize() const;
    void setCanvasSize(const QSizeF &);

    QSize tileSize() const;
    void setTileSize(const QSize &);

    QRectF canvasWindow() const;
    void setCanvasWindow(const QRectF& rect);

    RenderTarget renderTarget() const;
    void setRenderTarget(RenderTarget target);

    RenderStrategy renderStrategy() const;
    void setRenderStrategy(RenderStrategy strategy);

    QQuickCanvasContext *rawContext() const;

    QImage toImage(const QRectF& rect = QRectF()) const;

    Q_INVOKABLE void getContext(QQmlV8Function *args);

    Q_INVOKABLE void requestAnimationFrame(QQmlV8Function *args);
    Q_INVOKABLE void cancelRequestAnimationFrame(QQmlV8Function *args);

    Q_INVOKABLE void requestPaint();
    Q_INVOKABLE void markDirty(const QRectF& dirtyRect = QRectF());

    Q_INVOKABLE bool save(const QString &filename) const;
    Q_INVOKABLE QString toDataURL(const QString& type = QLatin1String("image/png")) const;
    QQmlRefPointer<QQuickCanvasPixmap> loadedPixmap(const QUrl& url);

Q_SIGNALS:
    void paint(const QRect &region);
    void painted();
    void availableChanged();
    void contextTypeChanged();
    void contextChanged();
    void canvasSizeChanged();
    void tileSizeChanged();
    void canvasWindowChanged();
    void renderTargetChanged();
    void renderStrategyChanged();
    void imageLoaded();

public Q_SLOTS:
    void loadImage(const QUrl& url);
    void unloadImage(const QUrl& url);
    bool isImageLoaded(const QUrl& url) const;
    bool isImageLoading(const QUrl& url) const;
    bool isImageError(const QUrl& url) const;

private Q_SLOTS:
    void sceneGraphInitialized();
    void checkAnimationCallbacks();

protected:
    void componentComplete();
    void itemChange(QQuickItem::ItemChange, const QQuickItem::ItemChangeData &);
    void updatePolish();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void releaseResources();
private:
    Q_DECLARE_PRIVATE(QQuickCanvasItem)
    Q_INVOKABLE void delayedCreate();
    bool createContext(const QString &contextType);
    void initializeContext(QQuickCanvasContext *context, const QVariantMap &args = QVariantMap());
    QRect tiledRect(const QRectF &window, const QSize &tileSize);
    bool isPaintConnected();
};

class QQuickContext2DRenderThread : public QThread
{
    Q_OBJECT
public:
    QQuickContext2DRenderThread(QQmlEngine *eng);
    ~QQuickContext2DRenderThread();

    static QQuickContext2DRenderThread *instance(QQmlEngine *engine);

private:
    QQmlEngine *m_engine;
    QObject *m_eventLoopQuitHack;
    static QHash<QQmlEngine *,QQuickContext2DRenderThread*> renderThreads;
    static QMutex renderThreadsMutex;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QQuickCanvasItem)

QT_END_HEADER

#endif //QQUICKCANVASITEM_P_H
