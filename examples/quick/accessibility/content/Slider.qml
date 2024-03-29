/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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

import QtQuick 2.0

// Minimal slider implementation
Rectangle {
    id: slider

    property alias text: buttonText.text
    Accessible.role: Accessible.Slider

    property int value: 5         // required
    property int minimumValue: 0  // optional (default INT_MIN)
    property int maximumValue: 20 // optional (default INT_MAX)
    property int stepSize: 1      // optional (default 1)

    width: 100
    height: 30
    border.color: "black"
    border.width: 1

    Rectangle {
        id: indicator
        x: 1
        y: 1
        height: parent.height - 2
        width: ((parent.width - 2) / maximumValue) * value
        color: "lightgrey"
        Behavior on width {
            NumberAnimation { duration: 50 }
        }
    }

    Text {
        id: buttonText
        text: parent.value
        anchors.centerIn: parent
        font.pixelSize: parent.height * .5
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            var pos = mouse.x / slider.width * (maximumValue - minimumValue) + minimumValue
            slider.value = pos
        }
    }

    Keys.onLeftPressed: value > minimumValue ? value = value - stepSize : minimumValue
    Keys.onRightPressed: value < maximumValue ? value = value + stepSize : maximumValue
}
