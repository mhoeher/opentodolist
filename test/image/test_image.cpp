#include "datamodel/image.h"

#include <QObject>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

class ImageTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init();
  void testProperties();
  void testPersistence();
  void testSaveLoad();
  void cleanup();
  void cleanupTestCase() {}

private:

  QTemporaryDir *m_dir;

};



void ImageTest::init()
{
    m_dir = new QTemporaryDir();
}

void ImageTest::testProperties()
{
    Image item;
    QSignalSpy imageChanged(&item, &Image::imageChanged);

    item.setImage("hello.bmp");

    QCOMPARE(imageChanged.count(), 1);

    QCOMPARE(item.image(), QString("hello.bmp"));
}

void ImageTest::testPersistence()
{
    Image item, anotherItem;
    QSignalSpy imageChanged(&anotherItem, &Image::imageChanged);

    item.setImage("hello.bmp");

    anotherItem.fromVariant(item.toVariant());

    QCOMPARE(imageChanged.count(), 1);

    QCOMPARE(anotherItem.image(), item.image());
}

void ImageTest::testSaveLoad()
{
    Image item(QDir(m_dir->path()));
    QSignalSpy imageChanged(&item, &Image::imageChanged);
    item.setImage(SRCDIR "/test_image.cpp");
    QVERIFY(item.validImage());
    QVERIFY(item.save());

    Image anotherItem(item.filename());
    anotherItem.load();
    QCOMPARE(anotherItem.image(), item.image());
    QVERIFY(anotherItem.validImage());
}

void ImageTest::cleanup()
{
    delete m_dir;
}

QTEST_MAIN(ImageTest)
#include "test_image.moc"
