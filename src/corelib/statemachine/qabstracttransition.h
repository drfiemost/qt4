/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QABSTRACTTRANSITION_H
#define QABSTRACTTRANSITION_H

#include <QtCore/qobject.h>

#include <QtCore/qlist.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_STATEMACHINE

class QEvent;
class QAbstractState;
class QState;
class QStateMachine;

#ifndef QT_NO_ANIMATION
class QAbstractAnimation;
#endif

class QAbstractTransitionPrivate;
class Q_CORE_EXPORT QAbstractTransition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QState* sourceState READ sourceState)
    Q_PROPERTY(QAbstractState* targetState READ targetState WRITE setTargetState)
    Q_PROPERTY(QList<QAbstractState*> targetStates READ targetStates WRITE setTargetStates)
public:
    QAbstractTransition(QState *sourceState = nullptr);
    ~QAbstractTransition() override;

    QState *sourceState() const;
    QAbstractState *targetState() const;
    void setTargetState(QAbstractState* target);
    QList<QAbstractState*> targetStates() const;
    void setTargetStates(const QList<QAbstractState*> &targets);

    QStateMachine *machine() const;

#ifndef QT_NO_ANIMATION
    void addAnimation(QAbstractAnimation *animation);
    void removeAnimation(QAbstractAnimation *animation);
    QList<QAbstractAnimation*> animations() const;
#endif

Q_SIGNALS:
#if !defined(Q_MOC_RUN) && !defined(qdoc)
private: // can only be emitted by QAbstractTransition
#endif
    void triggered();

protected:
    virtual bool eventTest(QEvent *event) = 0;

    virtual void onTransition(QEvent *event) = 0;

    bool event(QEvent *e) override;

protected:
    QAbstractTransition(QAbstractTransitionPrivate &dd, QState *parent);

private:
    Q_DISABLE_COPY(QAbstractTransition)
    Q_DECLARE_PRIVATE(QAbstractTransition)
};

#endif //QT_NO_STATEMACHINE

QT_END_NAMESPACE

QT_END_HEADER

#endif
