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

#ifndef QQUICKTEXTEDIT_P_P_H
#define QQUICKTEXTEDIT_P_P_H

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

#include "qquicktextedit_p.h"
#include "qquickimplicitsizeitem_p_p.h"
#include "qquicktextcontrol_p.h"

#include <QtQml/qqml.h>

QT_BEGIN_NAMESPACE
class QTextLayout;
class QQuickTextDocumentWithImageResources;
class QQuickTextControl;
class QQuickTextEditPrivate : public QQuickImplicitSizeItemPrivate
{
public:
    Q_DECLARE_PUBLIC(QQuickTextEdit)

    typedef QQuickTextEdit Public;

    QQuickTextEditPrivate()
        : color(QRgb(0xFF000000)), selectionColor(QRgb(0xFF000080)), selectedTextColor(QRgb(0xFFFFFFFF))
        , textMargin(0.0), xoff(0), yoff(0), font(sourceFont), cursorComponent(0), cursorItem(0), document(0), control(0)
        , lastSelectionStart(0), lastSelectionEnd(0), lineCount(0)
        , hAlign(QQuickTextEdit::AlignLeft), vAlign(QQuickTextEdit::AlignTop)
        , format(QQuickTextEdit::PlainText), wrapMode(QQuickTextEdit::NoWrap)
        , renderType(QQuickTextEdit::QtRendering)
        , contentDirection(Qt::LayoutDirectionAuto)
        , mouseSelectionMode(QQuickTextEdit::SelectCharacters)
#ifndef QT_NO_IM
        , inputMethodHints(Qt::ImhNone)
#endif
        , updateType(UpdatePaintNode)
        , documentDirty(true), dirty(false), richText(false), cursorVisible(false), cursorPending(false)
        , focusOnPress(true), persistentSelection(false), requireImplicitWidth(false)
        , selectByMouse(false), canPaste(false), canPasteValid(false), hAlignImplicit(true)
        , textCached(true), inLayout(false)
    {
    }

    static QQuickTextEditPrivate *get(QQuickTextEdit *item) {
        return static_cast<QQuickTextEditPrivate *>(QObjectPrivate::get(item)); }

    void init();

    void updateDefaultTextOption();
    void relayoutDocument();
    bool determineHorizontalAlignment();
    bool setHAlign(QQuickTextEdit::HAlignment, bool forceAlign = false);
    void mirrorChange();
    qreal getImplicitWidth() const;
    Qt::LayoutDirection textDirection(const QString &text) const;

    void setNativeCursorEnabled(bool enabled) { control->setCursorWidth(enabled ? 1 : 0); }

    QColor color;
    QColor selectionColor;
    QColor selectedTextColor;

    QSizeF contentSize;

    qreal textMargin;
    qreal xoff;
    qreal yoff;

    QString text;
    QUrl baseUrl;
    QFont sourceFont;
    QFont font;

    QQmlComponent* cursorComponent;
    QQuickItem* cursorItem;
    QQuickTextDocumentWithImageResources *document;
    QQuickTextControl *control;

    int lastSelectionStart;
    int lastSelectionEnd;
    int lineCount;

    enum UpdateType {
        UpdateNone,
        UpdateOnlyPreprocess,
        UpdatePaintNode
    };

    QQuickTextEdit::HAlignment hAlign;
    QQuickTextEdit::VAlignment vAlign;
    QQuickTextEdit::TextFormat format;
    QQuickTextEdit::WrapMode wrapMode;
    QQuickTextEdit::RenderType renderType;
    Qt::LayoutDirection contentDirection;
    QQuickTextEdit::SelectionMode mouseSelectionMode;
#ifndef QT_NO_IM
    Qt::InputMethodHints inputMethodHints;
#endif
    UpdateType updateType;

    bool documentDirty : 1;
    bool dirty : 1;
    bool richText : 1;
    bool cursorVisible : 1;
    bool cursorPending : 1;
    bool focusOnPress : 1;
    bool persistentSelection : 1;
    bool requireImplicitWidth:1;
    bool selectByMouse:1;
    bool canPaste:1;
    bool canPasteValid:1;
    bool hAlignImplicit:1;
    bool textCached:1;
    bool inLayout:1;
};

QT_END_NAMESPACE

#endif // QQUICKTEXTEDIT_P_P_H
