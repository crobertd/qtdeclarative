/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtQuick module of the Qt Toolkit.
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

#ifndef QQUICKANIMATEDSPRITE_P_H
#define QQUICKANIMATEDSPRITE_P_H

#include <QtQuick/QQuickItem>
#include <private/qquicksprite_p.h>
#include <QTime>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGContext;
class QQuickSprite;
class QQuickSpriteEngine;
class QSGGeometryNode;
class QQuickAnimatedSpriteMaterial;
class Q_AUTOTEST_EXPORT QQuickAnimatedSprite : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool interpolate READ interpolate WRITE setInterpolate NOTIFY interpolateChanged)
    //###try to share similar spriteEngines for less overhead?
    //These properties come out of QQuickSprite, since an AnimatedSprite is a renderer for a single sprite
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool reverse READ reverse WRITE setReverse NOTIFY reverseChanged)
    Q_PROPERTY(bool frameSync READ frameSync WRITE setFrameSync NOTIFY frameSyncChanged)
    Q_PROPERTY(int frameCount READ frameCount WRITE setFrameCount NOTIFY frameCountChanged)
    //If frame height or width is not specified, it is assumed to be a single long row of square frames.
    //Otherwise, it can be multiple contiguous rows, when one row runs out the next will be used.
    Q_PROPERTY(int frameHeight READ frameHeight WRITE setFrameHeight NOTIFY frameHeightChanged)
    Q_PROPERTY(int frameWidth READ frameWidth WRITE setFrameWidth NOTIFY frameWidthChanged)
    Q_PROPERTY(int frameX READ frameX WRITE setFrameX NOTIFY frameXChanged)
    Q_PROPERTY(int frameY READ frameY WRITE setFrameY NOTIFY frameYChanged)
    //Precedence order: frameRate, frameDuration
    Q_PROPERTY(qreal frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged RESET resetFrameRate)
    Q_PROPERTY(int frameDuration READ frameDuration WRITE setFrameDuration NOTIFY frameDurationChanged RESET resetFrameDuration)
    //Extra Simple Sprite Stuff
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(bool paused READ paused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(int currentFrame READ currentFrame WRITE setCurrentFrame NOTIFY currentFrameChanged)

    Q_ENUMS(LoopParameters)
public:
    explicit QQuickAnimatedSprite(QQuickItem *parent = 0);
    enum LoopParameters {
        Infinite = -1
    };

    bool running() const
    {
        return m_running;
    }

    bool interpolate() const
    {
        return m_interpolate;
    }

    QUrl source() const
    {
        return m_sprite->source();
    }

    bool reverse() const
    {
        return m_sprite->reverse();
    }

    bool frameSync() const
    {
        return m_sprite->frameSync();
    }

    int frameCount() const
    {
        return m_sprite->frames();
    }

    int frameHeight() const
    {
        return m_sprite->frameHeight();
    }

    int frameWidth() const
    {
        return m_sprite->frameWidth();
    }

    int frameX() const
    {
        return m_sprite->frameX();
    }

    int frameY() const
    {
        return m_sprite->frameY();
    }

    qreal frameRate() const
    {
        return m_sprite->frameRate();
    }

    int frameDuration() const
    {
        return m_sprite->frameDuration();
    }

    int loops() const
    {
        return m_loops;
    }

    bool paused() const
    {
        return m_paused;
    }

    int currentFrame() const
    {
        return m_curFrame;
    }

signals:

    void pausedChanged(bool arg);
    void runningChanged(bool arg);
    void interpolateChanged(bool arg);

    void sourceChanged(QUrl arg);

    void reverseChanged(bool arg);

    void frameSyncChanged(bool arg);

    void frameCountChanged(int arg);

    void frameHeightChanged(int arg);

    void frameWidthChanged(int arg);

    void frameXChanged(int arg);

    void frameYChanged(int arg);

    void frameRateChanged(qreal arg);

    void frameDurationChanged(int arg);

    void loopsChanged(int arg);

    void currentFrameChanged(int arg);

public slots:
    void start();
    void stop();
    void restart() {stop(); start();}
    void advance(int frames=1);
    void pause();
    void resume();

    void setRunning(bool arg)
    {
        if (m_running != arg) {
            if (m_running)
                stop();
            else
                start();
        }
    }

    void setPaused(bool arg)
    {
        if (m_paused != arg) {
            if (m_paused)
                resume();
            else
                pause();
        }
    }

    void setInterpolate(bool arg)
    {
        if (m_interpolate != arg) {
            m_interpolate = arg;
            emit interpolateChanged(arg);
        }
    }

    void setSource(QUrl arg)
    {
        if (m_sprite->m_source != arg) {
            m_sprite->setSource(arg);
            emit sourceChanged(arg);
            reloadImage();
        }
    }

    void setReverse(bool arg)
    {
        if (m_sprite->m_reverse != arg) {
            m_sprite->setReverse(arg);
            emit reverseChanged(arg);
        }
    }

    void setFrameSync(bool arg)
    {
        if (m_sprite->m_frameSync != arg) {
            m_sprite->setFrameSync(arg);
            emit frameSyncChanged(arg);
            if (m_running)
                restart();
        }
    }

    void setFrameCount(int arg)
    {
        if (m_sprite->m_frames != arg) {
            m_sprite->setFrameCount(arg);
            emit frameCountChanged(arg);
            reloadImage();
        }
    }

    void setFrameHeight(int arg)
    {
        if (m_sprite->m_frameHeight != arg) {
            m_sprite->setFrameHeight(arg);
            emit frameHeightChanged(arg);
            reloadImage();
        }
    }

    void setFrameWidth(int arg)
    {
        if (m_sprite->m_frameWidth != arg) {
            m_sprite->setFrameWidth(arg);
            emit frameWidthChanged(arg);
            reloadImage();
        }
    }

    void setFrameX(int arg)
    {
        if (m_sprite->m_frameX != arg) {
            m_sprite->setFrameX(arg);
            emit frameXChanged(arg);
            reloadImage();
        }
    }

    void setFrameY(int arg)
    {
        if (m_sprite->m_frameY != arg) {
            m_sprite->setFrameY(arg);
            emit frameYChanged(arg);
            reloadImage();
        }
    }

    void setFrameRate(qreal arg)
    {
        if (m_sprite->m_frameRate != arg) {
            m_sprite->setFrameRate(arg);
            emit frameRateChanged(arg);
            if (m_running)
                restart();
        }
    }

    void setFrameDuration(int arg)
    {
        if (m_sprite->m_frameDuration != arg) {
            m_sprite->setFrameDuration(arg);
            emit frameDurationChanged(arg);
            if (m_running)
                restart();
        }
    }

    void resetFrameRate()
    {
        setFrameRate(-1.0);
    }

    void resetFrameDuration()
    {
        setFrameDuration(-1);
    }

    void setLoops(int arg)
    {
        if (m_loops != arg) {
            m_loops = arg;
            emit loopsChanged(arg);
        }
    }

    void setCurrentFrame(int arg) //TODO-C: Probably only works when paused
    {
        if (m_curFrame != arg) {
            m_curFrame = arg;
            emit currentFrameChanged(arg); //TODO-C Only emitted on manual advance!
        }
    }


private slots:
    void createEngine();
    void sizeVertices();

protected:
    void reset();
    void componentComplete();
    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);
private:
    bool isCurrentFrameChangedConnected();
    void prepareNextFrame();
    void reloadImage();
    QSGGeometryNode* buildNode();
    QSGGeometryNode *m_node;
    QQuickAnimatedSpriteMaterial *m_material;
    QQuickSprite* m_sprite;
    QQuickSpriteEngine* m_spriteEngine;
    QTime m_timestamp;
    int m_curFrame;
    bool m_pleaseReset;
    bool m_running;
    bool m_paused;
    bool m_interpolate;
    QSizeF m_sheetSize;
    int m_loops;
    int m_curLoop;
    int m_pauseOffset;
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QQUICKANIMATEDSPRITE_P_H
