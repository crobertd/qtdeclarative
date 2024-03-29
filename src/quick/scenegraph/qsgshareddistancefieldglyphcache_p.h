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

#ifndef QSGSHAREDDISTANCEFIELDGLYPHCACHE_H
#define QSGSHAREDDISTANCEFIELDGLYPHCACHE_H

#include <QtCore/qwaitcondition.h>
#include <private/qsgadaptationlayer_p.h>
#include <private/qqmlguard_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QPlatformSharedGraphicsCache;
class QSGSharedDistanceFieldGlyphCache : public QObject, public QSGDistanceFieldGlyphCache
{
    Q_OBJECT
public:
    explicit QSGSharedDistanceFieldGlyphCache(const QByteArray &cacheId,
                                              QPlatformSharedGraphicsCache *sharedGraphicsCache,
                                              QSGDistanceFieldGlyphCacheManager *man,
                                              QOpenGLContext *c,
                                              const QRawFont &font);
    ~QSGSharedDistanceFieldGlyphCache();

    void registerOwnerElement(QQuickItem *ownerElement);
    void unregisterOwnerElement(QQuickItem *ownerElement);
    void processPendingGlyphs();

    void requestGlyphs(const QSet<glyph_t> &glyphs);
    void referenceGlyphs(const QSet<glyph_t> &glyphs);
    void storeGlyphs(const QHash<glyph_t, QImage> &glyphs);
    void releaseGlyphs(const QSet<glyph_t> &glyphs);

Q_SIGNALS:
    void glyphsPending();

private Q_SLOTS:
    void reportItemsMissing(const QByteArray &cacheId, const QVector<quint32> &itemIds);
    void reportItemsAvailable(const QByteArray &cacheId,
                              void *bufferId,
                              const QVector<quint32> &itemIds,
                              const QVector<QPoint> &positions);
    void reportItemsUpdated(const QByteArray &cacheId,
                            void *bufferId,
                            const QVector<quint32> &itemIds,
                            const QVector<QPoint> &positions);
    void reportItemsInvalidated(const QByteArray &cacheId, const QVector<quint32> &itemIds);

    void sceneGraphUpdateStarted();
    void sceneGraphUpdateDone();

private:
    void waitForGlyphs();
    void saveTexture(GLuint textureId, int width, int height);

    QSet<quint32> m_requestedGlyphsThatHaveNotBeenReturned;
    QSet<quint32> m_requestedGlyphs;
    QWaitCondition m_pendingGlyphsCondition;
    QByteArray m_cacheId;
    QPlatformSharedGraphicsCache *m_sharedGraphicsCache;
    QMutex m_pendingGlyphsMutex;

    QSet<glyph_t> m_pendingInvalidatedGlyphs;
    QSet<glyph_t> m_pendingMissingGlyphs;

    struct PendingGlyph
    {
        PendingGlyph() : buffer(0) {}

        void *buffer;
        QSize bufferSize;
        QPoint position;
    };

    struct Owner
    {
        Owner() : ref(0) {}
        Owner(const Owner &o) : item(o.item), ref(o.ref) {}
        Owner &operator =(const Owner &o) { item = o.item; ref = o.ref; return *this; }

        QQmlGuard<QQuickItem> item;
        int ref;
    };

    QHash<quint32, PendingGlyph> m_pendingReadyGlyphs;
    QHash<glyph_t, void *> m_bufferForGlyph;
    QHash<QQuickItem *, Owner> m_registeredOwners;

    bool m_isInSceneGraphUpdate;
    bool m_hasPostedEvents;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSGSHAREDDISTANCEFIELDGLYPHCACHE_H
