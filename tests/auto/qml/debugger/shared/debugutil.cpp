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

#include "debugutil_p.h"

#include <QEventLoop>
#include <QTimer>

bool QQmlDebugTest::waitForSignal(QObject *receiver, const char *member, int timeout) {
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(receiver, member, &loop, SLOT(quit()));
    timer.start(timeout);
    loop.exec();
    if (!timer.isActive())
        qWarning("waitForSignal %s timed out after %d ms", member, timeout);
    return timer.isActive();
}

QQmlDebugTestClient::QQmlDebugTestClient(const QString &s, QQmlDebugConnection *c)
    : QQmlDebugClient(s, c)
{
}

QByteArray QQmlDebugTestClient::waitForResponse()
{
    lastMsg.clear();
    QQmlDebugTest::waitForSignal(this, SIGNAL(serverMessage(QByteArray)));
    if (lastMsg.isEmpty()) {
        qWarning() << "no response from server!";
        return QByteArray();
    }
    return lastMsg;
}

void QQmlDebugTestClient::stateChanged(State stat)
{
    QCOMPARE(stat, state());
    emit stateHasChanged();
}

void QQmlDebugTestClient::messageReceived(const QByteArray &ba)
{
    lastMsg = ba;
    emit serverMessage(ba);
}

QQmlDebugProcess::QQmlDebugProcess(const QString &executable, QObject *parent)
    : QObject(parent)
    , m_executable(executable)
    , m_started(false)
{
    m_process.setProcessChannelMode(QProcess::MergedChannels);
    m_timer.setSingleShot(true);
    m_timer.setInterval(5000);
    connect(&m_process, SIGNAL(readyReadStandardOutput()), this, SLOT(processAppOutput()));
    connect(&m_timer, SIGNAL(timeout()), SLOT(timeout()));
}

QQmlDebugProcess::~QQmlDebugProcess()
{
    stop();
}

QString QQmlDebugProcess::state()
{
    QString stateStr;
    switch (m_process.state()) {
    case QProcess::NotRunning: {
        stateStr = "not running";
        if (m_process.exitStatus() == QProcess::CrashExit)
            stateStr += " (crashed!)";
        else
            stateStr += ", return value" + m_process.exitCode();
        break;
    }
    case QProcess::Starting: stateStr = "starting"; break;
    case QProcess::Running: stateStr = "running"; break;
    }
    return stateStr;
}

void QQmlDebugProcess::start(const QStringList &arguments)
{
    m_mutex.lock();
    m_process.setEnvironment(m_environment);
    m_process.start(m_executable, arguments);
    if (!m_process.waitForStarted()) {
        qWarning() << "QML Debug Client: Could not launch app " << m_executable
                   << ": " << m_process.errorString();
        m_eventLoop.quit();
    } else {
        m_timer.start();
    }
    m_mutex.unlock();
}

void QQmlDebugProcess::stop()
{
    if (m_process.state() != QProcess::NotRunning) {
        m_process.kill();
        m_process.waitForFinished(5000);
    }
}

void QQmlDebugProcess::timeout()
{
    qWarning() << "Timeout while waiting for QML debugging messages "
                  "in application output. Process is in state" << m_process.state() << ".";
    m_eventLoop.quit();
}

bool QQmlDebugProcess::waitForSessionStart()
{
    if (m_process.state() != QProcess::Running) {
        qWarning() << "Could not start up " << m_executable;
        return false;
    }
    m_eventLoop.exec();

    return m_started;
}

void QQmlDebugProcess::setEnvironment(const QStringList &environment)
{
    m_environment = environment;
}

QString QQmlDebugProcess::output() const
{
    return m_output;
}

void QQmlDebugProcess::processAppOutput()
{
    m_mutex.lock();

    QString newOutput = m_process.readAll();
    m_output.append(newOutput);
    m_outputBuffer.append(newOutput);

    while (true) {
        const int nlIndex = m_outputBuffer.indexOf(QLatin1Char('\n'));
        if (nlIndex < 0) // no further complete lines
            break;
        const QString line = m_outputBuffer.left(nlIndex);
        m_outputBuffer = m_outputBuffer.right(m_outputBuffer.size() - nlIndex - 1);

        if (line.contains("QML Debugger:")) {
            if (line.contains("Waiting for connection ")) {
                m_timer.stop();
                m_started = true;
                m_eventLoop.quit();
                continue;
            }
            if (line.contains("Unable to listen")) {
                qWarning() << "App was unable to bind to port!";
                m_timer.stop();
                m_eventLoop.quit();
                continue;
            }
        }
    }
    m_mutex.unlock();
}
