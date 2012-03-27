/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtQml module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qqmlexpression.h"
#include "qqmlexpression_p.h"

#include "qqmlengine_p.h"
#include "qqmlcontext_p.h"
#include "qqmlrewrite_p.h"
#include "qqmlscriptstring_p.h"
#include "qqmlcompiler_p.h"

#include <QtCore/qdebug.h>

QT_BEGIN_NAMESPACE

static QQmlJavaScriptExpression::VTable QQmlExpressionPrivate_jsvtable = {
    QQmlExpressionPrivate::expressionIdentifier,
    QQmlExpressionPrivate::expressionChanged
};

QQmlExpressionPrivate::QQmlExpressionPrivate()
: QQmlJavaScriptExpression(&QQmlExpressionPrivate_jsvtable),
  expressionFunctionValid(true), expressionFunctionRewritten(false),
  extractExpressionFromFunction(false), line(-1), dataRef(0)
{
}

QQmlExpressionPrivate::~QQmlExpressionPrivate()
{
    qPersistentDispose(v8qmlscope);
    qPersistentDispose(v8function);
    if (dataRef) dataRef->release();
    dataRef = 0;
}

void QQmlExpressionPrivate::init(QQmlContextData *ctxt, const QString &expr, QObject *me)
{
    expression = expr;

    QQmlAbstractExpression::setContext(ctxt);
    setScopeObject(me);
    expressionFunctionValid = false;
    expressionFunctionRewritten = false;
}

void QQmlExpressionPrivate::init(QQmlContextData *ctxt, const QString &expr,
                                 bool isRewritten, QObject *me, const QString &srcUrl,
                                 int lineNumber, int columnNumber)
{
    url = srcUrl;
    line = lineNumber;
    column = columnNumber;

    expression = expr;

    expressionFunctionValid = false;
    expressionFunctionRewritten = isRewritten;

    QQmlAbstractExpression::setContext(ctxt);
    setScopeObject(me);
}

void QQmlExpressionPrivate::init(QQmlContextData *ctxt, const QByteArray &expr,
                                 bool isRewritten, QObject *me, const QString &srcUrl,
                                 int lineNumber, int columnNumber)
{
    url = srcUrl;
    line = lineNumber;
    column = columnNumber;

    if (isRewritten) {
        expressionFunctionValid = true;
        expressionFunctionRewritten = true;
        v8function = evalFunction(ctxt, me, expr.constData(), expr.length(),
                                  srcUrl, lineNumber, &v8qmlscope);
        setUseSharedContext(false);

        expressionUtf8 = expr;
    } else {
        expression = QString::fromUtf8(expr);

        expressionFunctionValid = false;
        expressionFunctionRewritten = isRewritten;
    }

    QQmlAbstractExpression::setContext(ctxt);
    setScopeObject(me);
}

QQmlExpression *
QQmlExpressionPrivate::create(QQmlContextData *ctxt, QObject *object,
                              const QString &expr, bool isRewritten,
                              const QString &url, int lineNumber, int columnNumber)
{
    return new QQmlExpression(ctxt, object, expr, isRewritten, url, lineNumber, columnNumber,
                              *new QQmlExpressionPrivate);
}

/*!
    \class QQmlExpression
    \since 5.0
    \inmodule QtQml
    \brief The QQmlExpression class evaluates JavaScript in a QML context.

    For example, given a file \c main.qml like this:

    \qml
    import QtQuick 2.0

    Item {
        width: 200; height: 200
    }
    \endqml

    The following code evaluates a JavaScript expression in the context of the
    above QML:

    \code
    QQmlEngine *engine = new QQmlEngine;
    QQmlComponent component(engine, QUrl::fromLocalFile("main.qml"));

    QObject *myObject = component.create();
    QQmlExpression *expr = new QQmlExpression(engine->rootContext(), myObject, "width * 2");
    int result = expr->evaluate().toInt();  // result = 400
    \endcode

    Note that the QtQuick 1 version is called QDeclarativeExpression.
*/

/*!
    Create an invalid QQmlExpression.

    As the expression will not have an associated QQmlContext, this will be a
    null expression object and its value will always be an invalid QVariant.
 */
QQmlExpression::QQmlExpression()
: QObject(*new QQmlExpressionPrivate, 0)
{
}

/*!  \internal */
QQmlExpression::QQmlExpression(QQmlContextData *ctxt, 
                                               QObject *object, const QString &expr, bool isRewritten,
                                               const QString &url, int lineNumber, int columnNumber,
                                               QQmlExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QQmlExpression);
    d->init(ctxt, expr, isRewritten, object, url, lineNumber, columnNumber);
}

/*!  \internal */
QQmlExpression::QQmlExpression(QQmlContextData *ctxt,
                                               QObject *object, const QByteArray &expr,
                                               bool isRewritten,
                                               const QString &url, int lineNumber, int columnNumber,
                                               QQmlExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QQmlExpression);
    d->init(ctxt, expr, isRewritten, object, url, lineNumber, columnNumber);
}

/*!
    Create a QQmlExpression object that is a child of \a parent.

    The \script provides the expression to be evaluated, the context to evaluate it in,
    and the scope object to evaluate it with.

    This constructor is functionally equivalent to the following, but in most cases
    is more efficient.
    \code
    QQmlExpression expression(script.context(), script.scopeObject(), script.script(), parent);
    \endcode

    \sa QQmlScriptString
*/
QQmlExpression::QQmlExpression(const QQmlScriptString &script, QObject *parent)
: QObject(*new QQmlExpressionPrivate, parent)
{
    Q_D(QQmlExpression);
    bool defaultConstruction = false;

    int id = script.d.data()->bindingId;
    if (id < 0) {
        defaultConstruction = true;
    } else {
        QQmlContextData *ctxtdata = QQmlContextData::get(script.context());

        QQmlEnginePrivate *engine = QQmlEnginePrivate::get(script.context()->engine());
        QQmlCompiledData *cdata = 0;
        QQmlTypeData *typeData = 0;
        if (engine && ctxtdata && !ctxtdata->url.isEmpty()) {
            typeData = engine->typeLoader.get(ctxtdata->url);
            cdata = typeData->compiledData();
        }

        if (cdata)
            d->init(ctxtdata, cdata->primitives.at(id), true, script.scopeObject(),
                    cdata->name, script.d.data()->lineNumber, script.d.data()->columnNumber);
        else
           defaultConstruction = true;

        if (cdata)
            cdata->release();
        if (typeData)
            typeData->release();
    }

    if (defaultConstruction)
        d->init(QQmlContextData::get(script.context()), script.script(), script.scopeObject());
}

/*!
    Create a QQmlExpression object that is a child of \a parent.

    The \a expression JavaScript will be executed in the \a ctxt QQmlContext.
    If specified, the \a scope object's properties will also be in scope during
    the expression's execution.
*/
QQmlExpression::QQmlExpression(QQmlContext *ctxt,
                                               QObject *scope,
                                               const QString &expression,
                                               QObject *parent)
: QObject(*new QQmlExpressionPrivate, parent)
{
    Q_D(QQmlExpression);
    d->init(QQmlContextData::get(ctxt), expression, scope);
}

/*! 
    \internal
*/
QQmlExpression::QQmlExpression(QQmlContextData *ctxt, QObject *scope,
                                               const QString &expression)
: QObject(*new QQmlExpressionPrivate, 0)
{
    Q_D(QQmlExpression);
    d->init(ctxt, expression, scope);
}

/*!  \internal */
QQmlExpression::QQmlExpression(QQmlContextData *ctxt, QObject *scope,
                                               const QString &expression, QQmlExpressionPrivate &dd)
: QObject(dd, 0)
{
    Q_D(QQmlExpression);
    d->init(ctxt, expression, scope);
}

/*!
    Destroy the QQmlExpression instance.
*/
QQmlExpression::~QQmlExpression()
{
}

/*!
    Returns the QQmlEngine this expression is associated with, or 0 if there
    is no association or the QQmlEngine has been destroyed.
*/
QQmlEngine *QQmlExpression::engine() const
{
    Q_D(const QQmlExpression);
    return d->context()?d->context()->engine:0;
}

/*!
    Returns the QQmlContext this expression is associated with, or 0 if there
    is no association or the QQmlContext has been destroyed.
*/
QQmlContext *QQmlExpression::context() const
{
    Q_D(const QQmlExpression);
    QQmlContextData *data = d->context();
    return data?data->asQQmlContext():0;
}

/*!
    Returns the expression string.
*/
QString QQmlExpression::expression() const
{
    Q_D(const QQmlExpression);
    if (d->extractExpressionFromFunction && context()->engine()) {
        QV8Engine *v8engine = QQmlEnginePrivate::getV8Engine(context()->engine());
        v8::HandleScope handle_scope;
        v8::Context::Scope scope(v8engine->context());

        return v8engine->toString(v8::Handle<v8::Value>(d->v8function));
    } else if (!d->expressionUtf8.isEmpty()) {
        return QString::fromUtf8(d->expressionUtf8);
    } else {
        return d->expression;
    }
}

/*!
    Set the expression to \a expression.
*/
void QQmlExpression::setExpression(const QString &expression)
{
    Q_D(QQmlExpression);

    d->resetNotifyOnValueChanged();
    d->expression = expression;
    d->expressionUtf8.clear();
    d->expressionFunctionValid = false;
    d->expressionFunctionRewritten = false;
    qPersistentDispose(d->v8function);
    qPersistentDispose(d->v8qmlscope);
}

// Must be called with a valid handle scope
v8::Local<v8::Value> QQmlExpressionPrivate::v8value(bool *isUndefined)
{
    if (!expressionFunctionValid) {
        bool ok = true;

        QQmlRewrite::RewriteBinding rewriteBinding;
        rewriteBinding.setName(name);
        QString code;
        if (expressionFunctionRewritten)
            code = expression;
        else
            code = rewriteBinding(expression, &ok);

        if (ok) v8function = evalFunction(context(), scopeObject(), code, url, line, &v8qmlscope);
        setUseSharedContext(false);
        expressionFunctionValid = true;
    }

    return evaluate(context(), v8function, isUndefined);
}

QVariant QQmlExpressionPrivate::value(bool *isUndefined)
{
    Q_Q(QQmlExpression);

    if (!context() || !context()->isValid()) {
        qWarning("QQmlExpression: Attempted to evaluate an expression in an invalid context");
        return QVariant();
    }

    QQmlEnginePrivate *ep = QQmlEnginePrivate::get(q->engine());
    QVariant rv;

    ep->referenceScarceResources(); // "hold" scarce resources in memory during evaluation.

    {
        v8::HandleScope handle_scope;
        v8::Context::Scope context_scope(ep->v8engine()->context());
        v8::Local<v8::Value> result = v8value(isUndefined);
        rv = ep->v8engine()->toVariant(result, qMetaTypeId<QList<QObject*> >());
    }

    ep->dereferenceScarceResources(); // "release" scarce resources if top-level expression evaluation is complete.

    return rv;
}

/*!
    Evaulates the expression, returning the result of the evaluation,
    or an invalid QVariant if the expression is invalid or has an error.

    \a valueIsUndefined is set to true if the expression resulted in an
    undefined value.

    \sa hasError(), error()
*/
QVariant QQmlExpression::evaluate(bool *valueIsUndefined)
{
    Q_D(QQmlExpression);
    return d->value(valueIsUndefined);
}

/*!
Returns true if the valueChanged() signal is emitted when the expression's evaluated
value changes.
*/
bool QQmlExpression::notifyOnValueChanged() const
{
    Q_D(const QQmlExpression);
    return d->notifyOnValueChanged();
}

/*!
  Sets whether the valueChanged() signal is emitted when the
  expression's evaluated value changes.

  If \a notifyOnChange is true, the QQmlExpression will
  monitor properties involved in the expression's evaluation, and emit
  QQmlExpression::valueChanged() if they have changed.  This
  allows an application to ensure that any value associated with the
  result of the expression remains up to date.

  If \a notifyOnChange is false (default), the QQmlExpression
  will not montitor properties involved in the expression's
  evaluation, and QQmlExpression::valueChanged() will never be
  emitted.  This is more efficient if an application wants a "one off"
  evaluation of the expression.
*/
void QQmlExpression::setNotifyOnValueChanged(bool notifyOnChange)
{
    Q_D(QQmlExpression);
    d->setNotifyOnValueChanged(notifyOnChange);
}

/*!
    Returns the source file URL for this expression.  The source location must
    have been previously set by calling setSourceLocation().
*/
QString QQmlExpression::sourceFile() const
{
    Q_D(const QQmlExpression);
    return d->url;
}

/*!
    Returns the source file line number for this expression.  The source location 
    must have been previously set by calling setSourceLocation().
*/
int QQmlExpression::lineNumber() const
{
    Q_D(const QQmlExpression);
    return d->line;
}

/*!
    Returns the source file column number for this expression.  The source location
    must have been previously set by calling setSourceLocation().
*/
int QQmlExpression::columnNumber() const
{
    Q_D(const QQmlExpression);
    return d->column;
}

/*!
    Set the location of this expression to \a line of \a url. This information
    is used by the script engine.
*/
void QQmlExpression::setSourceLocation(const QString &url, int line, int column)
{
    Q_D(QQmlExpression);
    d->url = url;
    d->line = line;
    d->column = column;
}

/*!
    Returns the expression's scope object, if provided, otherwise 0.

    In addition to data provided by the expression's QQmlContext, the scope
    object's properties are also in scope during the expression's evaluation.
*/
QObject *QQmlExpression::scopeObject() const
{
    Q_D(const QQmlExpression);
    return d->scopeObject();
}

/*!
    Returns true if the last call to evaluate() resulted in an error,
    otherwise false.
    
    \sa error(), clearError()
*/
bool QQmlExpression::hasError() const
{
    Q_D(const QQmlExpression);
    return d->hasError();
}

/*!
    Clear any expression errors.  Calls to hasError() following this will
    return false.

    \sa hasError(), error()
*/
void QQmlExpression::clearError()
{
    Q_D(QQmlExpression);
    d->clearError();
}

/*!
    Return any error from the last call to evaluate().  If there was no error,
    this returns an invalid QQmlError instance.

    \sa hasError(), clearError()
*/

QQmlError QQmlExpression::error() const
{
    Q_D(const QQmlExpression);
    return d->error();
}

/*!
    \fn void QQmlExpression::valueChanged()

    Emitted each time the expression value changes from the last time it was
    evaluated.  The expression must have been evaluated at least once (by
    calling QQmlExpression::evaluate()) before this signal will be emitted.
*/

void QQmlExpressionPrivate::expressionChanged(QQmlJavaScriptExpression *e)
{
    QQmlExpressionPrivate *This = static_cast<QQmlExpressionPrivate *>(e);
    This->expressionChanged();
}

void QQmlExpressionPrivate::expressionChanged()
{
    Q_Q(QQmlExpression);
    emit q->valueChanged();
}

QString QQmlExpressionPrivate::expressionIdentifier(QQmlJavaScriptExpression *e)
{
    QQmlExpressionPrivate *This = static_cast<QQmlExpressionPrivate *>(e);
    return QLatin1String("\"") + This->expression + QLatin1String("\"");
}

QT_END_NAMESPACE

#include <moc_qqmlexpression.cpp>
