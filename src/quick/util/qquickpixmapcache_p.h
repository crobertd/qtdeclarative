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

#ifndef QQUICKPIXMAPCACHE_H
#define QQUICKPIXMAPCACHE_H

#include <QtCore/qcoreapplication.h>
#include <QtCore/qstring.h>
#include <QtGui/qpixmap.h>
#include <QtCore/qurl.h>
#include <private/qtquickglobal_p.h>
#include <QtQuick/qquickimageprovider.h>

#include <private/qintrusivelist_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QQmlEngine;
class QQuickPixmapData;
class QQuickTextureFactory;

class QQuickDefaultTextureFactory : public QQuickTextureFactory
{
    Q_OBJECT
public:
    QQuickDefaultTextureFactory(const QImage &i)
        : im(i)
    {
    }

    QSGTexture *createTexture(QQuickWindow *window) const;
    QSize textureSize() const { return im.size(); }
    int textureByteCount() const { return im.byteCount(); }
    QImage image() const { return im; }

private:
    QImage im;
};

class Q_QUICK_PRIVATE_EXPORT QQuickPixmap
{
    Q_DECLARE_TR_FUNCTIONS(QQuickPixmap)
public:
    QQuickPixmap();
    QQuickPixmap(QQmlEngine *, const QUrl &);
    QQuickPixmap(QQmlEngine *, const QUrl &, const QSize &);
    ~QQuickPixmap();

    enum Status { Null, Ready, Error, Loading };

    enum Option {
        Asynchronous = 0x00000001,
        Cache        = 0x00000002
    };
    Q_DECLARE_FLAGS(Options, Option)

    bool isNull() const;
    bool isReady() const;
    bool isError() const;
    bool isLoading() const;

    Status status() const;
    QString error() const;
    const QUrl &url() const;
    const QSize &implicitSize() const;
    const QSize &requestSize() const;
    QImage image() const;
    void setImage(const QImage &);

    QQuickTextureFactory *textureFactory() const;

    QRect rect() const;
    int width() const;
    int height() const;

    void load(QQmlEngine *, const QUrl &);
    void load(QQmlEngine *, const QUrl &, QQuickPixmap::Options options);
    void load(QQmlEngine *, const QUrl &, const QSize &);
    void load(QQmlEngine *, const QUrl &, const QSize &, QQuickPixmap::Options options);

    void clear();
    void clear(QObject *);

    bool connectFinished(QObject *, const char *);
    bool connectFinished(QObject *, int);
    bool connectDownloadProgress(QObject *, const char *);
    bool connectDownloadProgress(QObject *, int);

    static void purgeCache();

private:
    Q_DISABLE_COPY(QQuickPixmap)
    QQuickPixmapData *d;
    QIntrusiveListNode dataListNode;
    friend class QQuickPixmapData;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QQuickPixmap::Options)

QT_END_NAMESPACE

QT_END_HEADER

#endif // QQUICKPIXMAPCACHE_H
