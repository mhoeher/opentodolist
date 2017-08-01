#include "sync/webdavsynchronizer.h"
#include "sync/genericdavsynchronizer.h"

#include <QObject>
#include <QTest>
#include <QSignalSpy>

class WebDAVSynchronizerTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void remoteDirectory();
  void disableCertificateCheck();
  void username();
  void password();
  void cleanup() {}
  void cleanupTestCase() {}
};




void WebDAVSynchronizerTest::remoteDirectory()
{
    GenericDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::remoteDirectoryChanged);
    QCOMPARE(sync.remoteDirectory(), QString(""));
    sync.setRemoteDirectory("/foo/");
    QCOMPARE(sync.remoteDirectory(), QString("/foo/"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::disableCertificateCheck()
{
    GenericDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::disableCertificateCheckChanged);
    QVERIFY(!sync.disableCertificateCheck());
    sync.setDisableCertificateCheck(true);
    QVERIFY(sync.disableCertificateCheck());
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::username()
{
    GenericDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::usernameChanged);
    QCOMPARE(sync.username(), QString(""));
    sync.setUsername("sample.user");
    QCOMPARE(sync.username(), QString("sample.user"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::password()
{
    GenericDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::passwordChanged);
    QCOMPARE(sync.password(), QString(""));
    sync.setPassword("fooBarBas123!");
    QCOMPARE(sync.password(), QString("fooBarBas123!"));
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(WebDAVSynchronizerTest)
#include "test_webdavsynchronizer.moc"
