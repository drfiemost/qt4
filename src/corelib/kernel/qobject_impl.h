/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q_QDOC

#ifndef QOBJECT_H
#error Do not include qobject_impl.h directly
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

namespace QtPrivate {
    template <typename T> struct RemoveRef { typedef T Type; };
    template <typename T> struct RemoveRef<const T&> { typedef T Type; };
    template <typename T> struct RemoveRef<T&> { typedef T Type; };
    template <typename T> struct RemoveConstRef { typedef T Type; };
    template <typename T> struct RemoveConstRef<const T&> { typedef T Type; };

    /*
       The following List classes are used to help to handle the list of arguments.
       It follow the same principles as the lisp lists.
       List_Left<L,N> take a list and a number as a parametter and returns (via the Value typedef,
       the list composed of the first N element of the list
     */
#ifndef Q_COMPILER_VARIADIC_TEMPLATES
    template <typename Head, typename Tail> struct List { typedef Head Car; typedef Tail Cdr; };
    template <typename L, int N> struct List_Left { typedef List<typename L::Car, typename List_Left<typename L::Cdr, N - 1>::Value > Value; };
    template <typename L> struct List_Left<L,0> { typedef void Value; };
#else
    // With variadic template, lists are represented using a variadic template argument instead of the lisp way
    template <typename...> struct List {};
    template <typename Head, typename... Tail> struct List<Head, Tail...> { typedef Head Car; typedef List<Tail...> Cdr; };
    template <typename, typename> struct List_Append;
    template <typename... L1, typename...L2> struct List_Append<List<L1...>, List<L2...>> { typedef List<L1..., L2...> Value; };
    template <typename L, int N> struct List_Left {
        typedef typename List_Append<List<typename L::Car>,typename List_Left<typename L::Cdr, N - 1>::Value>::Value Value;
    };
    template <typename L> struct List_Left<L, 0> { typedef List<> Value; };
#endif
    // List_Select<L,N> returns (via typedef Value) the Nth element of the list L
    template <typename L, int N> struct List_Select { typedef typename List_Select<typename L::Cdr, N - 1>::Value Value; };
    template <typename L> struct List_Select<L,0> { typedef typename L::Car Value; };

    /*
       trick to set the return value of a slot that works even if the signal or the slot returns void
       to be used like     function(), ApplyReturnValue<ReturnType>(&return_value)
       if function() returns a value, the operator,(T, ApplyReturnValue<ReturnType>) is called, but if it
       returns void, the builtin one is used without an error.
    */
    template <typename T>
    struct ApplyReturnValue {
        void *data;
        ApplyReturnValue(void *data) : data(data) {}
    };
    template<typename T, typename U>
    void operator,(const T &value, const ApplyReturnValue<U> &container) {
        *reinterpret_cast<U*>(container.data) = value;
    }
#ifdef Q_COMPILER_RVALUE_REFS
    template<typename T, typename U>
    void operator,(T &&value, const ApplyReturnValue<U> &container) {
        *reinterpret_cast<U*>(container.data) = value;
    }
#endif
    template<typename T>
    void operator,(T, const ApplyReturnValue<void> &) {}


    /*
      The FunctionPointer<Func> struct is a type trait for function pointer.
        - ArgumentCount  is the number of argument, or -1 if it is unknown
        - the Object typedef is the Object of a pointer to member function
        - the Arguments typedef is the list of argument (in a QtPrivate::List)
        - the Function typedef is an alias to the template parametter Func
        - the call<Args, R>(f,o,args) method is used to call that slot
            Args is the list of argument of the signal
            R is the return type of the signal
            f is the function pointer
            o is the receiver object
            and args is the array of pointer to arguments, as used in qt_metacall

       The Functor<Func,N> struct is the helper to call a functor of N argument.
       its call function is the same as the FunctionPointer::call function.
     */
#ifndef Q_COMPILER_VARIADIC_TEMPLATES
    template<typename Func> struct FunctionPointer { enum {ArgumentCount = -1}; };
    template<class Obj, typename Ret> struct FunctionPointer<Ret (Obj::*) ()>
    {
        typedef Obj Object;
        typedef void Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) ();
        enum {ArgumentCount = 0};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) { (o->*f)(), ApplyReturnValue<R>(arg[0]); }
    };
    template<class Obj, typename Ret, typename Arg1> struct FunctionPointer<Ret (Obj::*) (Arg1)>
    {
        typedef Obj Object;
        typedef List<Arg1, void> Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1);
        enum {ArgumentCount = 1};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<typename Args::Car>::Type *>(arg[1]))), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2> struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, void> >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2);
        enum {ArgumentCount = 2};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<class Obj, typename Ret, typename Arg1, typename Arg2, typename Arg3> struct FunctionPointer<Ret (Obj::*) (Arg1, Arg2, Arg3)>
    {
        typedef Obj Object;
        typedef List<Arg1, List<Arg2, List<Arg3, void> > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Arg1, Arg2, Arg3);
        enum {ArgumentCount = 3};
        template <typename Args, typename R>
        static void call(Function f, Obj *o, void **arg) {
            (o->*f)( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3])), ApplyReturnValue<R>(arg[0]);
        }
    };

    template<typename Ret> struct FunctionPointer<Ret (*) ()>
    {
        typedef void Arguments;
        typedef Ret (*Function) ();
        typedef Ret ReturnType;
        enum {ArgumentCount = 0};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) { f(), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Ret, typename Arg1> struct FunctionPointer<Ret (*) (Arg1)>
    {
        typedef List<Arg1, void> Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1);
        enum {ArgumentCount = 1};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg)
        { f(*reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1])), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Ret, typename Arg1, typename Arg2> struct FunctionPointer<Ret (*) (Arg1, Arg2)>
    {
        typedef List<Arg1, List<Arg2, void> > Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2);
        enum {ArgumentCount = 2};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(*reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
              *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2])), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Ret, typename Arg1, typename Arg2, typename Arg3> struct FunctionPointer<Ret (*) (Arg1, Arg2, Arg3)>
    {
        typedef List<Arg1, List<Arg2, List<Arg3, void> > >  Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Arg1, Arg2, Arg3);
        enum {ArgumentCount = 3};
        template <typename Args, typename R>
        static void call(Function f, void *, void **arg) {
            f(       *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
                     *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[3])), ApplyReturnValue<R>(arg[0]);
        }
    };

    template<typename F, int N> struct Functor;
    template<typename Function> struct Functor<Function, 0>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) { f(), ApplyReturnValue<R>(arg[0]); }
    };
    template<typename Function> struct Functor<Function, 1>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f(*reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 2>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2])), ApplyReturnValue<R>(arg[0]);
        }
    };
    template<typename Function> struct Functor<Function, 3>
    {
        template <typename Args, typename R>
        static void call(Function &f, void *, void **arg) {
            f( *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 0>::Value>::Type *>(arg[1]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 1>::Value>::Type *>(arg[2]),
               *reinterpret_cast<typename RemoveRef<typename List_Select<Args, 2>::Value>::Type *>(arg[4])), ApplyReturnValue<R>(arg[0]);
        }
    };
#else
    template <int...> struct IndexesList {};
    template <typename IndexList, int Right> struct IndexesAppend;
    template <int... Left, int Right> struct IndexesAppend<IndexesList<Left...>, Right>
    { typedef IndexesList<Left..., Right> Value; };
    template <int N> struct Indexes
    { typedef typename IndexesAppend<typename Indexes<N - 1>::Value, N - 1>::Value Value; };
    template <> struct Indexes<0> { typedef IndexesList<> Value; };
    template<typename Func> struct FunctionPointer { enum {ArgumentCount = -1}; };

    template <typename, typename, typename, typename> struct FunctorCall;
    template <int... I, typename... SignalArgs, typename R, typename Function>
    struct FunctorCall<IndexesList<I...>, List<SignalArgs...>, R, Function> {
        static void call(Function f, void **arg) {
            f((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[I+1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
    template <int... I, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<I...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...)> {
        static void call(SlotRet (Obj::*f)(SlotArgs...), Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[I+1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };

    template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...)>
    {
        typedef Obj Object;
        typedef List<Args...>  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Args...);
        enum {ArgumentCount = sizeof...(Args)};
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };

    template<typename Ret, typename... Args> struct FunctionPointer<Ret (*) (Args...)>
    {
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Args...);
        enum {ArgumentCount = sizeof...(Args)};
        template <typename SignalArgs, typename R>
        static void call(Function f, void *, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };

    template<typename Function, int N> struct Functor
    {
        template <typename SignalArgs, typename R>
        static void call(Function &f, void *, void **arg) {
            FunctorCall<typename Indexes<N>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };
#endif

    /*
       Logic that check if the arguments of the slot matches the argument of the signal.
       To be used like this:
       CheckCompatibleArguments<FunctionPointer<Signal>::Arguments, FunctionPointer<Slot>::Arguments>::IncompatibleSignalSlotArguments
       The IncompatibleSignalSlotArguments type do not exist if the argument are incompatible and can
       then produce error message.
    */
    template<typename T, bool B> struct CheckCompatibleArgumentsHelper {};
    template<typename T> struct CheckCompatibleArgumentsHelper<T, true> : T {};
    template<typename A1, typename A2> struct AreArgumentsCompatible {
        static int test(A2);
        static char test(...);
        static A2 dummy();
        enum { value = sizeof(test(dummy())) == sizeof(int) };
    };
    template<typename A1, typename A2> struct AreArgumentsCompatible<A1, A2&> { enum { value = false }; };
    template<typename A> struct AreArgumentsCompatible<A&, A&> { enum { value = true }; };

#ifndef Q_COMPILER_VARIADIC_TEMPLATES
    template <typename List1, typename List2> struct CheckCompatibleArguments{};
    template <> struct CheckCompatibleArguments<void, void> { typedef bool IncompatibleSignalSlotArguments; };
    template <typename List1> struct CheckCompatibleArguments<List1, void> { typedef bool IncompatibleSignalSlotArguments; };
    template <typename Arg1, typename Arg2, typename Tail1, typename Tail2> struct CheckCompatibleArguments<List<Arg1, Tail1>, List<Arg2, Tail2> >
        : CheckCompatibleArgumentsHelper<CheckCompatibleArguments<Tail1, Tail2>, AreArgumentsCompatible<
            typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value > {};
#else
    template <typename List1, typename List2> struct CheckCompatibleArguments{};
    template <> struct CheckCompatibleArguments<List<>, List<>> { typedef bool IncompatibleSignalSlotArguments; };
    template <typename List1> struct CheckCompatibleArguments<List1, List<>> { typedef bool IncompatibleSignalSlotArguments; };
    template <typename Arg1, typename Arg2, typename... Tail1, typename... Tail2>
    struct CheckCompatibleArguments<List<Arg1, Tail1...>, List<Arg2, Tail2...>>
        : CheckCompatibleArgumentsHelper<CheckCompatibleArguments<List<Tail1...>, List<Tail2...>>, AreArgumentsCompatible<
            typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value > {};

#endif

    /*
        Logic to statically generate the array of qMetaTypeId
        ConnectionTypes<FunctionPointer<Signal>::Arguments>::types() returns an array
        of int that is suitable for the types arguments of the connection functions.

        The array only exist of all the types are declared as a metatype
        (detected using the TypesAreDeclaredMetaType helper struct)
        If one of the type is not declared, the function return 0 and the signal
        cannot be used in queued connection.
    */
#ifndef Q_COMPILER_VARIADIC_TEMPLATES
    template <typename ArgList> struct TypesAreDeclaredMetaType { enum { Value = false }; };
    template <> struct TypesAreDeclaredMetaType<void> { enum { Value = true }; };
    template <typename Arg, typename Tail> struct TypesAreDeclaredMetaType<List<Arg, Tail> > { enum { Value = QMetaTypeId2<Arg>::Defined && TypesAreDeclaredMetaType<Tail>::Value }; };

    template <typename ArgList, bool Declared = TypesAreDeclaredMetaType<ArgList>::Value > struct ConnectionTypes
    { static const int *types() { return 0; } };
    template <> struct ConnectionTypes<void, true>
    { static const int *types() { static const int t[1] = { 0 }; return t; } };
    template <typename Arg1> struct ConnectionTypes<List<Arg1, void>, true>
    { static const int *types() { static const int t[2] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2> struct ConnectionTypes<List<Arg1, List<Arg2, void> >, true>
    { static const int *types() { static const int t[3] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(), 0 }; return t; } };
    template <typename Arg1, typename Arg2, typename Arg3> struct ConnectionTypes<List<Arg1, List<Arg2,  List<Arg3, void> > >, true>
    { static const int *types() { static const int t[4] = { QtPrivate::QMetaTypeIdHelper<Arg1>::qt_metatype_id(), QtPrivate::QMetaTypeIdHelper<Arg2>::qt_metatype_id(),
                                                            QtPrivate::QMetaTypeIdHelper<Arg3>::qt_metatype_id(), 0 }; return t; } };
#else
    template <typename ArgList> struct TypesAreDeclaredMetaType { enum { Value = false }; };
    template <> struct TypesAreDeclaredMetaType<List<>> { enum { Value = true }; };
    template <typename Arg, typename... Tail> struct TypesAreDeclaredMetaType<List<Arg, Tail...> >
    { enum { Value = QMetaTypeId2<Arg>::Defined && TypesAreDeclaredMetaType<List<Tail...>>::Value }; };

    template <typename ArgList, bool Declared = TypesAreDeclaredMetaType<ArgList>::Value > struct ConnectionTypes
    { static const int *types() { return 0; } };
    template <typename... Args> struct ConnectionTypes<List<Args...>, true>
    { static const int *types() { static const int t[sizeof...(Args) + 1] = { (QtPrivate::QMetaTypeIdHelper<Args>::qt_metatype_id())..., 0 }; return t; } };
#endif
}


QT_END_NAMESPACE

QT_END_HEADER

#endif
