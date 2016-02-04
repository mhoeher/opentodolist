#include "image.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>

class ImageTest : public QObject
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void testPersistence();
  void cleanupTestCase() {}
  
};

void ImageTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto image = new Image(dir.path());
  Q_CHECK_PTR(image);
  delete image;
}

void ImageTest::testPersistence()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Failed to create temporary directory.");
  auto image = new Image(dir.path());
  image->setImage("test.png");
  delete image;
  image = new Image(dir.path());
  QCOMPARE(image->image(), QString("test.png"));
  delete image;
}

QTEST_MAIN(ImageTest)
#include "test_image.moc"
