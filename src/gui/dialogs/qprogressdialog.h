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

#ifndef QPROGRESSDIALOG_H
#define QPROGRESSDIALOG_H

#include <QtGui/qdialog.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE


#ifndef QT_NO_PROGRESSDIALOG

class QPushButton;
class QLabel;
class QProgressBar;
class QTimer;
class QProgressDialogPrivate;

class Q_GUI_EXPORT QProgressDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QProgressDialog)
    Q_PROPERTY(bool wasCanceled READ wasCanceled)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum)
    Q_PROPERTY(int maximum READ maximum WRITE setMaximum)
    Q_PROPERTY(int value READ value WRITE setValue)
    Q_PROPERTY(bool autoReset READ autoReset WRITE setAutoReset)
    Q_PROPERTY(bool autoClose READ autoClose WRITE setAutoClose)
    Q_PROPERTY(int minimumDuration READ minimumDuration WRITE setMinimumDuration)
    Q_PROPERTY(QString labelText READ labelText WRITE setLabelText)

public:
    explicit QProgressDialog(QWidget *parent = nullptr, Qt::WindowFlags flags = nullptr);
    QProgressDialog(const QString &labelText, const QString &cancelButtonText,
                    int minimum, int maximum, QWidget *parent = nullptr, Qt::WindowFlags flags = nullptr);
    ~QProgressDialog() override;

    void setLabel(QLabel *label);
    void setCancelButton(QPushButton *button);
    void setBar(QProgressBar *bar);

    bool wasCanceled() const;

    int minimum() const;
    int maximum() const;

    int value() const;

    QSize sizeHint() const override;

    QString labelText() const;
    int minimumDuration() const;

    void setAutoReset(bool reset);
    bool autoReset() const;
    void setAutoClose(bool close);
    bool autoClose() const;

    using QDialog::open;

    void open(QObject *receiver, const char *member);

public Q_SLOTS:
    void cancel();
    void reset();
    void setMaximum(int maximum);
    void setMinimum(int minimum);
    void setRange(int minimum, int maximum);
    void setValue(int progress);
    void setLabelText(const QString &text);
    void setCancelButtonText(const QString &text);
    void setMinimumDuration(int ms);

Q_SIGNALS:
    void canceled();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void changeEvent(QEvent *event) override;
    void showEvent(QShowEvent *event) override;

protected Q_SLOTS:
    void forceShow();

private:
    Q_DISABLE_COPY(QProgressDialog)

    Q_PRIVATE_SLOT(d_func(), void _q_disconnectOnClose())
};

#endif // QT_NO_PROGRESSDIALOG

QT_END_NAMESPACE

QT_END_HEADER

#endif // QPROGRESSDIALOG_H
