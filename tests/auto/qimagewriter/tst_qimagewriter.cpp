/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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


#include <QtTest/QtTest>


#include <QDebug>
#include <QFile>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QLabel>
#include <QPainter>
#include <QSet>

#if defined(Q_OS_SYMBIAN)
# define SRCDIR ""
#endif
typedef QMap<QString, QString> QStringMap;
typedef QList<int> QIntList;
Q_DECLARE_METATYPE(QImage)
Q_DECLARE_METATYPE(QStringMap)
Q_DECLARE_METATYPE(QIntList)
Q_DECLARE_METATYPE(QImageWriter::ImageWriterError)
Q_DECLARE_METATYPE(QIODevice *)
Q_DECLARE_METATYPE(QImage::Format)

//TESTED_FILES=

class tst_QImageWriter : public QObject
{
    Q_OBJECT

public:
    tst_QImageWriter();
    virtual ~tst_QImageWriter();

public slots:
    void init();
    void cleanup();

private slots:
    void getSetCheck();
    void writeImage_data();
    void writeImage();
    void writeImage2_data();
    void writeImage2();
    void supportedFormats();

    void readWriteNonDestructive_data();
    void readWriteNonDestructive();

#if defined QTEST_HAVE_TIFF
    void largeTiff();
#endif

    void writeToInvalidDevice();

    void supportsOption_data();
    void supportsOption();

    void saveWithNoFormat_data();
    void saveWithNoFormat();

    void resolution_data();
    void resolution();

    void saveToTemporaryFile();
};
#ifdef Q_OS_SYMBIAN
static const QLatin1String prefix(SRCDIR "images/");
#else
static const QLatin1String prefix(SRCDIR "/images/");
#endif
static void initializePadding(QImage *image)
{
    int effectiveBytesPerLine = (image->width() * image->depth() + 7) / 8;
    int paddingBytes = image->bytesPerLine() - effectiveBytesPerLine;
    if (paddingBytes == 0)
        return;
    for (int y = 0; y < image->height(); ++y) {
        memset(image->scanLine(y) + effectiveBytesPerLine, 0, paddingBytes);
    }
}

// Testing get/set functions
void tst_QImageWriter::getSetCheck()
{
    QImageWriter obj1;
    // QIODevice * QImageWriter::device()
    // void QImageWriter::setDevice(QIODevice *)
    QFile *var1 = new QFile;
    obj1.setDevice(var1);

    QCOMPARE((QIODevice *) var1, obj1.device());
    // The class should possibly handle a 0-pointer as a device, since
    // there is a default contructor, so it's "handling" a 0 device by default.
    // For example: QMovie::setDevice(0) works just fine
    obj1.setDevice((QIODevice *)0);
    QCOMPARE((QIODevice *) 0, obj1.device());
    delete var1;

    // int QImageWriter::quality()
    // void QImageWriter::setQuality(int)
    obj1.setQuality(0);
    QCOMPARE(0, obj1.quality());
    obj1.setQuality(INT_MIN);
    QCOMPARE(INT_MIN, obj1.quality());
    obj1.setQuality(INT_MAX);
    QCOMPARE(INT_MAX, obj1.quality());

    // int QImageWriter::compression()
    // void QImageWriter::setCompression(int)
    obj1.setCompression(0);
    QCOMPARE(0, obj1.compression());
    obj1.setCompression(INT_MIN);
    QCOMPARE(INT_MIN, obj1.compression());
    obj1.setCompression(INT_MAX);
    QCOMPARE(INT_MAX, obj1.compression());

    // float QImageWriter::gamma()
    // void QImageWriter::setGamma(float)
    obj1.setGamma(0.0f);
    QCOMPARE(0.0f, obj1.gamma());
    obj1.setGamma(1.1f);
    QCOMPARE(1.1f, obj1.gamma());
}

tst_QImageWriter::tst_QImageWriter()
{
}

tst_QImageWriter::~tst_QImageWriter()
{
    QDir dir(prefix);
    QStringList filesToDelete = dir.entryList(QStringList() << "gen-*" , QDir::NoDotAndDotDot | QDir::Files);
    foreach( QString file, filesToDelete) {
        QFile::remove(dir.absoluteFilePath(file));
    }

}

void tst_QImageWriter::init()
{
}

void tst_QImageWriter::cleanup()
{
}

void tst_QImageWriter::writeImage_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("lossy");
    QTest::addColumn<QByteArray>("format");

    QTest::newRow("BMP: colorful") << QString("colorful.bmp") << false << QByteArray("bmp");
    QTest::newRow("BMP: font") << QString("font.bmp") << false << QByteArray("bmp");
    QTest::newRow("XPM: marble") << QString("marble.xpm") << false << QByteArray("xpm");
    QTest::newRow("PNG: kollada") << QString("kollada.png") << false << QByteArray("png");
    QTest::newRow("PPM: teapot") << QString("teapot.ppm") << false << QByteArray("ppm");
    QTest::newRow("PBM: ship63") << QString("ship63.pbm") << true << QByteArray("pbm");
    QTest::newRow("XBM: gnus") << QString("gnus.xbm") << false << QByteArray("xbm");
    QTest::newRow("JPEG: beavis") << QString("beavis.jpg") << true << QByteArray("jpeg");
#if defined QTEST_HAVE_TIFF
    QTest::newRow("TIFF: teapot") << QString("teapot.tiff") << false << QByteArray("tiff");
#endif
}

void tst_QImageWriter::writeImage()
{
    QFETCH(QString, fileName);
    QFETCH(bool, lossy);
    QFETCH(QByteArray, format);

    QImage image;
    {
        QImageReader reader(prefix + fileName);
        image = reader.read();
        QVERIFY2(!image.isNull(), qPrintable(reader.errorString()));
    }
    {
        QImageWriter writer(prefix + "gen-" + fileName, format);
        QVERIFY(writer.write(image));
    }

    {
        // Shouldn't be able to write to read-only file
        QFile sourceFile(prefix + "gen-" + fileName);
        QFile::Permissions permissions = sourceFile.permissions();
        QVERIFY(sourceFile.setPermissions(QFile::ReadOwner | QFile::ReadUser | QFile::ReadGroup | QFile::ReadOther));

        QImageWriter writer(prefix + "gen-" + fileName, format);
        QVERIFY(!writer.write(image));

        QVERIFY(sourceFile.setPermissions(permissions));
    }

    QImage image2;
    {
        QImageReader reader(prefix + "gen-" + fileName);
        image2 = reader.read();
        QVERIFY(!image2.isNull());
    }
    if (!lossy) {
        QCOMPARE(image, image2);
    } else {
        QCOMPARE(image.format(), image2.format());
        QCOMPARE(image.depth(), image2.depth());
    }
}

void tst_QImageWriter::writeImage2_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QByteArray>("format");
    QTest::addColumn<QImage>("image");

    const QStringList formats = QStringList() << "bmp" << "xpm" << "png"
                                              << "ppm"; //<< "jpeg";
    QImage image0(70, 70, QImage::Format_ARGB32);
    image0.fill(QColor(Qt::red).rgb());

    QImage::Format imgFormat = QImage::Format_Mono;
    while (imgFormat != QImage::NImageFormats) {
        QImage image = image0.convertToFormat(imgFormat);
        initializePadding(&image);
        foreach (const QString format, formats) {
            const QString fileName = QString("solidcolor_%1.%2").arg(imgFormat)
                                     .arg(format);
            QTest::newRow(fileName.toLatin1()) << fileName
                                               << format.toLatin1()
                                               << image;
        }
        imgFormat = QImage::Format(int(imgFormat) + 1);
    }
}

#if defined QTEST_HAVE_TIFF
void tst_QImageWriter::largeTiff()
{
#if !defined(Q_OS_WINCE) && !defined(Q_OS_SYMBIAN)
    QImage img(4096, 2048, QImage::Format_ARGB32);

    QPainter p(&img);
    img.fill(0x0);
    p.fillRect(0, 0, 4096, 2048, QBrush(Qt::CrossPattern));
    p.end();

    QByteArray array;
    QBuffer writeBuffer(&array);
    writeBuffer.open(QIODevice::WriteOnly);

    QImageWriter writer(&writeBuffer, "tiff");
    QVERIFY(writer.write(img));

    writeBuffer.close();

    QBuffer readBuffer(&array);
    readBuffer.open(QIODevice::ReadOnly);

    QImageReader reader(&readBuffer, "tiff");

    QImage img2 = reader.read();
    QVERIFY(!img2.isNull());

    QCOMPARE(img, img2);
#else
    QWARN("not tested on Symbian/WinCE");
#endif
}
#endif

/*
    Workaround for the equality operator for indexed formats
    (which fails if the colortables are different).

    Images must have the same format and size.
*/
static bool equalImageContents(const QImage &image1, const QImage &image2)
{
    switch (image1.format()) {
    case QImage::Format_Mono:
    case QImage::Format_Indexed8:
        for (int y = 0; y < image1.height(); ++y)
            for (int x = 0; x < image1.width(); ++x)
                if (image1.pixel(x, y) != image2.pixel(x, y))
                    return false;
        return true;
    default:
        return (image1 == image2);
    }
}

void tst_QImageWriter::writeImage2()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);
    QFETCH(QImage, image);

    //we reduce the scope of writer so that it closes the associated file
    // and QFile::remove can actually work
    {
        QImageWriter writer(fileName, format);
        QVERIFY(writer.write(image));
    }

    QImage written;

    //we reduce the scope of reader so that it closes the associated file
    // and QFile::remove can actually work
    {
        QImageReader reader(fileName, format);
        QVERIFY(reader.read(&written));
    }

    written = written.convertToFormat(image.format());
    if (!equalImageContents(written, image)) {
        qDebug() << "image" << image.format() << image.width()
                 << image.height() << image.depth()
                 << hex << image.pixel(0, 0);
        qDebug() << "written" << written.format() << written.width()
                 << written.height() << written.depth()
                 << hex << written.pixel(0, 0);
    }
    QVERIFY(equalImageContents(written, image));

    QVERIFY(QFile::remove(fileName));
}

void tst_QImageWriter::supportedFormats()
{
    QList<QByteArray> formats = QImageWriter::supportedImageFormats();
    QList<QByteArray> sortedFormats = formats;
    qSort(sortedFormats);

    // check that the list is sorted
    QCOMPARE(formats, sortedFormats);

    QSet<QByteArray> formatSet;
    foreach (QByteArray format, formats)
        formatSet << format;

    // check that the list does not contain duplicates
    QCOMPARE(formatSet.size(), formats.size());
}

void tst_QImageWriter::readWriteNonDestructive_data()
{
    QTest::addColumn<QImage::Format>("format");
    QTest::addColumn<QImage::Format>("expectedFormat");
    QTest::addColumn<bool>("grayscale");
    QTest::newRow("tiff mono") << QImage::Format_Mono << QImage::Format_Mono << false;
    QTest::newRow("tiff indexed") << QImage::Format_Indexed8 << QImage::Format_Indexed8 << false;
    QTest::newRow("tiff rgb32") << QImage::Format_ARGB32 << QImage::Format_ARGB32 << false;
    QTest::newRow("tiff grayscale") << QImage::Format_Indexed8 << QImage::Format_Indexed8 << true;
}

void tst_QImageWriter::readWriteNonDestructive()
{
    QFETCH(QImage::Format, format);
    QFETCH(QImage::Format, expectedFormat);
    QFETCH(bool, grayscale);
    QImage image = QImage(prefix + "colorful.bmp").convertToFormat(format);

    if (grayscale) {
        QVector<QRgb> colors;
        for (int i = 0; i < 256; ++i)
            colors << qRgb(i, i, i);
        image.setColorTable(colors);
    }

    QVERIFY(image.save(prefix + "gen-readWriteNonDestructive.tiff"));

    QImage image2 = QImage(prefix + "gen-readWriteNonDestructive.tiff");
    QImage::Format readFormat = image2.format();
    QCOMPARE(readFormat, expectedFormat);
    QCOMPARE(image, image2);
}

void tst_QImageWriter::writeToInvalidDevice()
{
    QLatin1String fileName("/these/directories/do/not/exist/001.png");
    {
        QImageWriter writer(fileName);
        QVERIFY(!writer.canWrite());
        QCOMPARE(writer.error(), QImageWriter::DeviceError);
    }
    {
        QImageWriter writer(fileName);
        writer.setFormat("png");
        QVERIFY(!writer.canWrite());
        QCOMPARE(writer.error(), QImageWriter::DeviceError);
    }
    {
        QImageWriter writer(fileName);
        QImage im(10, 10, QImage::Format_ARGB32);
        QVERIFY(!writer.write(im));
        QCOMPARE(writer.error(), QImageWriter::DeviceError);
    }
    {
        QImageWriter writer(fileName);
        writer.setFormat("png");
        QImage im(10, 10, QImage::Format_ARGB32);
        QVERIFY(!writer.write(im));
        QCOMPARE(writer.error(), QImageWriter::DeviceError);
    }
}

void tst_QImageWriter::supportsOption_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QIntList>("options");

    QTest::newRow("png") << QString("gen-black.png")
                         << (QIntList() << QImageIOHandler::Gamma
                              << QImageIOHandler::Description
                              << QImageIOHandler::Quality
                              << QImageIOHandler::Size);
#if defined QTEST_HAVE_TIFF
    QTest::newRow("tiff") << QString("gen-black.tiff")
                          << (QIntList() << QImageIOHandler::Size
                              << QImageIOHandler::CompressionRatio);
#endif
}

void tst_QImageWriter::supportsOption()
{
    QFETCH(QString, fileName);
    QFETCH(QIntList, options);

    QSet<QImageIOHandler::ImageOption> allOptions;
    allOptions << QImageIOHandler::Size
               << QImageIOHandler::ClipRect
               << QImageIOHandler::Description
               << QImageIOHandler::ScaledClipRect
               << QImageIOHandler::ScaledSize
               << QImageIOHandler::CompressionRatio
               << QImageIOHandler::Gamma
               << QImageIOHandler::Quality
               << QImageIOHandler::Name
               << QImageIOHandler::SubType
               << QImageIOHandler::IncrementalReading
               << QImageIOHandler::Endianness
               << QImageIOHandler::Animation
               << QImageIOHandler::BackgroundColor;

    QImageWriter writer(prefix + fileName);
    for (int i = 0; i < options.size(); ++i) {
        QVERIFY(writer.supportsOption(QImageIOHandler::ImageOption(options.at(i))));
        allOptions.remove(QImageIOHandler::ImageOption(options.at(i)));
    }

    foreach (QImageIOHandler::ImageOption option, allOptions)
        QVERIFY(!writer.supportsOption(option));
}

void tst_QImageWriter::saveWithNoFormat_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QByteArray>("format");
    QTest::addColumn<QImageWriter::ImageWriterError>("error");

    QTest::newRow("garble") << prefix + QString("gen-out.garble") << QByteArray("jpeg") << QImageWriter::UnsupportedFormatError;
    QTest::newRow("bmp") << prefix + QString("gen-out.bmp") << QByteArray("bmp") << QImageWriter::ImageWriterError(0);
    QTest::newRow("xbm") << prefix + QString("gen-out.xbm") << QByteArray("xbm") << QImageWriter::ImageWriterError(0);
    QTest::newRow("xpm") << prefix + QString("gen-out.xpm") << QByteArray("xpm") << QImageWriter::ImageWriterError(0);
    QTest::newRow("png") << prefix + QString("gen-out.png") << QByteArray("png") << QImageWriter::ImageWriterError(0);
    QTest::newRow("ppm") << prefix + QString("gen-out.ppm") << QByteArray("ppm") << QImageWriter::ImageWriterError(0);
    QTest::newRow("pbm") << prefix + QString("gen-out.pbm") << QByteArray("pbm") << QImageWriter::ImageWriterError(0);
#if defined QTEST_HAVE_TIFF
    QTest::newRow("tiff") << prefix + QString("gen-out.tiff") << QByteArray("tiff") << QImageWriter::ImageWriterError(0);
#endif
}

void tst_QImageWriter::saveWithNoFormat()
{
    QFETCH(QString, fileName);
    QFETCH(QByteArray, format);
    QFETCH(QImageWriter::ImageWriterError, error);

    QImage niceImage(64, 64, QImage::Format_ARGB32);
    memset(niceImage.bits(), 0, niceImage.byteCount());

    QImageWriter writer(fileName /* , 0 - no format! */);
    if (error != 0) {
        QVERIFY(!writer.write(niceImage));
        QCOMPARE(writer.error(), error);
        return;
    }

    QVERIFY2(writer.write(niceImage), qPrintable(writer.errorString()));

    QImageReader reader(fileName);
    QCOMPARE(reader.format(), format);

    QVERIFY(reader.canRead());

    QImage outImage = reader.read();
    QVERIFY2(!outImage.isNull(), qPrintable(reader.errorString()));
}

void tst_QImageWriter::resolution_data()
{
    QTest::addColumn<QString>("filename");
    QTest::addColumn<int>("expectedDotsPerMeterX");
    QTest::addColumn<int>("expectedDotsPerMeterY");
#if defined QTEST_HAVE_TIFF
    QTest::newRow("TIFF: 100 dpi") << ("image_100dpi.tif") << qRound(100 * (100 / 2.54)) << qRound(100 * (100 / 2.54));
    QTest::newRow("TIFF: 50 dpi") << ("image_50dpi.tif") << qRound(50 * (100 / 2.54)) << qRound(50 * (100 / 2.54));
    QTest::newRow("TIFF: 300 dot per meter") << ("image_300dpm.tif") << 300 << 300;
#endif
}

void tst_QImageWriter::resolution()
{
    QFETCH(QString, filename);
    QFETCH(int, expectedDotsPerMeterX);
    QFETCH(int, expectedDotsPerMeterY);

    QImage image(prefix + QLatin1String("colorful.bmp"));
    image.setDotsPerMeterX(expectedDotsPerMeterX);
    image.setDotsPerMeterY(expectedDotsPerMeterY);
    const QString generatedFilepath = prefix + "gen-" + filename;
    {
        QImageWriter writer(generatedFilepath);
        QVERIFY(writer.write(image));
    }
    QImageReader reader(generatedFilepath);
    const QImage generatedImage = reader.read();

    QCOMPARE(expectedDotsPerMeterX, generatedImage.dotsPerMeterX());
    QCOMPARE(expectedDotsPerMeterY, generatedImage.dotsPerMeterY());
}

void tst_QImageWriter::saveToTemporaryFile()
{
    QImage image(prefix + "kollada.png");
    QVERIFY(!image.isNull());

    {
        // 1) Via QImageWriter's API, with a standard temp file name
        QTemporaryFile file;
        QVERIFY(file.open());
        QImageWriter writer(&file, "PNG");
        if (writer.canWrite())
            QVERIFY(writer.write(image));
        else
            qWarning() << file.errorString();
#if defined(Q_OS_WINCE)
        file.reset();
#endif
        QCOMPARE(QImage(writer.fileName()), image);
    }
    {
        // 2) Via QImage's API, with a standard temp file name
        QTemporaryFile file;
        QVERIFY(file.open());
        QVERIFY(image.save(&file, "PNG"));
        file.reset();
        QImage tmp;
        QVERIFY(tmp.load(&file, "PNG"));
        QCOMPARE(tmp, image);
    }
    {
        // 3) Via QImageWriter's API, with a named temp file
        QTemporaryFile file("tempXXXXXX");
        QVERIFY(file.open());
        QImageWriter writer(&file, "PNG");
        QVERIFY(writer.write(image));
#if defined(Q_OS_WINCE)
        file.reset();
#endif
        QCOMPARE(QImage(writer.fileName()), image);
    }
    {
        // 4) Via QImage's API, with a named temp file
        QTemporaryFile file("tempXXXXXX");
        QVERIFY(file.open());
        QVERIFY(image.save(&file, "PNG"));
        file.reset();
        QImage tmp;
        QVERIFY(tmp.load(&file, "PNG"));
        QCOMPARE(tmp, image);
    }
}

QTEST_MAIN(tst_QImageWriter)
#include "tst_qimagewriter.moc"
