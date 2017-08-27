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
#include <QSet>

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
    void entryList();
    void entryList_data();
    void download();
    void download_data();
    void deleteEntry();
    void deleteEntry_data();
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

void WebDAVSynchronizerTest::entryList()
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
    {
        QFile file(dir.path() + "/sample2.txt");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Hello World\n");
        file.close();
    }
    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
    davClient->setDirectory(dir.path());
    davClient->setRemoteDirectory(dirName);
    QVERIFY(davClient->upload("sample.txt"));
    QVERIFY(davClient->upload("sample2.txt"));
    davClient->mkdir(davClient->remoteDirectory() + "/subdir");
    bool ok;
    auto entryList = davClient->entryList(".", &ok);
    QVERIFY(ok);
    QCOMPARE(entryList.length(), 4);
    QSet<QString> expected = {".", "subdir", "sample.txt", "sample2.txt"};
    QSet<QString> got = {entryList[0].name, entryList[1].name, entryList[2].name, entryList[3].name};
    QCOMPARE(got, expected);
    for (auto entry : entryList) {
        if (entry.name == "." || entry.name == "subdir") {
            QCOMPARE(entry.type, WebDAVSynchronizer::Directory);
        } else {
            QCOMPARE(entry.type, WebDAVSynchronizer::File);
        }
        QVERIFY(entry.etag != "");
    }
}

void WebDAVSynchronizerTest::entryList_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::download()
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

    QTemporaryDir dir2;
    davClient->setDirectory(dir2.path());
    QVERIFY(davClient->download("sample.txt"));
    {
        QFile file(dir2.path() + "/sample.txt");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QCOMPARE(file.readAll(), QByteArray("Hello World\n"));
    }
    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression("Upload failed with code 404"));
    QVERIFY(!davClient->download("sample2.txt"));
    davClient->setDirectory(dir.path());
    {
        QFile file(dir.path() + "/sample.txt");
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("Foo Bar Baz");
        file.close();
    }
    QVERIFY(davClient->upload("sample.txt"));
    davClient->setDirectory(dir2.path());
    QVERIFY(davClient->download("sample.txt"));
    {
        QFile file(dir2.path() + "/sample.txt");
        QVERIFY(file.open(QIODevice::ReadOnly));
        QCOMPARE(file.readAll(), QByteArray("Foo Bar Baz"));
    }
}

void WebDAVSynchronizerTest::download_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::deleteEntry()
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
    QVERIFY(davClient->deleteEntry("sample.txt"));
    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression("Deleting entry failed with code 404"));
    QVERIFY(!davClient->deleteEntry("sample.txt"));
    QVERIFY(davClient->upload("sample.txt"));
    davClient->setRemoteDirectory("");
    QVERIFY(davClient->deleteEntry(dirName));
    QVERIFY(davClient->mkdir(dirName));
    QVERIFY(davClient->deleteEntry(dirName));
}

void WebDAVSynchronizerTest::deleteEntry_data()
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
