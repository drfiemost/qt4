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
#include <QtCore/qmetatype.h>

#include <QtCore/qobject_impl.h>


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
    static QString tr(const char *sourceText, const char * = nullptr, int = -1)
        { return QString::fromLatin1(sourceText); }
#ifndef QT_NO_TEXTCODEC
    static QString trUtf8(const char *sourceText, const char * = nullptr, int = -1)
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
    {
        typedef typename QtPrivate::remove_cv<typename QtPrivate::remove_pointer<T>::type>::type ObjType;
        return static_cast<T>(qt_qFindChild_helper(this, aName, ObjType::staticMetaObject));
    }

    template<typename T>
    inline QList<T> findChildren(const QString &aName = QString()) const
    {
        typedef typename QtPrivate::remove_cv<typename QtPrivate::remove_pointer<T>::type>::type ObjType;
        QList<T> list;
        qt_qFindChildren_helper(this, aName, nullptr, ObjType::staticMetaObject,
                                reinterpret_cast<QList<void *> *>(&list));
        return list;
    }

#ifndef QT_NO_REGEXP
    template<typename T>
    inline QList<T> findChildren(const QRegExp &re) const
    {
        typedef typename QtPrivate::remove_cv<typename QtPrivate::remove_pointer<T>::type>::type ObjType;
        QList<T> list;
        qt_qFindChildren_helper(this, QString(), &re, ObjType::staticMetaObject,
                                reinterpret_cast<QList<void *> *>(&list));
        return list;
    }
#endif

    inline const QObjectList &children() const { return d_ptr->children; }

    void setParent(QObject *);
    void installEventFilter(QObject *);
    void removeEventFilter(QObject *);


    static QMetaObject::Connection connect(const QObject *sender, const char *signal,
                        const QObject *receiver, const char *member, Qt::ConnectionType =
                        Qt::AutoConnection
        );

    static QMetaObject::Connection connect(const QObject *sender, const QMetaMethod &signal,
                        const QObject *receiver, const QMetaMethod &method,
                        Qt::ConnectionType type = 
                        Qt::AutoConnection
        );

    inline QMetaObject::Connection connect(const QObject *sender, const char *signal,
                        const char *member, Qt::ConnectionType type =
                        Qt::AutoConnection
        ) const;

    //Connect a signal to a pointer to qobject member function
    template <typename Func1, typename Func2>
    static inline QMetaObject::Connection connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                     const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot,
                                     Qt::ConnectionType type = Qt::AutoConnection)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;
        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value,
                          "No Q_OBJECT in the class with the signal");

        //compilation error if the arguments does not match.
        static_assert(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount),
                          "The slot requires more arguments than the signal provides.");
        static_assert((QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value),
                          "Signal and slot arguments are not compatible.");
        static_assert((QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, typename SignalType::ReturnType>::value),
                          "Return type of the slot is not compatible with the return type of the signal.");

        const int *types = 0;
        if (type == Qt::QueuedConnection || type == Qt::BlockingQueuedConnection)
            types = QtPrivate::ConnectionTypes<typename SignalType::Arguments>::types();

        return connectImpl(sender, reinterpret_cast<void **>(&signal),
                           receiver, reinterpret_cast<void **>(&slot),
                           new QSlotObject<Func2, typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                           typename SignalType::ReturnType>(slot),
                            type, types, &SignalType::Object::staticMetaObject);
    }

    //connect to a function pointer  (not a member)
    template <typename Func1, typename Func2>
    static inline typename std::enable_if<int(QtPrivate::FunctionPointer<Func2>::ArgumentCount) >= 0, QMetaObject::Connection>::type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;

        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value,
                          "No Q_OBJECT in the class with the signal");

        //compilation error if the arguments does not match.
        static_assert(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount),
                          "The slot requires more arguments than the signal provides.");
        static_assert((QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value),
                          "Signal and slot arguments are not compatible.");
        static_assert((QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, typename SignalType::ReturnType>::value),
                          "Return type of the slot is not compatible with the return type of the signal.");

        return connectImpl(sender, reinterpret_cast<void **>(&signal), sender, 0,
                           new QStaticSlotObject<Func2,
                                                 typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                                 typename SignalType::ReturnType>(slot),
                           Qt::DirectConnection, 0, &SignalType::Object::staticMetaObject);
    }

    //connect to a functor
    template <typename Func1, typename Func2>
    static inline typename std::enable_if<QtPrivate::FunctionPointer<Func2>::ArgumentCount == -1, QMetaObject::Connection>::type
            connect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal, Func2 slot)
    {
#ifndef Q_COMPILER_DECLTYPE  //Workaround the lack of decltype using another function as indirection
        return connect_functor(sender, signal, slot, &Func2::operator()); }
    template <typename Func1, typename Func2, typename Func2Operator>
    static inline QMetaObject::Connection connect_functor(const QObject *sender, Func1 signal, Func2 slot, Func2Operator) {
        typedef QtPrivate::FunctionPointer<Func2Operator> SlotType ;
#else

        typedef QtPrivate::FunctionPointer<decltype(&Func2::operator())> SlotType ;
#endif
        typedef QtPrivate::FunctionPointer<Func1> SignalType;

        static_assert(int(SignalType::ArgumentCount) >= int(SlotType::ArgumentCount),
                          "The slot requires more arguments than the signal provides.");
        static_assert((QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value),
                          "Signal and slot arguments are not compatible.");
        static_assert((QtPrivate::AreArgumentsCompatible<typename SlotType::ReturnType, typename SignalType::ReturnType>::value),
                          "Return type of the slot is not compatible with the return type of the signal.");

        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value,
                          "No Q_OBJECT in the class with the signal");

        return connectImpl(sender, reinterpret_cast<void **>(&signal), sender, 0,
                           new QFunctorSlotObject<Func2, SlotType::ArgumentCount,
                                typename QtPrivate::List_Left<typename SignalType::Arguments, SlotType::ArgumentCount>::Value,
                                typename SignalType::ReturnType>(slot),
                           Qt::DirectConnection, 0, &SignalType::Object::staticMetaObject);
    }

    static bool disconnect(const QObject *sender, const char *signal,
                           const QObject *receiver, const char *member);
    static bool disconnect(const QObject *sender, const QMetaMethod &signal,
                           const QObject *receiver, const QMetaMethod &member);
    inline bool disconnect(const char *signal = nullptr,
                           const QObject *receiver = nullptr, const char *member = nullptr)
        { return disconnect(this, signal, receiver, member); }
    inline bool disconnect(const QObject *receiver, const char *member = nullptr)
        { return disconnect(this, nullptr, receiver, member); }
    static bool disconnect(const QMetaObject::Connection &);

    template <typename Func1, typename Func2>
    static inline bool disconnect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                  const typename QtPrivate::FunctionPointer<Func2>::Object *receiver, Func2 slot)
    {
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        typedef QtPrivate::FunctionPointer<Func2> SlotType;
        static_assert(QtPrivate::HasQ_OBJECT_Macro<typename SignalType::Object>::Value,
                          "No Q_OBJECT in the class with the signal");


        //compilation error if the arguments does not match.
        static_assert((QtPrivate::CheckCompatibleArguments<typename SignalType::Arguments, typename SlotType::Arguments>::value),
                          "Signal and slot arguments are not compatible.");

        return disconnectImpl(sender, reinterpret_cast<void **>(&signal), receiver, reinterpret_cast<void **>(&slot),
                              &SignalType::Object::staticMetaObject);
    }
    template <typename Func1>
    static inline bool disconnect(const typename QtPrivate::FunctionPointer<Func1>::Object *sender, Func1 signal,
                                  const QObject *receiver, void **zero)
    {
        // This is the overload for when one wish to disconnect a signal from any slot. (slot=0)
        // Since the function template parametter cannot be deduced from '0', we use a
        // dummy void ** parametter that must be equal to 0
        Q_ASSERT(!zero);
        typedef QtPrivate::FunctionPointer<Func1> SignalType;
        return disconnectImpl(sender, reinterpret_cast<void **>(&signal), receiver, zero,
                              &SignalType::Object::staticMetaObject);
    }


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
    friend class QMetaCallEvent;
    friend class QApplication;
    friend class QApplicationPrivate;
    friend class QCoreApplication;
    friend class QCoreApplicationPrivate;
    friend class QWidget;
    friend class QThreadData;

private:
    Q_DISABLE_COPY(QObject)
    Q_PRIVATE_SLOT(d_func(), void _q_reregisterTimers(void *))

    private:
    // internal base class (interface) containing functions required to call a slot managed by a pointer to function.
    struct QSlotObjectBase {
        QAtomicInt ref;
        QSlotObjectBase() : ref(1) {}
        virtual ~QSlotObjectBase();
        virtual void call(QObject *receiver, void **a) = 0;
        virtual bool compare(void **);
    };
    // implementation of QSlotObjectBase for which the slot is a pointer to member function of a QObject
    // Args and R are the List of arguments and the returntype of the signal to which the slot is connected.
    template<typename Func, typename Args, typename R> struct QSlotObject : QSlotObjectBase
    {
        typedef QtPrivate::FunctionPointer<Func> FuncType;
        Func function;
        QSlotObject(Func f) : function(f) {};
        virtual void call(QObject *receiver, void **a) {
            FuncType::template call<Args, R>(function, static_cast<typename FuncType::Object *>(receiver), a);
        }
        virtual bool compare(void **f) {
            return *reinterpret_cast<Func *>(f) == function;
        }
    };
    // implementation of QSlotObjectBase for which the slot is a static function
    // Args and R are the List of arguments and the returntype of the signal to which the slot is connected.
    template<typename Func, typename Args, typename R> struct QStaticSlotObject : QSlotObjectBase
    {
        typedef QtPrivate::FunctionPointer<Func> FuncType;
        Func function;
        QStaticSlotObject(Func f) : function(f) {}
        virtual void call(QObject *receiver, void **a) {
            FuncType::template call<Args, R>(function, receiver, a);
        }
    };
    // implementation of QSlotObjectBase for which the slot is a functor (or lambda)
    // N is the number of arguments
    // Args and R are the List of arguments and the returntype of the signal to which the slot is connected.
    template<typename Func, int N, typename Args, typename R> struct QFunctorSlotObject : QSlotObjectBase
    {
        typedef QtPrivate::Functor<Func, N> FuncType;
        Func function;
        QFunctorSlotObject(const Func &f) : function(f) {}
        virtual void call(QObject *receiver, void **a) {
            FuncType::template call<Args, R>(function, receiver, a);
        }
    };

    static QMetaObject::Connection connectImpl(const QObject *sender, void **signal,
                                               const QObject *receiver, void **slotPtr,
                                               QSlotObjectBase *slot, Qt::ConnectionType type,
                                               const int *types, const QMetaObject *senderMetaObject);

    static bool disconnectImpl(const QObject *sender, void **signal, const QObject *receiver, void **slot,
                               const QMetaObject *senderMetaObject);
};

inline QMetaObject::Connection QObject::connect(const QObject *asender, const char *asignal,
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
    typedef typename QtPrivate::remove_cv<typename QtPrivate::remove_pointer<T>::type>::type ObjType;
    static_assert(QtPrivate::HasQ_OBJECT_Macro<ObjType>::Value,
                    "qobject_cast require the type to have a Q_OBJECT macro");
    return static_cast<T>(ObjType::staticMetaObject.cast(object));
}

template <class T>
inline T qobject_cast(const QObject *object)
{
    typedef typename QtPrivate::remove_cv<typename QtPrivate::remove_pointer<T>::type>::type ObjType;
    static_assert(QtPrivate::HasQ_OBJECT_Macro<ObjType>::Value,
                      "qobject_cast require the type to have a Q_OBJECT macro");
    return static_cast<T>(ObjType::staticMetaObject.cast(object));
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
