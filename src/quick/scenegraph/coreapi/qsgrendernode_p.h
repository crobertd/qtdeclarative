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

#ifndef QSGRENDERNODE_P_H
#define QSGRENDERNODE_P_H

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

#include "qsgnode.h"
#include <private/qtquickglobal_p.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class Q_QUICK_PRIVATE_EXPORT QSGRenderNode : public QSGNode
{
public:
    enum StateFlag
    {
        DepthState = 0x01, // depth mask, depth test enable, depth func, clear depth
        StencilState = 0x02, // stencil mask, stencil test enable, stencil op, stencil func, clear stencil
        ScissorState = 0x04, // scissor enable, scissor test enable
        ColorState = 0x08, // clear color, color mask
        BlendState = 0x10, // blend enable, blend func
        CullState = 0x20, // front face, cull face enable
        ViewportState = 0x40 // viewport
    };
    Q_DECLARE_FLAGS(StateFlags, StateFlag)

    struct RenderState
    {
        // The model-view matrix can be retrieved with matrix().
        // The opacity can be retrieved with inheritedOpacity().
        const QMatrix4x4 *projectionMatrix;
        QRect scissorRect;
        int stencilValue;

        bool stencilEnabled;
        bool scissorEnabled;
    };

    QSGRenderNode();

    virtual StateFlags changedStates() = 0;
    virtual void render(const RenderState &state) = 0;

    const QMatrix4x4 *matrix() const { return m_matrix; }
    const QSGClipNode *clipList() const { return m_clip_list; }

    void setInheritedOpacity(qreal opacity);
    qreal inheritedOpacity() const { return m_opacity; }

private:
    friend class QSGNodeUpdater;

    const QMatrix4x4 *m_matrix;
    const QSGClipNode *m_clip_list;
    qreal m_opacity;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSGRenderNode::StateFlags)

QT_END_NAMESPACE

QT_END_HEADER

#endif
