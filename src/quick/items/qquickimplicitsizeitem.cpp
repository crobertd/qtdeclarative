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

#include "qquickimplicitsizeitem_p.h"
#include "qquickimplicitsizeitem_p_p.h"

QT_BEGIN_NAMESPACE

void QQuickImplicitSizeItemPrivate::implicitWidthChanged()
{
    Q_Q(QQuickImplicitSizeItem);
    for (int ii = 0; ii < changeListeners.count(); ++ii) {
        const QQuickItemPrivate::ChangeListener &change = changeListeners.at(ii);
        if (change.types & QQuickItemPrivate::ImplicitWidth) {
            change.listener->itemImplicitWidthChanged(q);
        }
    }
    emit q->implicitWidthChanged();
}

void QQuickImplicitSizeItemPrivate::implicitHeightChanged()
{
    Q_Q(QQuickImplicitSizeItem);
    for (int ii = 0; ii < changeListeners.count(); ++ii) {
        const QQuickItemPrivate::ChangeListener &change = changeListeners.at(ii);
        if (change.types & QQuickItemPrivate::ImplicitHeight) {
            change.listener->itemImplicitHeightChanged(q);
        }
    }
    emit q->implicitHeightChanged();
}

QQuickImplicitSizeItem::QQuickImplicitSizeItem(QQuickImplicitSizeItemPrivate &dd, QQuickItem *parent)
    : QQuickItem(dd, parent)
{
}

QT_END_NAMESPACE
