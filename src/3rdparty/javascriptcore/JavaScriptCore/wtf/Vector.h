/*
 *  Copyright (C) 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef WTF_Vector_h
#define WTF_Vector_h

#include "FastAllocBase.h"
#include "Noncopyable.h"
#include "NotFound.h"
#include "VectorTraits.h"
#include <limits>
#include <utility>

#if PLATFORM(QT)
#include <QDataStream>
#endif

namespace WTF {

    using std::min;
    using std::max;

    // WTF_ALIGN_OF / WTF_ALIGNED
    #if COMPILER(GCC) || COMPILER(MINGW) || COMPILER(RVCT) || COMPILER(WINSCW)
        #define WTF_ALIGN_OF(type) __alignof__(type)
        #define WTF_ALIGNED(variable_type, variable, n) variable_type variable __attribute__((__aligned__(n)))
    #elif COMPILER(MSVC)
        #define WTF_ALIGN_OF(type) __alignof(type)
        #define WTF_ALIGNED(variable_type, variable, n) __declspec(align(n)) variable_type variable
    #else
        #define WTF_ALIGN_OF(type)   0
    #endif

    #if COMPILER(GCC) && !COMPILER(INTEL) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)
        typedef char __attribute__((__may_alias__)) AlignedBufferChar; 
    #else
        typedef char AlignedBufferChar; 
    #endif

    #ifdef WTF_ALIGNED
    template <size_t size, size_t alignment> struct AlignedBuffer;
    template <size_t size> struct AlignedBuffer<size, 1> { AlignedBufferChar buffer[size]; };
    template <size_t size> struct AlignedBuffer<size, 2> { WTF_ALIGNED(AlignedBufferChar, buffer[size], 2);  };
    template <size_t size> struct AlignedBuffer<size, 4> { WTF_ALIGNED(AlignedBufferChar, buffer[size], 4);  };
    template <size_t size> struct AlignedBuffer<size, 8> { WTF_ALIGNED(AlignedBufferChar, buffer[size], 8);  };
    template <size_t size> struct AlignedBuffer<size, 16> { WTF_ALIGNED(AlignedBufferChar, buffer[size], 16); };
    template <size_t size> struct AlignedBuffer<size, 32> { WTF_ALIGNED(AlignedBufferChar, buffer[size], 32); };
    template <size_t size> struct AlignedBuffer<size, 64> { WTF_ALIGNED(AlignedBufferChar, buffer[size], 64); };
    #else
    template <size_t size, size_t> struct AlignedBuffer
    {
        AlignedBufferChar oversizebuffer[size + 64];
        AlignedBufferChar *buffer()
        {
            AlignedBufferChar *ptr = oversizebuffer;
            ptr += 64 - (reinterpret_cast<size_t>(ptr) & 0x3f);
            return ptr;
        }
    };
    #endif

    template <size_t size, size_t alignment>
    void swap(AlignedBuffer<size, alignment>& a, AlignedBuffer<size, alignment>& b)
    {
        for (size_t i = 0; i < size; ++i)
            std::swap(a.buffer[i], b.buffer[i]);
    }

    template <bool needsDestruction, typename T>
    struct VectorDestructor;

    template<typename T>
    struct VectorDestructor<false, T>
    {
        static void destruct(T*, T*) {}
    };

    template<typename T>
    struct VectorDestructor<true, T>
    {
        static void destruct(T* begin, T* end) 
        {
            for (T* cur = begin; cur != end; ++cur)
                cur->~T();
        }
    };

    template <bool needsInitialization, bool canInitializeWithMemset, typename T>
    struct VectorInitializer;

    template<bool ignore, typename T>
    struct VectorInitializer<false, ignore, T>
    {
        static void initialize(T*, T*) {}
    };

    template<typename T>
    struct VectorInitializer<true, false, T>
    {
        static void initialize(T* begin, T* end) 
        {
            for (T* cur = begin; cur != end; ++cur)
                new (cur) T;
        }
    };

    template<typename T>
    struct VectorInitializer<true, true, T>
    {
        static void initialize(T* begin, T* end) 
        {
            std::memset(begin, 0, reinterpret_cast<char*>(end) - reinterpret_cast<char*>(begin));
        }
    };

    template <bool canMoveWithMemcpy, typename T>
    struct VectorMover;

    template<typename T>
    struct VectorMover<false, T>
    {
        static void move(T* src, const T* srcEnd, T* dst)
        {
            while (src != srcEnd) {
                new (dst) T(*src);
                src->~T();
                ++dst;
                ++src;
            }
        }
        static void moveOverlapping(T* src, const T* srcEnd, T* dst)
        {
            if (src > dst)
                move(src, srcEnd, dst);
            else {
                T* dstEnd = dst + (srcEnd - src);
                while (src != srcEnd) {
                    --srcEnd;
                    --dstEnd;
                    new (dstEnd) T(*srcEnd);
                    srcEnd->~T();
                }
            }
        }
    };

    template<typename T>
    struct VectorMover<true, T>
    {
        static void move(T* src, const T* srcEnd, T* dst) 
        {
            std::memcpy(dst, src, reinterpret_cast<const char*>(srcEnd) - reinterpret_cast<const char*>(src));
        }
        static void moveOverlapping(T* src, const T* srcEnd, T* dst) 
        {
            std::memmove(dst, src, reinterpret_cast<const char*>(srcEnd) - reinterpret_cast<const char*>(src));
        }
    };

    template <bool canCopyWithMemcpy, typename T>
    struct VectorCopier;

    template<typename T>
    struct VectorCopier<false, T>
    {
        static void uninitializedCopy(const T* src, const T* srcEnd, T* dst) 
        {
            while (src != srcEnd) {
                new (dst) T(*src);
                ++dst;
                ++src;
            }
        }
    };

    template<typename T>
    struct VectorCopier<true, T>
    {
        static void uninitializedCopy(const T* src, const T* srcEnd, T* dst) 
        {
            std::memcpy(dst, src, reinterpret_cast<const char*>(srcEnd) - reinterpret_cast<const char*>(src));
        }
    };

    template <bool canFillWithMemset, typename T>
    struct VectorFiller;

    template<typename T>
    struct VectorFiller<false, T>
    {
        static void uninitializedFill(T* dst, T* dstEnd, const T& val) 
        {
            while (dst != dstEnd) {
                new (dst) T(val);
                ++dst;
            }
        }
    };

    template<typename T>
    struct VectorFiller<true, T>
    {
        static void uninitializedFill(T* dst, T* dstEnd, const T& val) 
        {
            ASSERT(sizeof(T) == sizeof(char));
            std::memset(dst, val, dstEnd - dst);
        }
    };

    template<bool canCompareWithMemcmp, typename T>
    struct VectorComparer;

    template<typename T>
    struct VectorComparer<false, T>
    {
        static bool compare(const T* a, const T* b, size_t size)
        {
            for (size_t i = 0; i < size; ++i)
                if (a[i] != b[i])
                    return false;
            return true;
        }
    };

    template<typename T>
    struct VectorComparer<true, T>
    {
        static bool compare(const T* a, const T* b, size_t size)
        {
            return std::memcmp(a, b, sizeof(T) * size) == 0;
        }
    };

    template<typename T>
    struct VectorTypeOperations
    {
        static void destruct(T* begin, T* end)
        {
            VectorDestructor<VectorTraits<T>::needsDestruction, T>::destruct(begin, end);
        }

        static void initialize(T* begin, T* end)
        {
            VectorInitializer<VectorTraits<T>::needsInitialization, VectorTraits<T>::canInitializeWithMemset, T>::initialize(begin, end);
        }

        static void move(T* src, const T* srcEnd, T* dst)
        {
            VectorMover<VectorTraits<T>::canMoveWithMemcpy, T>::move(src, srcEnd, dst);
        }

        static void moveOverlapping(T* src, const T* srcEnd, T* dst)
        {
            VectorMover<VectorTraits<T>::canMoveWithMemcpy, T>::moveOverlapping(src, srcEnd, dst);
        }

        static void uninitializedCopy(const T* src, const T* srcEnd, T* dst)
        {
            VectorCopier<VectorTraits<T>::canCopyWithMemcpy, T>::uninitializedCopy(src, srcEnd, dst);
        }

        static void uninitializedFill(T* dst, T* dstEnd, const T& val)
        {
            VectorFiller<VectorTraits<T>::canFillWithMemset, T>::uninitializedFill(dst, dstEnd, val);
        }

        static bool compare(const T* a, const T* b, size_t size)
        {
            return VectorComparer<VectorTraits<T>::canCompareWithMemcmp, T>::compare(a, b, size);
        }
    };

    template<typename T>
    class VectorBufferBase : public Noncopyable {
    public:
        void allocateBuffer(size_t newCapacity)
        {
            m_capacity = newCapacity;
            if (newCapacity > std::numeric_limits<size_t>::max() / sizeof(T))
                CRASH();
            m_buffer = static_cast<T*>(fastMalloc(newCapacity * sizeof(T)));
        }

        void deallocateBuffer(T* bufferToDeallocate)
        {
            if (m_buffer == bufferToDeallocate) {
                m_buffer = nullptr;
                m_capacity = 0;
            }
            fastFree(bufferToDeallocate);
        }

        T* buffer() { return m_buffer; }
        const T* buffer() const { return m_buffer; }
        T** bufferSlot() { return &m_buffer; }
        size_t capacity() const { return m_capacity; }

        T* releaseBuffer()
        {
            T* buffer = m_buffer;
            m_buffer = nullptr;
            m_capacity = 0;
            return buffer;
        }

    protected:
        VectorBufferBase()
            : m_buffer(nullptr)
            , m_capacity(0)
        {
        }

        VectorBufferBase(T* buffer, size_t capacity)
            : m_buffer(buffer)
            , m_capacity(capacity)
        {
        }

        ~VectorBufferBase()
        {
            // FIXME: It would be nice to find a way to ASSERT that m_buffer hasn't leaked here.
        }

        T* m_buffer;
        size_t m_capacity;
    };

    template<typename T, size_t inlineCapacity>
    class VectorBuffer;

    template<typename T>
    class VectorBuffer<T, 0> : private VectorBufferBase<T> {
    private:
        typedef VectorBufferBase<T> Base;
    public:
        VectorBuffer()
        = default;

        VectorBuffer(size_t capacity)
        {
            allocateBuffer(capacity);
        }

        ~VectorBuffer()
        {
            deallocateBuffer(buffer());
        }
        
        void swap(VectorBuffer<T, 0>& other)
        {
            std::swap(m_buffer, other.m_buffer);
            std::swap(m_capacity, other.m_capacity);
        }
        
        void restoreInlineBufferIfNeeded() { }

        using Base::allocateBuffer;
        using Base::deallocateBuffer;

        using Base::buffer;
        using Base::bufferSlot;
        using Base::capacity;

        using Base::releaseBuffer;
    private:
        using Base::m_buffer;
        using Base::m_capacity;
    };

    template<typename T, size_t inlineCapacity>
    class VectorBuffer : private VectorBufferBase<T> {
    private:
        typedef VectorBufferBase<T> Base;
    public:
        VectorBuffer()
            : Base(inlineBuffer(), inlineCapacity)
        {
        }

        VectorBuffer(size_t capacity)
            : Base(inlineBuffer(), inlineCapacity)
        {
            if (capacity > inlineCapacity)
                Base::allocateBuffer(capacity);
        }

        ~VectorBuffer()
        {
            deallocateBuffer(buffer());
        }

        void allocateBuffer(size_t newCapacity)
        {
            if (newCapacity > inlineCapacity)
                Base::allocateBuffer(newCapacity);
            else {
                m_buffer = inlineBuffer();
                m_capacity = inlineCapacity;
            }
        }

        void deallocateBuffer(T* bufferToDeallocate)
        {
            if (bufferToDeallocate == inlineBuffer())
                return;
            Base::deallocateBuffer(bufferToDeallocate);
        }
        
        void swap(VectorBuffer<T, inlineCapacity>& other)
        {
            if (buffer() == inlineBuffer() && other.buffer() == other.inlineBuffer()) {
                WTF::swap(m_inlineBuffer, other.m_inlineBuffer);
                std::swap(m_capacity, other.m_capacity);
            } else if (buffer() == inlineBuffer()) {
                m_buffer = other.m_buffer;
                other.m_buffer = other.inlineBuffer();
                WTF::swap(m_inlineBuffer, other.m_inlineBuffer);
                std::swap(m_capacity, other.m_capacity);
            } else if (other.buffer() == other.inlineBuffer()) {
                other.m_buffer = m_buffer;
                m_buffer = inlineBuffer();
                WTF::swap(m_inlineBuffer, other.m_inlineBuffer);
                std::swap(m_capacity, other.m_capacity);
            } else {
                std::swap(m_buffer, other.m_buffer);
                std::swap(m_capacity, other.m_capacity);
            }
        }

        void restoreInlineBufferIfNeeded()
        {
            if (m_buffer)
                return;
            m_buffer = inlineBuffer();
            m_capacity = inlineCapacity;
        }

        using Base::buffer;
        using Base::bufferSlot;
        using Base::capacity;

        T* releaseBuffer()
        {
            if (buffer() == inlineBuffer())
                return nullptr;
            return Base::releaseBuffer();
        }

    private:
        using Base::m_buffer;
        using Base::m_capacity;

        static const size_t m_inlineBufferSize = inlineCapacity * sizeof(T);
        #ifdef WTF_ALIGNED
        T* inlineBuffer() { return reinterpret_cast<T*>(m_inlineBuffer.buffer); }
        #else
        T* inlineBuffer() { return reinterpret_cast<T*>(m_inlineBuffer.buffer()); }
        #endif

        AlignedBuffer<m_inlineBufferSize, WTF_ALIGN_OF(T)> m_inlineBuffer;
    };

    template<typename T, size_t inlineCapacity = 0>
    class Vector : public FastAllocBase {
    private:
        typedef VectorBuffer<T, inlineCapacity> Buffer;
        typedef VectorTypeOperations<T> TypeOperations;

    public:
        typedef T ValueType;

        typedef T* iterator;
        typedef const T* const_iterator;

        Vector() 
            : m_size(0)
        {
        }
        
        explicit Vector(size_t size) 
            : m_size(size)
            , m_buffer(size)
        {
            if (begin())
                TypeOperations::initialize(begin(), end());
        }

        ~Vector()
        {
            if (m_size) shrink(0);
        }

        Vector(const Vector&);
        template<size_t otherCapacity> 
        Vector(const Vector<T, otherCapacity>&);

        Vector& operator=(const Vector&);
        template<size_t otherCapacity> 
        Vector& operator=(const Vector<T, otherCapacity>&);

        size_t size() const { return m_size; }
        size_t capacity() const { return m_buffer.capacity(); }
        bool isEmpty() const { return !size(); }

        T& at(size_t i) 
        { 
            ASSERT(i < size());
            return m_buffer.buffer()[i]; 
        }
        const T& at(size_t i) const 
        {
            ASSERT(i < size());
            return m_buffer.buffer()[i]; 
        }

        T& operator[](size_t i) { return at(i); }
        const T& operator[](size_t i) const { return at(i); }

        T* data() { return m_buffer.buffer(); }
        const T* data() const { return m_buffer.buffer(); }
        T** dataSlot() { return m_buffer.bufferSlot(); }

        iterator begin() { return data(); }
        iterator end() { return begin() + m_size; }
        const_iterator begin() const { return data(); }
        const_iterator end() const { return begin() + m_size; }
        
        T& first() { return at(0); }
        const T& first() const { return at(0); }
        T& last() { return at(size() - 1); }
        const T& last() const { return at(size() - 1); }

        template<typename U> size_t find(const U&) const;

        void shrink(size_t size);
        void grow(size_t size);
        void resize(size_t size);
        void reserveCapacity(size_t newCapacity);
        void reserveInitialCapacity(size_t initialCapacity);
        void shrinkCapacity(size_t newCapacity);
        void shrinkToFit() { shrinkCapacity(size()); }

        void clear() { shrinkCapacity(0); }

        template<typename U> void append(const U*, size_t);
        template<typename U> void append(const U&);
        template<typename U> void uncheckedAppend(const U& val);
        template<size_t otherCapacity> void append(const Vector<T, otherCapacity>&);

        template<typename U> void insert(size_t position, const U*, size_t);
        template<typename U> void insert(size_t position, const U&);
        template<typename U, size_t c> void insert(size_t position, const Vector<U, c>&);

        template<typename U> void prepend(const U*, size_t);
        template<typename U> void prepend(const U&);
        template<typename U, size_t c> void prepend(const Vector<U, c>&);

        void remove(size_t position);
        void remove(size_t position, size_t length);

        void removeLast() 
        {
            ASSERT(!isEmpty());
            shrink(size() - 1); 
        }

        Vector(size_t size, const T& val)
            : m_size(size)
            , m_buffer(size)
        {
            if (begin())
                TypeOperations::uninitializedFill(begin(), end(), val);
        }

        void fill(const T&, size_t);
        void fill(const T& val) { fill(val, size()); }

        template<typename Iterator> void appendRange(Iterator start, Iterator end);

        T* releaseBuffer();

        void swap(Vector<T, inlineCapacity>& other)
        {
            std::swap(m_size, other.m_size);
            m_buffer.swap(other.m_buffer);
        }

    private:
        void expandCapacity(size_t newMinCapacity);
        const T* expandCapacity(size_t newMinCapacity, const T*);
        template<typename U> U* expandCapacity(size_t newMinCapacity, U*); 

        size_t m_size;
        Buffer m_buffer;
    };

#if PLATFORM(QT)
    QT_USE_NAMESPACE
    template<typename T>
    QDataStream& operator<<(QDataStream& stream, const Vector<T>& data)
    {
        stream << qint64(data.size());
        for (const T& i : data)
            stream << i;
        return stream;
    }

    template<typename T>
    QDataStream& operator>>(QDataStream& stream, Vector<T>& data)
    {
        data.clear();
        qint64 count;
        T item;
        stream >> count;
        data.reserveCapacity(count);
        for (qint64 i = 0; i < count; ++i) {
            stream >> item;
            data.append(item);
        }
        return stream;
    }
#endif

    template<typename T, size_t inlineCapacity>
    Vector<T, inlineCapacity>::Vector(const Vector& other)
        : m_size(other.size())
        , m_buffer(other.capacity())
    {
        if (begin())
            TypeOperations::uninitializedCopy(other.begin(), other.end(), begin());
    }

    template<typename T, size_t inlineCapacity>
    template<size_t otherCapacity> 
    Vector<T, inlineCapacity>::Vector(const Vector<T, otherCapacity>& other)
        : m_size(other.size())
        , m_buffer(other.capacity())
    {
        if (begin())
            TypeOperations::uninitializedCopy(other.begin(), other.end(), begin());
    }

    template<typename T, size_t inlineCapacity>
    Vector<T, inlineCapacity>& Vector<T, inlineCapacity>::operator=(const Vector<T, inlineCapacity>& other)
    {
        if (&other == this)
            return *this;
        
        if (size() > other.size())
            shrink(other.size());
        else if (other.size() > capacity()) {
            clear();
            reserveCapacity(other.size());
            if (!begin())
                return *this;
        }
        
        std::copy(other.begin(), other.begin() + size(), begin());
        TypeOperations::uninitializedCopy(other.begin() + size(), other.end(), end());
        m_size = other.size();

        return *this;
    }

    template<typename T, size_t inlineCapacity>
    template<size_t otherCapacity> 
    Vector<T, inlineCapacity>& Vector<T, inlineCapacity>::operator=(const Vector<T, otherCapacity>& other)
    {
        if (&other == this)
            return *this;
        
        if (size() > other.size())
            shrink(other.size());
        else if (other.size() > capacity()) {
            clear();
            reserveCapacity(other.size());
            if (!begin())
                return *this;
        }
        
        std::copy(other.begin(), other.begin() + size(), begin());
        TypeOperations::uninitializedCopy(other.begin() + size(), other.end(), end());
        m_size = other.size();

        return *this;
    }

    template<typename T, size_t inlineCapacity>
    template<typename U>
    size_t Vector<T, inlineCapacity>::find(const U& value) const
    {
        for (size_t i = 0; i < size(); ++i) {
            if (at(i) == value)
                return i;
        }
        return notFound;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::fill(const T& val, size_t newSize)
    {
        if (size() > newSize)
            shrink(newSize);
        else if (newSize > capacity()) {
            clear();
            reserveCapacity(newSize);
            if (!begin())
                return;
        }
        
        std::fill(begin(), end(), val);
        TypeOperations::uninitializedFill(end(), begin() + newSize, val);
        m_size = newSize;
    }

    template<typename T, size_t inlineCapacity>
    template<typename Iterator>
    void Vector<T, inlineCapacity>::appendRange(Iterator start, Iterator end)
    {
        for (Iterator it = start; it != end; ++it)
            append(*it);
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::expandCapacity(size_t newMinCapacity)
    {
        reserveCapacity(max(newMinCapacity, max(static_cast<size_t>(16), capacity() + capacity() / 4 + 1)));
    }
    
    template<typename T, size_t inlineCapacity>
    const T* Vector<T, inlineCapacity>::expandCapacity(size_t newMinCapacity, const T* ptr)
    {
        if (ptr < begin() || ptr >= end()) {
            expandCapacity(newMinCapacity);
            return ptr;
        }
        size_t index = ptr - begin();
        expandCapacity(newMinCapacity);
        return begin() + index;
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    inline U* Vector<T, inlineCapacity>::expandCapacity(size_t newMinCapacity, U* ptr)
    {
        expandCapacity(newMinCapacity);
        return ptr;
    }

    template<typename T, size_t inlineCapacity>
    inline void Vector<T, inlineCapacity>::resize(size_t size)
    {
        if (size <= m_size)
            TypeOperations::destruct(begin() + size, end());
        else {
            if (size > capacity())
                expandCapacity(size);
            if (begin())
                TypeOperations::initialize(end(), begin() + size);
        }
        
        m_size = size;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::shrink(size_t size)
    {
        ASSERT(size <= m_size);
        TypeOperations::destruct(begin() + size, end());
        m_size = size;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::grow(size_t size)
    {
        ASSERT(size >= m_size);
        if (size > capacity())
            expandCapacity(size);
        if (begin())
            TypeOperations::initialize(end(), begin() + size);
        m_size = size;
    }

    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::reserveCapacity(size_t newCapacity)
    {
        if (newCapacity <= capacity())
            return;
        T* oldBuffer = begin();
        T* oldEnd = end();
        m_buffer.allocateBuffer(newCapacity);
        if (begin())
            TypeOperations::move(oldBuffer, oldEnd, begin());
        m_buffer.deallocateBuffer(oldBuffer);
    }
    
    template<typename T, size_t inlineCapacity>
    inline void Vector<T, inlineCapacity>::reserveInitialCapacity(size_t initialCapacity)
    {
        ASSERT(!m_size);
        ASSERT(capacity() == inlineCapacity);
        if (initialCapacity > inlineCapacity)
            m_buffer.allocateBuffer(initialCapacity);
    }
    
    template<typename T, size_t inlineCapacity>
    void Vector<T, inlineCapacity>::shrinkCapacity(size_t newCapacity)
    {
        if (newCapacity >= capacity())
            return;

        if (newCapacity < size()) 
            shrink(newCapacity);

        T* oldBuffer = begin();
        if (newCapacity > 0) {
            T* oldEnd = end();
            m_buffer.allocateBuffer(newCapacity);
            if (begin() != oldBuffer)
                TypeOperations::move(oldBuffer, oldEnd, begin());
        }

        m_buffer.deallocateBuffer(oldBuffer);
        m_buffer.restoreInlineBufferIfNeeded();
    }

    // Templatizing these is better than just letting the conversion happen implicitly,
    // because for instance it allows a PassRefPtr to be appended to a RefPtr vector
    // without refcount thrash.

    template<typename T, size_t inlineCapacity> template<typename U>
    void Vector<T, inlineCapacity>::append(const U* data, size_t dataSize)
    {
        size_t newSize = m_size + dataSize;
        if (newSize > capacity()) {
            data = expandCapacity(newSize, data);
            if (!begin())
                return;
        }
        if (newSize < m_size)
            CRASH();
        T* dest = end();
        for (size_t i = 0; i < dataSize; ++i)
            new (&dest[i]) T(data[i]);
        m_size = newSize;
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    ALWAYS_INLINE void Vector<T, inlineCapacity>::append(const U& val)
    {
        const U* ptr = &val;
        if (size() == capacity()) {
            ptr = expandCapacity(size() + 1, ptr);
            if (!begin())
                return;
        }
            
#if COMPILER(MSVC7)
        // FIXME: MSVC7 generates compilation errors when trying to assign
        // a pointer to a Vector of its base class (i.e. can't downcast). So far
        // I've been unable to determine any logical reason for this, so I can
        // only assume it is a bug with the compiler. Casting is a bad solution,
        // however, because it subverts implicit conversions, so a better 
        // one is needed. 
        new (end()) T(static_cast<T>(*ptr));
#else
        new (end()) T(*ptr);
#endif
        ++m_size;
    }

    // This version of append saves a branch in the case where you know that the
    // vector's capacity is large enough for the append to succeed.

    template<typename T, size_t inlineCapacity> template<typename U>
    inline void Vector<T, inlineCapacity>::uncheckedAppend(const U& val)
    {
        ASSERT(size() < capacity());
        const U* ptr = &val;
        new (end()) T(*ptr);
        ++m_size;
    }

    // This method should not be called append, a better name would be appendElements.
    // It could also be eliminated entirely, and call sites could just use
    // appendRange(val.begin(), val.end()).
    template<typename T, size_t inlineCapacity> template<size_t otherCapacity>
    inline void Vector<T, inlineCapacity>::append(const Vector<T, otherCapacity>& val)
    {
        append(val.begin(), val.size());
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    void Vector<T, inlineCapacity>::insert(size_t position, const U* data, size_t dataSize)
    {
        ASSERT(position <= size());
        size_t newSize = m_size + dataSize;
        if (newSize > capacity()) {
            data = expandCapacity(newSize, data);
            if (!begin())
                return;
        }
        if (newSize < m_size)
            CRASH();
        T* spot = begin() + position;
        TypeOperations::moveOverlapping(spot, end(), spot + dataSize);
        for (size_t i = 0; i < dataSize; ++i)
            new (&spot[i]) T(data[i]);
        m_size = newSize;
    }
     
    template<typename T, size_t inlineCapacity> template<typename U>
    inline void Vector<T, inlineCapacity>::insert(size_t position, const U& val)
    {
        ASSERT(position <= size());
        const U* data = &val;
        if (size() == capacity()) {
            data = expandCapacity(size() + 1, data);
            if (!begin())
                return;
        }
        T* spot = begin() + position;
        TypeOperations::moveOverlapping(spot, end(), spot + 1);
        new (spot) T(*data);
        ++m_size;
    }
   
    template<typename T, size_t inlineCapacity> template<typename U, size_t c>
    inline void Vector<T, inlineCapacity>::insert(size_t position, const Vector<U, c>& val)
    {
        insert(position, val.begin(), val.size());
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    void Vector<T, inlineCapacity>::prepend(const U* data, size_t dataSize)
    {
        insert(0, data, dataSize);
    }

    template<typename T, size_t inlineCapacity> template<typename U>
    inline void Vector<T, inlineCapacity>::prepend(const U& val)
    {
        insert(0, val);
    }
   
    template<typename T, size_t inlineCapacity> template<typename U, size_t c>
    inline void Vector<T, inlineCapacity>::prepend(const Vector<U, c>& val)
    {
        insert(0, val.begin(), val.size());
    }
    
    template<typename T, size_t inlineCapacity>
    inline void Vector<T, inlineCapacity>::remove(size_t position)
    {
        ASSERT(position < size());
        T* spot = begin() + position;
        spot->~T();
        TypeOperations::moveOverlapping(spot + 1, end(), spot);
        --m_size;
    }

    template<typename T, size_t inlineCapacity>
    inline void Vector<T, inlineCapacity>::remove(size_t position, size_t length)
    {
        ASSERT(position < size());
        ASSERT(position + length <= size());
        T* beginSpot = begin() + position;
        T* endSpot = beginSpot + length;
        TypeOperations::destruct(beginSpot, endSpot); 
        TypeOperations::moveOverlapping(endSpot, end(), beginSpot);
        m_size -= length;
    }

    template<typename T, size_t inlineCapacity>
    inline T* Vector<T, inlineCapacity>::releaseBuffer()
    {
        T* buffer = m_buffer.releaseBuffer();
        if (inlineCapacity && !buffer && m_size) {
            // If the vector had some data, but no buffer to release,
            // that means it was using the inline buffer. In that case,
            // we create a brand new buffer so the caller always gets one.
            size_t bytes = m_size * sizeof(T);
            buffer = static_cast<T*>(fastMalloc(bytes));
            std::memcpy(buffer, data(), bytes);
        }
        m_size = 0;
        return buffer;
    }

    template<typename T, size_t inlineCapacity>
    void deleteAllValues(const Vector<T, inlineCapacity>& collection)
    {
        typedef typename Vector<T, inlineCapacity>::const_iterator iterator;
        iterator end = collection.end();
        for (iterator it = collection.begin(); it != end; ++it)
            delete *it;
    }

    template<typename T, size_t inlineCapacity>
    inline void swap(Vector<T, inlineCapacity>& a, Vector<T, inlineCapacity>& b)
    {
        a.swap(b);
    }

    template<typename T, size_t inlineCapacity>
    bool operator==(const Vector<T, inlineCapacity>& a, const Vector<T, inlineCapacity>& b)
    {
        if (a.size() != b.size())
            return false;

        return VectorTypeOperations<T>::compare(a.data(), b.data(), a.size());
    }

    template<typename T, size_t inlineCapacity>
    inline bool operator!=(const Vector<T, inlineCapacity>& a, const Vector<T, inlineCapacity>& b)
    {
        return !(a == b);
    }


} // namespace WTF

using WTF::Vector;

#endif // WTF_Vector_h
