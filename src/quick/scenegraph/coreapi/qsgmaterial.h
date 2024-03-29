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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <QtQuick/qtquickglobal.h>
#include <qopenglshaderprogram.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QSGMaterial;

class Q_QUICK_EXPORT QSGMaterialShader
{
public:
    class Q_QUICK_EXPORT RenderState {
    public:
        enum DirtyState
        {
            DirtyMatrix         = 0x0001,
            DirtyOpacity        = 0x0002
        };
        Q_DECLARE_FLAGS(DirtyStates, DirtyState)

        inline DirtyStates dirtyStates() const { return m_dirty; }

        inline bool isMatrixDirty() const { return m_dirty & DirtyMatrix; }
        inline bool isOpacityDirty() const { return m_dirty & DirtyOpacity; }

        float opacity() const;
        QMatrix4x4 combinedMatrix() const;
        QMatrix4x4 modelViewMatrix() const;
        QRect viewportRect() const;
        QRect deviceRect() const;
        float determinant() const;

        QOpenGLContext *context() const;

    private:
        friend class QSGRenderer;
        DirtyStates m_dirty;
        const void *m_data;
    };

    QSGMaterialShader();
    virtual ~QSGMaterialShader() {};

    virtual void activate();
    virtual void deactivate();
    // First time a material is used, oldMaterial is null.
    virtual void updateState(const RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial);
    virtual char const *const *attributeNames() const = 0; // Array must end with null.

    inline QOpenGLShaderProgram *program() { return &m_program; }

protected:

    friend class QSGContext;

    virtual void compile();
    virtual void initialize() { }

    virtual const char *vertexShader() const = 0;
    virtual const char *fragmentShader() const = 0;

private:
    QOpenGLShaderProgram m_program;
    void *m_reserved;
};

struct QSGMaterialType { };

class Q_QUICK_EXPORT QSGMaterial
{
public:
    enum Flag {
        Blending            = 0x0001,
        RequiresDeterminant = 0x0002, // Allow precalculated translation and 2D rotation
        RequiresFullMatrixExceptTranslate = 0x0004 | RequiresDeterminant, // Allow precalculated translation
        RequiresFullMatrix  = 0x0008 | RequiresFullMatrixExceptTranslate
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    QSGMaterial();
    virtual ~QSGMaterial();

    virtual QSGMaterialType *type() const = 0;
    virtual QSGMaterialShader *createShader() const = 0;
    virtual int compare(const QSGMaterial *other) const;

    QSGMaterial::Flags flags() const { return m_flags; }
    void setFlag(Flags flags, bool on = true);

private:
    Flags m_flags;
    void *m_reserved;
    Q_DISABLE_COPY(QSGMaterial)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QSGMaterial::Flags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QSGMaterialShader::RenderState::DirtyStates)

QT_END_NAMESPACE

QT_END_HEADER

#endif
