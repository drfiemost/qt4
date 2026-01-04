/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2013 Olivier Goffart <ogoffart@woboq.com>
** Contact: http://www.qt-project.org/
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
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef Q_QDOC

#ifndef QOBJECTDEFS_H
#error Do not include qobjectdefs_impl.h directly
#endif


QT_BEGIN_NAMESPACE

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
    // With variadic template, lists are represented using a variadic template argument instead of the lisp way
    template <typename...> struct List {};
    template <typename Head, typename... Tail> struct List<Head, Tail...> { typedef Head Car; typedef List<Tail...> Cdr; };
    template <typename, typename> struct List_Append;
    template <typename... L1, typename...L2> struct List_Append<List<L1...>, List<L2...>> { typedef List<L1..., L2...> Value; };
    template <typename L, int N> struct List_Left {
        typedef typename List_Append<List<typename L::Car>,typename List_Left<typename L::Cdr, N - 1>::Value>::Value Value;
    };
    template <typename L> struct List_Left<L, 0> { typedef List<> Value; };

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
        explicit ApplyReturnValue(void *data) : data(data) {}
    };
    template<typename T, typename U>
    void operator,(const T &value, const ApplyReturnValue<U> &container) {
        if (container.data)
            *reinterpret_cast<U*>(container.data) = value;
    }

    template<typename T, typename U>
    void operator,(T &&value, const ApplyReturnValue<U> &container) {
        if (container.data)
            *reinterpret_cast<U*>(container.data) = value;
    }

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
    template <int...> struct IndexesList {};
    template <typename IndexList, int Right> struct IndexesAppend;
    template <int... Left, int Right> struct IndexesAppend<IndexesList<Left...>, Right>
    { typedef IndexesList<Left..., Right> Value; };
    template <int N> struct Indexes
    { typedef typename IndexesAppend<typename Indexes<N - 1>::Value, N - 1>::Value Value; };
    template <> struct Indexes<0> { typedef IndexesList<> Value; };
    template<typename Func> struct FunctionPointer { enum {ArgumentCount = -1, IsPointerToMemberFunction = false}; };

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
    template <int... I, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<I...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) const> {
        static void call(SlotRet (Obj::*f)(SlotArgs...) const, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[I+1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) noexcept> {
        static void call(SlotRet (Obj::*f)(SlotArgs...) noexcept, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II+1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
    template <int... II, typename... SignalArgs, typename R, typename... SlotArgs, typename SlotRet, class Obj>
    struct FunctorCall<IndexesList<II...>, List<SignalArgs...>, R, SlotRet (Obj::*)(SlotArgs...) const noexcept> {
        static void call(SlotRet (Obj::*f)(SlotArgs...) const noexcept, Obj *o, void **arg) {
            (o->*f)((*reinterpret_cast<typename RemoveRef<SignalArgs>::Type *>(arg[II+1]))...), ApplyReturnValue<R>(arg[0]);
        }
    };
#endif

    template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...)>
    {
        typedef Obj Object;
        typedef List<Args...>  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Args...);
        enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };
    template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...) const>
    {
        typedef Obj Object;
        typedef List<Args...>  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Args...) const;
        enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
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
        enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = false};
        template <typename SignalArgs, typename R>
        static void call(Function f, void *, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };

#if defined(__cpp_noexcept_function_type) && __cpp_noexcept_function_type >= 201510
    template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...) noexcept>
    {
        typedef Obj Object;
        typedef List<Args...>  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Args...) noexcept;
        template <class Base> struct ChangeClass { typedef Ret (Base:: *Type)(Args...) noexcept; };
        enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };
    template<class Obj, typename Ret, typename... Args> struct FunctionPointer<Ret (Obj::*) (Args...) const noexcept>
    {
        typedef Obj Object;
        typedef List<Args...>  Arguments;
        typedef Ret ReturnType;
        typedef Ret (Obj::*Function) (Args...) const noexcept;
        template <class Base> struct ChangeClass { typedef Ret (Base:: *Type)(Args...) const noexcept; };
        enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = true};
        template <typename SignalArgs, typename R>
        static void call(Function f, Obj *o, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, o, arg);
        }
    };

    template<typename Ret, typename... Args> struct FunctionPointer<Ret (*) (Args...) noexcept>
    {
        typedef List<Args...> Arguments;
        typedef Ret ReturnType;
        typedef Ret (*Function) (Args...) noexcept;
        enum {ArgumentCount = sizeof...(Args), IsPointerToMemberFunction = false};
        template <typename SignalArgs, typename R>
        static void call(Function f, void *, void **arg) {
            FunctorCall<typename Indexes<ArgumentCount>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };
#endif

    template<typename Function, int N> struct Functor
    {
        template <typename SignalArgs, typename R>
        static void call(Function &f, void *, void **arg) {
            FunctorCall<typename Indexes<N>::Value, SignalArgs, R, Function>::call(f, arg);
        }
    };

    /*
       Logic that check if the arguments of the slot matches the argument of the signal.
       To be used like this:
       static_assert(CheckCompatibleArguments<FunctionPointer<Signal>::Arguments, FunctionPointer<Slot>::Arguments>::value)
    */
    template<typename A1, typename A2> struct AreArgumentsCompatible {
        static int test(const std::remove_reference_t<A2>&);
        static char test(...);
        enum { value = sizeof(test(std::declval<std::remove_reference_t<A1>>())) == sizeof(int) };
    };
    template<typename A1, typename A2> struct AreArgumentsCompatible<A1, A2&> { enum { value = false }; };
    template<typename A> struct AreArgumentsCompatible<A&, A&> { enum { value = true }; };
    // void as a return value
    template<typename A> struct AreArgumentsCompatible<void, A> { enum { value = true }; };
    template<typename A> struct AreArgumentsCompatible<A, void> { enum { value = true }; };
    template<> struct AreArgumentsCompatible<void, void> { enum { value = true }; };

    template <typename List1, typename List2> struct CheckCompatibleArguments { enum { value = false }; };
    template <> struct CheckCompatibleArguments<List<>, List<>> { enum { value = true }; };
    template <typename List1> struct CheckCompatibleArguments<List1, List<>> { enum { value = true }; };
    template <typename Arg1, typename Arg2, typename... Tail1, typename... Tail2>
    struct CheckCompatibleArguments<List<Arg1, Tail1...>, List<Arg2, Tail2...>>
    {
        enum { value = AreArgumentsCompatible<typename RemoveConstRef<Arg1>::Type, typename RemoveConstRef<Arg2>::Type>::value
                    && CheckCompatibleArguments<List<Tail1...>, List<Tail2...>>::value };
    };

    /*
       Find the maximum number of arguments a functor object can take and be still compatible with
       the arguments from the signal.
       Value is the number of arguments, or -1 if nothing matches.
     */
    template <typename Functor, typename ArgList> struct ComputeFunctorArgumentCount;

    template <typename Functor, typename ArgList, bool Done> struct ComputeFunctorArgumentCountHelper
    { enum { Value = -1 }; };
    template <typename Functor, typename First, typename... ArgList>
    struct ComputeFunctorArgumentCountHelper<Functor, List<First, ArgList...>, false>
        : ComputeFunctorArgumentCount<Functor,
            typename List_Left<List<First, ArgList...>, sizeof...(ArgList)>::Value> {};

    template <typename Functor, typename... ArgList> struct ComputeFunctorArgumentCount<Functor, List<ArgList...>>
    {
        template <typename F> static auto test(F f) -> decltype(((f.operator()((std::declval<ArgList>())...)), int()));
        static char test(...);
        enum {
            Ok = sizeof(test(std::declval<Functor>())) == sizeof(int),
            Value = Ok ? int(sizeof...(ArgList)) : int(ComputeFunctorArgumentCountHelper<Functor, List<ArgList...>, Ok>::Value)
        };
    };

    /* get the return type of a functor, given the signal argument list  */
    template <typename Functor, typename ArgList> struct FunctorReturnType;
    template <typename Functor, typename ... ArgList> struct FunctorReturnType<Functor, List<ArgList...>> {
        typedef decltype(std::declval<Functor>().operator()((std::declval<ArgList>())...)) Value;
    };

}

QT_END_NAMESPACE


#endif
