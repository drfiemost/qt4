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

#ifndef QOBJECT_H
#define QOBJECT_H

#ifndef QT_NO_QOBJECT

#include <QtCore/qobjectdefs.h>
#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#ifdef QT_INCLUDE_COMPAT
#include <QtCore/qcoreevent.h>
#endif
#include <QtCore/qscopedpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QEvent;
class QTimerEvent;
class QChildEvent;
struct QMetaObject;
class QVariant;
class QObjectPrivate;
class QObject;
class QThread;
class QWidget;
#ifndef QT_NO_REGEXP
class QRegExp;
#endif
#ifndef QT_NO_USERDATA
class QObjectUserData;
#endif

typedef QList<QObject*> QObjectList;

Q_CORE_EXPORT void qt_qFindChildren_helper(const QObject *parent, const QString &name, const QRegExp *re,
                                           const QMetaObject &mo, QList<void *> *list);
Q_CORE_EXPORT QObject *qt_qFindChild_helper(const QObject *parent, const QString &name, const QMetaObject &mo);

class
#if defined(__INTEL_COMPILER) && defined(Q_OS_WIN)
Q_CORE_EXPORT
#endif
QObjectData {
public:
    virtual ~QObjectData() = 0;
    QObject *q_ptr;
    QObject *parent;
    QObjectList children;

    uint isWidget : 1;
    uint pendTimer : 1;
    uint blockSig : 1;
    uint wasDeleted : 1;
    uint sendChildEvents : 1;
    uint receiveChildEvents : 1;
    uint inThreadChangeEvent : 1;
    uint hasGuards : 1; //true iff there is one or more QPointer attached to this object
    uint deleteLaterCalled : 1;
    uint unused : 23;
    int postedEvents;
    QMetaObject *metaObject; // assert dynamic
};


class Q_CORE_EXPORT QObject
{
    Q_OBJECT
    Q_PROPERTY(QString objectName READ objectName WRITE setObjectName)
    Q_DECLARE_PRIVATE(QObject)

public:
    Q_INVOKABLE explicit QObject(QObject *parent=nullptr);
    virtual ~QObject();

    virtual bool event(QEvent *);
    virtual bool eventFilter(QObject *, QEvent *);

#ifdef qdoc
    static QString tr(const char *sourceText, const char *comment = nullptr, int n = -1);
    static QString trUtf8(const char *sourceText, const char *comment = nullptr, int n = -1);
    virtual const QMetaObject *metaObject() const;
    static const QMetaObject staticMetaObject;
#endif
#ifdef QT_NO_TRANSLATION
    static QString tr(const char *sourceText, const char *, int)
        { return QString::fromLatin1(sourceText); }
    static QString tr(const char *sourceText, const char * = nullptr)
        { return QString::fromLatin1(sourceText); }
#ifndef QT_NO_TEXTCODEC
    static QString trUtf8(const char *sourceText, const char *, int)
        { return QString::fromUtf8(sourceText); }
    static QString trUtf8(const char *sourceText, const char * = nullptr)
        { return QString::fromUtf8(sourceText); }
#endif
#endif //QT_NO_TRANSLATION

    QString objectName() const;
    void setObjectName(const QString &name);

    inline bool isWidgetType() const { return d_ptr->isWidget; }

    inline bool signalsBlocked() const { return d_ptr->blockSig; }
    bool blockSignals(bool b);

    QThread *thread() const;
    void moveToThread(QThread *thread);

    int startTimer(int interval, Qt::TimerType timerType = Qt::CoarseTimer);
    void killTimer(int id);

    template<typename T>
    inline T findChild(const QString &aName = QString()) const
    { return static_cast<T>(qt_qFindChild_helper(this, aName, reinterpret_cast<T>(0)->staticMetaObject)); }

    template<typename T>
    inline QList<T> findChildren(const QString &aName = QString()) const
    {
        QList<T> list;
        union {
            QList<T> *typedList;
            QList<void *> *voidList;
        } u;
        u.typedList = &list;
        qt_qFindChildren_helper(this, aName, nullptr, reinterpret_cast<T>(0)->staticMetaObject, u.voidList);
        return list;
    }

#ifndef QT_NO_REGEXP
    template<typename T>
    inline QList<T> findChildren(const QRegExp &re) const
    {
        QList<T> list;
        union {
            QList<T> *typedList;
            QList<void *> *voidList;
        } u;
        u.typedList = &list;
        qt_qFindChildren_helper(this, QString(), &re, reinterpret_cast<T>(0)->staticMetaObject, u.voidList);
        return list;
    }
#endif

    inline const QObjectList &children() const { return d_ptr->children; }

    void setParent(QObject *);
    void installEventFilter(QObject *);
    void removeEventFilter(QObject *);


    static bool connect(const QObject *sender, const char *signal,
                        const QObject *receiver, const char *member, Qt::ConnectionType =
                        Qt::AutoConnection
        );
        
    static bool connect(const QObject *sender, const QMetaMethod &signal,
                        const QObject *receiver, const QMetaMethod &method,
                        Qt::ConnectionType type = 
                        Qt::AutoConnection
        );

    inline bool connect(const QObject *sender, const char *signal,
                        const char *member, Qt::ConnectionType type =
                        Qt::AutoConnection
        ) const;

    static bool disconnect(const QObject *sender, const char *signal,
                           const QObject *receiver, const char *member);
    static bool disconnect(const QObject *sender, const QMetaMethod &signal,
                           const QObject *receiver, const QMetaMethod &member);
    inline bool disconnect(const char *signal = nullptr,
                           const QObject *receiver = nullptr, const char *member = nullptr)
        { return disconnect(this, signal, receiver, member); }
    inline bool disconnect(const QObject *receiver, const char *member = nullptr)
        { return disconnect(this, nullptr, receiver, member); }

    void dumpObjectTree();
    void dumpObjectInfo();

#ifndef QT_NO_PROPERTIES
    bool setProperty(const char *name, const QVariant &value);
    QVariant property(const char *name) const;
    QList<QByteArray> dynamicPropertyNames() const;
#endif // QT_NO_PROPERTIES

#ifndef QT_NO_USERDATA
    static uint registerUserData();
    void setUserData(uint id, QObjectUserData* data);
    QObjectUserData* userData(uint id) const;
#endif // QT_NO_USERDATA

Q_SIGNALS:
    void destroyed(QObject * = nullptr);

public:
    inline QObject *parent() const { return d_ptr->parent; }

    inline bool inherits(const char *classname) const
        { return const_cast<QObject *>(this)->qt_metacast(classname) != nullptr; }

public Q_SLOTS:
    void deleteLater();

protected:
    QObject *sender() const;
    int senderSignalIndex() const;
    int receivers(const char* signal) const;

    virtual void timerEvent(QTimerEvent *);
    virtual void childEvent(QChildEvent *);
    virtual void customEvent(QEvent *);

    virtual void connectNotify(const char *signal);
    virtual void disconnectNotify(const char *signal);

protected:
    QObject(QObjectPrivate &dd, QObject *parent = nullptr);

protected:
    QScopedPointer<QObjectData> d_ptr;

    static const QMetaObject staticQtMetaObject;

    friend struct QMetaObject;
    friend class QApplication;
    friend class QApplicationPrivate;
    friend class QCoreApplication;
    friend class QCoreApplicationPrivate;
    friend class QWidget;
    friend class QThreadData;

private:
    Q_DISABLE_COPY(QObject)
    Q_PRIVATE_SLOT(d_func(), void _q_reregisterTimers(void *))
};

inline bool QObject::connect(const QObject *asender, const char *asignal,
                             const char *amember, Qt::ConnectionType atype) const
{ return connect(asender, asignal, this, amember, atype); }

#ifndef QT_NO_USERDATA
class Q_CORE_EXPORT QObjectUserData {
public:
    virtual ~QObjectUserData();
};
#endif

#ifdef qdoc
T qFindChild(const QObject *o, const QString &name = QString());
QList<T> qFindChildren(const QObject *oobj, const QString &name = QString());
QList<T> qFindChildren(const QObject *o, const QRegExp &re);
#endif
#ifdef QT_DEPRECATED
template<typename T>
inline QT_DEPRECATED T qFindChild(const QObject *o, const QString &name = QString())
{ return o->findChild<T>(name); }

template<typename T>
inline QT_DEPRECATED QList<T> qFindChildren(const QObject *o, const QString &name = QString())
{
    return o->findChildren<T>(name);
}

#ifndef QT_NO_REGEXP
template<typename T>
inline QT_DEPRECATED QList<T> qFindChildren(const QObject *o, const QRegExp &re)
{
    return o->findChildren<T>(re);
}
#endif

#endif //QT_DEPRECATED

template <class T>
inline T qobject_cast(QObject *object)
{
#if !defined(QT_NO_QOBJECT_CHECK)
    reinterpret_cast<T>(object)->qt_check_for_QOBJECT_macro(*reinterpret_cast<T>(object));
#endif
    return static_cast<T>(reinterpret_cast<T>(object)->staticMetaObject.cast(object));
}

template <class T>
inline T qobject_cast(const QObject *object)
{
#if !defined(QT_NO_QOBJECT_CHECK)
    reinterpret_cast<T>(object)->qt_check_for_QOBJECT_macro(*reinterpret_cast<T>(const_cast<QObject *>(object)));
#endif
    return static_cast<T>(reinterpret_cast<T>(object)->staticMetaObject.cast(object));
}


template <class T> inline const char * qobject_interface_iid()
{ return nullptr; }

#ifndef Q_MOC_RUN
#  define Q_DECLARE_INTERFACE(IFace, IId) \
    template <> inline const char *qobject_interface_iid<IFace *>() \
    { return IId; } \
    template <> inline IFace *qobject_cast<IFace *>(QObject *object) \
    { return reinterpret_cast<IFace *>((object ? object->qt_metacast(IId) : 0)); } \
    template <> inline IFace *qobject_cast<IFace *>(const QObject *object) \
    { return reinterpret_cast<IFace *>((object ? const_cast<QObject *>(object)->qt_metacast(IId) : 0)); }
#endif // Q_MOC_RUN

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QObject *);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif

#endif // QOBJECT_H
