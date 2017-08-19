#include "sync/webdavsynchronizer.h"
#include "sync/genericdavsynchronizer.h"
#include "sync/nextcloudsynchronizer.h"

#include <QObject>
#include <QObjectList>
#include <QSignalSpy>
#include <QTest>
#include <QUuid>

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
  void validate();
  void validate_data();
  void createDirectory();
  void createDirectory_data();
  void cleanup() {}
  void cleanupTestCase() {}

  void createDavClients();
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

void WebDAVSynchronizerTest::validate()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    Q_CHECK_PTR(davClient);
    QCOMPARE(davClient->valid(), false);
    QCOMPARE(davClient->validating(), false);
    QSignalSpy validatingChanged(davClient, &Synchronizer::validatingChanged);
    QSignalSpy validChanged(davClient, &Synchronizer::validChanged);
    davClient->validate();
    QVERIFY(validatingChanged.wait(30000));
    QCOMPARE(validatingChanged.count(), 2);
    QCOMPARE(validChanged.count(), 1);
    QCOMPARE(davClient->valid(), true);
    QCOMPARE(davClient->validating(), false);
}

void WebDAVSynchronizerTest::validate_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::createDirectory()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    Q_CHECK_PTR(davClient);
    QCOMPARE(davClient->creatingDirectory(), false);
    QSignalSpy creatingDirectoryChanged(davClient, &Synchronizer::creatingDirectoryChanged);
    QSignalSpy directoryCreated(davClient, &Synchronizer::directoryCreated);
    auto dirName = QUuid::createUuid().toString();
    davClient->createDirectory(dirName);
    QCOMPARE(davClient->creatingDirectory(), true);
    QVERIFY(creatingDirectoryChanged.wait(30000));
    QCOMPARE(creatingDirectoryChanged.count(), 2);
    QCOMPARE(directoryCreated.count(), 1);
    QCOMPARE(directoryCreated.at(0).at(0).toString(), dirName);
    QCOMPARE(directoryCreated.at(0).at(0).toBool(), true);
}

void WebDAVSynchronizerTest::createDirectory_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::createDavClients()
{
    QTest::addColumn<QObject*>("client");

#ifdef NEXTCLOUD_URL
    {
        auto client = new NextCloudSynchronizer();
        client->setUrl(QUrl(NEXTCLOUD_URL));
        client->setUsername(NEXTCLOUD_USER);
        client->setPassword(NEXTCLOUD_PASSWORD);
        if (client->url().scheme() == "http") {
            client->setDisableCertificateCheck(true);
        }
        QTest::newRow("NextCloud") << static_cast<QObject*>(client);
    }
#endif
}

QTEST_MAIN(WebDAVSynchronizerTest)
#include "test_webdavsynchronizer.moc"
