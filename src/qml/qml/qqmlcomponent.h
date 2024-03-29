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

#ifndef QQMLCOMPONENT_H
#define QQMLCOMPONENT_H

#include <QtQml/qqml.h>
#include <QtQml/qqmlerror.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>
#include <QtQml/qjsvalue.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QByteArray;
class QQmlEngine;
class QQmlComponent;
class QQmlIncubator;
class QQmlV8Function;
class QQmlCompiledData;
class QQmlComponentPrivate;
class QQmlComponentAttached;

class Q_QML_EXPORT QQmlComponent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QQmlComponent)

    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl url READ url CONSTANT)

public:
    Q_ENUMS(CompilationMode)
    enum CompilationMode { PreferSynchronous, Asynchronous };

    QQmlComponent(QObject *parent = 0);
    QQmlComponent(QQmlEngine *, QObject *parent=0);
    QQmlComponent(QQmlEngine *, const QString &fileName, QObject *parent = 0);
    QQmlComponent(QQmlEngine *, const QString &fileName, CompilationMode mode, QObject *parent = 0);
    QQmlComponent(QQmlEngine *, const QUrl &url, QObject *parent = 0);
    QQmlComponent(QQmlEngine *, const QUrl &url, CompilationMode mode, QObject *parent = 0);
    virtual ~QQmlComponent();

    Q_ENUMS(Status)
    enum Status { Null, Ready, Loading, Error };
    Status status() const;

    bool isNull() const;
    bool isReady() const;
    bool isError() const;
    bool isLoading() const;

    QList<QQmlError> errors() const;
    Q_INVOKABLE QString errorString() const;

    qreal progress() const;

    QUrl url() const;

    virtual QObject *create(QQmlContext *context = 0);
    virtual QObject *beginCreate(QQmlContext *);
    virtual void completeCreate();

    void create(QQmlIncubator &, QQmlContext *context = 0,
                QQmlContext *forContext = 0);

    QQmlContext *creationContext() const;

    static QQmlComponentAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void loadUrl(const QUrl &url);
    void loadUrl(const QUrl &url, CompilationMode mode);
    void setData(const QByteArray &, const QUrl &baseUrl);

Q_SIGNALS:
    void statusChanged(QQmlComponent::Status);
    void progressChanged(qreal);

protected:
    QQmlComponent(QQmlComponentPrivate &dd, QObject* parent);
    Q_INVOKABLE void createObject(QQmlV8Function *);
    Q_INVOKABLE void incubateObject(QQmlV8Function *);

private:
    QQmlComponent(QQmlEngine *, QQmlCompiledData *, int, QObject *parent);

    Q_DISABLE_COPY(QQmlComponent)
    friend class QQmlVME;
    friend class QQmlTypeData;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QQmlComponent::Status)
QML_DECLARE_TYPE(QQmlComponent)
QML_DECLARE_TYPEINFO(QQmlComponent, QML_HAS_ATTACHED_PROPERTIES)

QT_END_HEADER

#endif // QQMLCOMPONENT_H
