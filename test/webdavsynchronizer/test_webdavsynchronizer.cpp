#define WEBDAV_SYNCHRONIZER_TEST

#include "sync/webdavsynchronizer.h"
#include "sync/genericdavsynchronizer.h"
#include "sync/nextcloudsynchronizer.h"

#include <QObject>
#include <QObjectList>
#include <QRegularExpression>
#include <QSignalSpy>
#include <QTemporaryDir>
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
    void mkdir();
    void mkdir_data();
    void upload();
    void upload_data();
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

void WebDAVSynchronizerTest::mkdir()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
}

void WebDAVSynchronizerTest::mkdir_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::upload()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    QTemporaryDir dir;
    {
        QFile file(dir.path() + "/sample.txt");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Hello World\n");
        file.close();
    }
    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
    davClient->setDirectory(dir.path());
    davClient->setRemoteDirectory(dirName);
    QVERIFY(davClient->upload("sample.txt"));

    {
        QFile file(dir.path() + "/sample.txt");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Changed hello World\n");
        file.close();
    }
    QVERIFY(davClient->upload("sample.txt"));

    QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Failed to open.*for reading"));
    QVERIFY(!davClient->upload("sample2.txt"));
}

void WebDAVSynchronizerTest::upload_data()
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
