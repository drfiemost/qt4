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

#ifndef QVARIANT_H
#define QVARIANT_H

#include <QtCore/qatomic.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qlist.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qmap.h>
#include <QtCore/qhash.h>
#include <QtCore/qstring.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


class QBitArray;
class QDataStream;
class QDate;
class QDateTime;
class QEasingCurve;
class QLine;
class QLineF;
class QLocale;
class QMatrix;
class QTransform;
class QStringList;
class QTime;
class QPoint;
class QPointF;
class QSize;
class QSizeF;
class QRect;
class QRectF;
#ifndef QT_NO_REGEXP
class QRegExp;
#endif
class QTextFormat;
class QTextLength;
class QUrl;
class QVariant;
class QVariantComparisonHelper;

template <typename T>
inline QVariant qVariantFromValue(const T &);

template<typename T>
inline T qvariant_cast(const QVariant &);

class Q_CORE_EXPORT QVariant
{
 public:
    enum Type {
        Invalid = QMetaType::Void,

        Bool = QMetaType::Bool,
        Int = QMetaType::Int,
        UInt = QMetaType::UInt,
        LongLong = QMetaType::LongLong,
        ULongLong = QMetaType::ULongLong,
        Double = QMetaType::Double,
        Char = QMetaType::QChar,
        Map = QMetaType::QVariantMap,
        List = QMetaType::QVariantList,
        String = QMetaType::QString,
        StringList = QMetaType::QStringList,
        ByteArray = QMetaType::QByteArray,
        BitArray = QMetaType::QBitArray,
        Date = QMetaType::QDate,
        Time = QMetaType::QTime,
        DateTime = QMetaType::QDateTime,
        Url = QMetaType::QUrl,
        Locale = QMetaType::QLocale,
        Rect = QMetaType::QRect,
        RectF = QMetaType::QRectF,
        Size = QMetaType::QSize,
        SizeF = QMetaType::QSizeF,
        Line = QMetaType::QLine,
        LineF = QMetaType::QLineF,
        Point = QMetaType::QPoint,
        PointF = QMetaType::QPointF,
        RegExp = QMetaType::QRegExp,
        Hash = QMetaType::QVariantHash,
        EasingCurve = QMetaType::QEasingCurve,
        LastCoreType = QMetaType::LastCoreType,

        Font = QMetaType::QFont,
        Pixmap = QMetaType::QPixmap,
        Brush = QMetaType::QBrush,
        Color = QMetaType::QColor,
        Palette = QMetaType::QPalette,
        Icon = QMetaType::QIcon,
        Image = QMetaType::QImage,
        Polygon = QMetaType::QPolygon,
        Region = QMetaType::QRegion,
        Bitmap = QMetaType::QBitmap,
        Cursor = QMetaType::QCursor,
        SizePolicy = QMetaType::QSizePolicy,
        KeySequence = QMetaType::QKeySequence,
        Pen = QMetaType::QPen,
        TextLength = QMetaType::QTextLength,
        TextFormat = QMetaType::QTextFormat,
        Matrix = QMetaType::QMatrix,
        Transform = QMetaType::QTransform,
        Matrix4x4 = QMetaType::QMatrix4x4,
        Vector2D = QMetaType::QVector2D,
        Vector3D = QMetaType::QVector3D,
        Vector4D = QMetaType::QVector4D,
        Quaternion = QMetaType::QQuaternion,
        PolygonF = QMetaType::QPolygonF,
        LastGuiType = QMetaType::LastGuiType,

        UserType = 127,
        LastType = 0xffffffff // need this so that gcc >= 3.4 allocates 32 bits for Type
    };

    inline QVariant();
    ~QVariant();
    QVariant(Type type);
    QVariant(int typeOrUserType, const void *copy);
    QVariant(int typeOrUserType, const void *copy, uint flags);
    QVariant(const QVariant &other);

#ifndef QT_NO_DATASTREAM
    QVariant(QDataStream &s);
#endif

    QVariant(int i);
    QVariant(uint ui);
    QVariant(qlonglong ll);
    QVariant(qulonglong ull);
    QVariant(bool b);
    QVariant(double d);
    QVariant(float f) { d.is_null = false; d.type = QMetaType::Float; d.data.f = f; }
#ifndef QT_NO_CAST_FROM_ASCII
    QT_ASCII_CAST_WARN_CONSTRUCTOR QVariant(const char *str);
#endif

    QVariant(const QByteArray &bytearray);
    QVariant(const QBitArray &bitarray);
    QVariant(const QString &string);
    QVariant(QLatin1String string);
    QVariant(const QStringList &stringlist);
    QVariant(QChar qchar);
    QVariant(const QDate &date);
    QVariant(const QTime &time);
    QVariant(const QDateTime &datetime);
    QVariant(const QList<QVariant> &list);
    QVariant(const QMap<QString,QVariant> &map);
    QVariant(const QHash<QString,QVariant> &hash);
#ifndef QT_NO_GEOM_VARIANT
    QVariant(const QSize &size);
    QVariant(const QSizeF &size);
    QVariant(const QPoint &pt);
    QVariant(const QPointF &pt);
    QVariant(const QLine &line);
    QVariant(const QLineF &line);
    QVariant(const QRect &rect);
    QVariant(const QRectF &rect);
#endif
    QVariant(const QUrl &url);
    QVariant(const QLocale &locale);
#ifndef QT_NO_REGEXP
    QVariant(const QRegExp &regExp);
#endif
#ifndef QT_BOOTSTRAPPED
    QVariant(const QEasingCurve &easing);
#endif

    QVariant& operator=(const QVariant &other);
#ifdef Q_COMPILER_RVALUE_REFS
    inline QVariant(QVariant &&other) : d(other.d)
    { other.d = Private(); }
    inline QVariant &operator=(QVariant &&other)
    { qSwap(d, other.d); return *this; }
#endif

    inline void swap(QVariant &other) { qSwap(d, other.d); }

    Type type() const;
    int userType() const;
    const char *typeName() const;

    bool canConvert(Type t) const;
    bool convert(Type t);

    inline bool isValid() const;
    bool isNull() const;

    void clear();

    void detach();
    inline bool isDetached() const;

    int toInt(bool *ok = nullptr) const;
    uint toUInt(bool *ok = nullptr) const;
    qlonglong toLongLong(bool *ok = nullptr) const;
    qulonglong toULongLong(bool *ok = nullptr) const;
    bool toBool() const;
    double toDouble(bool *ok = nullptr) const;
    float toFloat(bool *ok = nullptr) const;
    qreal toReal(bool *ok = nullptr) const;
    QByteArray toByteArray() const;
    QBitArray toBitArray() const;
    QString toString() const;
    QStringList toStringList() const;
    QChar toChar() const;
    QDate toDate() const;
    QTime toTime() const;
    QDateTime toDateTime() const;
    QList<QVariant> toList() const;
    QMap<QString, QVariant> toMap() const;
    QHash<QString, QVariant> toHash() const;

#ifndef QT_NO_GEOM_VARIANT
    QPoint toPoint() const;
    QPointF toPointF() const;
    QRect toRect() const;
    QSize toSize() const;
    QSizeF toSizeF() const;
    QLine toLine() const;
    QLineF toLineF() const;
    QRectF toRectF() const;
#endif
    QUrl toUrl() const;
    QLocale toLocale() const;
#ifndef QT_NO_REGEXP
    QRegExp toRegExp() const;
#endif
#ifndef QT_BOOTSTRAPPED
    QEasingCurve toEasingCurve() const;
#endif

#ifndef QT_NO_DATASTREAM
    void load(QDataStream &ds);
    void save(QDataStream &ds) const;
#endif
    static const char *typeToName(Type type);
    static Type nameToType(const char *name);

    void *data();
    const void *constData() const;
    inline const void *data() const { return constData(); }

    template<typename T>
    inline void setValue(const T &value);

    template<typename T>
    inline T value() const
    { return qvariant_cast<T>(*this); }

    template<typename T>
    static inline QVariant fromValue(const T &value)
    { return qVariantFromValue(value); }

    template<typename T>
    bool canConvert() const
    { return canConvert(Type(qMetaTypeId<T>())); }

 public:
#ifndef qdoc
    struct PrivateShared
    {
        inline PrivateShared(void *v) : ptr(v), ref(1) { }
        void *ptr;
        QAtomicInt ref;
    };
    struct Private
    {
        inline Private(): type(Invalid), is_shared(false), is_null(true) { data.ptr = nullptr; }
        Private(const Private &other)
             
        = default;
        Private &operator=(const Private &other) = default;
        union Data
        {
            char c;
            int i;
            uint u;
            bool b;
            double d;
            float f;
            qreal real;
            qlonglong ll;
            qulonglong ull;
            QObject *o;
            void *ptr;
            PrivateShared *shared;
        } data;
        uint type : 30;
        uint is_shared : 1;
        uint is_null : 1;
    };
 public:
    typedef void (*f_construct)(Private *, const void *);
    typedef void (*f_clear)(Private *);
    typedef bool (*f_null)(const Private *);
#ifndef QT_NO_DATASTREAM
    typedef void (*f_load)(Private *, QDataStream &);
    typedef void (*f_save)(const Private *, QDataStream &);
#endif
    typedef bool (*f_compare)(const Private *, const Private *);
    typedef bool (*f_convert)(const QVariant::Private *d, Type t, void *, bool *);
    typedef bool (*f_canConvert)(const QVariant::Private *d, Type t);
    typedef void (*f_debugStream)(QDebug, const QVariant &);
    struct Handler {
        f_construct construct;
        f_clear clear;
        f_null isNull;
#ifndef QT_NO_DATASTREAM
        f_load load;
        f_save save;
#endif
        f_compare compare;
        f_convert convert;
        f_canConvert canConvert;
        f_debugStream debugStream;
    };
#endif

    inline bool operator==(const QVariant &v) const
    { return cmp(v); }
    inline bool operator!=(const QVariant &v) const
    { return !cmp(v); }

protected:
    friend inline bool qvariant_cast_helper(const QVariant &, QVariant::Type, void *);
    friend int qRegisterGuiVariant();
    friend int qUnregisterGuiVariant();
    friend inline bool operator==(const QVariant &, const QVariantComparisonHelper &);
#ifndef QT_NO_DEBUG_STREAM
    friend Q_CORE_EXPORT QDebug operator<<(QDebug, const QVariant &);
#endif
    Private d;

    static const Handler *handler;

    void create(int type, const void *copy);

    bool cmp(const QVariant &other) const;

private:
    // force compile error, prevent QVariant(bool) to be called
    inline QVariant(void *) { Q_UNREACHABLE(); }
#ifdef QT_NO_CAST_FROM_ASCII
    // force compile error when implicit conversion is not wanted
    inline QVariant(const char *) { Q_UNREACHABLE(); }
#endif
    // force compile error, prevent QVariant(QVariant::Type, int) to be called
    inline QVariant(bool, int) { Q_UNREACHABLE(); }

public:
    typedef Private DataPtr;
    inline DataPtr &data_ptr() { return d; }
};

inline bool qvariant_cast_helper(const QVariant &v, QVariant::Type tp, void *ptr)
{ return QVariant::handler->convert(&v.d, tp, ptr, nullptr); }

template <typename T>
inline QVariant qVariantFromValue(const T &t)
{
    return QVariant(qMetaTypeId<T>(reinterpret_cast<T *>(0)), &t, QTypeInfo<T>::isPointer);
}

template <>
inline QVariant qVariantFromValue(const QVariant &t) { return t; }

template <typename T>
inline void qVariantSetValue(QVariant &v, const T &t)
{
    //if possible we reuse the current QVariant private
    const uint type = qMetaTypeId<T>(reinterpret_cast<T *>(0));
    QVariant::Private &d = v.data_ptr();
    if (v.isDetached() && (type == d.type || (type <= uint(QVariant::Char) && d.type <= uint(QVariant::Char)))) {
        d.type = type;
        d.is_null = false;
        T *old = reinterpret_cast<T*>(d.is_shared ? d.data.shared->ptr : &d.data.ptr);
        if (QTypeInfo<T>::isComplex)
            old->~T();
        new (old) T(t); //call the copy constructor
    } else {
        v = QVariant(type, &t, QTypeInfo<T>::isPointer);
    }
}

template <>
inline void qVariantSetValue<QVariant>(QVariant &v, const QVariant &t)
{
    v = t;
}


inline QVariant::QVariant() = default;
inline bool QVariant::isValid() const { return d.type != Invalid; }

template<typename T>
inline void QVariant::setValue(const T &avalue)
{ qVariantSetValue(*this, avalue); }

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream& operator>> (QDataStream& s, QVariant& p);
Q_CORE_EXPORT QDataStream& operator<< (QDataStream& s, const QVariant& p);
Q_CORE_EXPORT QDataStream& operator>> (QDataStream& s, QVariant::Type& p);
Q_CORE_EXPORT QDataStream& operator<< (QDataStream& s, const QVariant::Type p);
#endif

inline bool QVariant::isDetached() const
{ return !d.is_shared || d.data.shared->ref.loadRelaxed() == 1; }


#ifdef qdoc
    inline bool operator==(const QVariant &v1, const QVariant &v2);
    inline bool operator!=(const QVariant &v1, const QVariant &v2);
#else

/* Helper class to add one more level of indirection to prevent
   implicit casts.
*/
class QVariantComparisonHelper
{
public:
    inline QVariantComparisonHelper(const QVariant &var)
        : v(&var) {}
private:
    friend inline bool operator==(const QVariant &, const QVariantComparisonHelper &);
    const QVariant *v;
};

inline bool operator==(const QVariant &v1, const QVariantComparisonHelper &v2)
{
    return v1.cmp(*v2.v);
}

inline bool operator!=(const QVariant &v1, const QVariantComparisonHelper &v2)
{
    return !operator==(v1, v2);
}
#endif

#ifndef QT_MOC
template<typename T> inline T qvariant_cast(const QVariant &v)
{
    const int vid = qMetaTypeId<T>(static_cast<T *>(nullptr));
    if (vid == v.userType())
        return *reinterpret_cast<const T *>(v.constData());
    if (vid < int(QMetaType::User)) {
        T t;
        if (qvariant_cast_helper(v, QVariant::Type(vid), &t))
            return t;
    }
    return T();
}

template<> inline QVariant qvariant_cast<QVariant>(const QVariant &v)
{
    if (v.userType() == QMetaType::QVariant)
        return *reinterpret_cast<const QVariant *>(v.constData());
    return v;
}

#ifdef QT_DEPRECATED
template<typename T>
inline QT_DEPRECATED T qVariantValue(const QVariant &variant)
{ return qvariant_cast<T>(variant); }

template<typename T>
inline QT_DEPRECATED bool qVariantCanConvert(const QVariant &variant)
{ return variant.template canConvert<T>(); }
#endif

#endif
Q_DECLARE_SHARED(QVariant)

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QVariant &);
Q_CORE_EXPORT QDebug operator<<(QDebug, const QVariant::Type);
#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // QVARIANT_H
