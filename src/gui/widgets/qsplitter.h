/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#ifndef QSPLITTER_H
#define QSPLITTER_H

#include <QtGui/qframe.h>
#include <QtGui/qsizepolicy.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_SPLITTER

class QSplitterPrivate;
class QTextStream;
template <typename T> class QList;

class QSplitterHandle;

class Q_GUI_EXPORT QSplitter : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
    Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)
    Q_PROPERTY(int handleWidth READ handleWidth WRITE setHandleWidth)
    Q_PROPERTY(bool childrenCollapsible READ childrenCollapsible WRITE setChildrenCollapsible)

public:
    explicit QSplitter(QWidget* parent = nullptr);
    explicit QSplitter(Qt::Orientation, QWidget* parent = nullptr);
    ~QSplitter() override;

    void addWidget(QWidget *widget);
    void insertWidget(int index, QWidget *widget);

    void setOrientation(Qt::Orientation);
    Qt::Orientation orientation() const;

    void setChildrenCollapsible(bool);
    bool childrenCollapsible() const;

    void setCollapsible(int index, bool);
    bool isCollapsible(int index) const;
    void setOpaqueResize(bool opaque = true);
    bool opaqueResize() const;
    void refresh();

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    QList<int> sizes() const;
    void setSizes(const QList<int> &list);

    QByteArray saveState() const;
    bool restoreState(const QByteArray &state);

    int handleWidth() const;
    void setHandleWidth(int);

    int indexOf(QWidget *w) const;
    QWidget *widget(int index) const;
    int count() const;

    void getRange(int index, int *, int *) const;
    QSplitterHandle *handle(int index) const;

    void setStretchFactor(int index, int stretch);

Q_SIGNALS:
    void splitterMoved(int pos, int index);

protected:
    virtual QSplitterHandle *createHandle();

    void childEvent(QChildEvent *) override;

    bool event(QEvent *) override;
    void resizeEvent(QResizeEvent *) override;

    void changeEvent(QEvent *) override;
    void moveSplitter(int pos, int index);
    void setRubberBand(int position);
    int closestLegalPosition(int, int);

private:
    Q_DISABLE_COPY(QSplitter)
    Q_DECLARE_PRIVATE(QSplitter)
private:
    friend class QSplitterHandle;
};

#ifndef QT_NO_TEXTSTREAM
Q_GUI_EXPORT QTextStream& operator<<(QTextStream&, const QSplitter&);
Q_GUI_EXPORT QTextStream& operator>>(QTextStream&, QSplitter&);
#endif

class QSplitterHandlePrivate;
class Q_GUI_EXPORT QSplitterHandle : public QWidget
{
    Q_OBJECT
public:
    explicit QSplitterHandle(Qt::Orientation o, QSplitter *parent);
    ~QSplitterHandle() override;
    void setOrientation(Qt::Orientation o);
    Qt::Orientation orientation() const;
    bool opaqueResize() const;
    QSplitter *splitter() const;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mousePressEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    bool event(QEvent *) override;

    void moveSplitter(int p);
    int closestLegalPosition(int p);

private:
    Q_DISABLE_COPY(QSplitterHandle)
    Q_DECLARE_PRIVATE(QSplitterHandle)
};

#endif // QT_NO_SPLITTER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSPLITTER_H
