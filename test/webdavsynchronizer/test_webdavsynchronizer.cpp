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

#ifdef NEXTCLOUD_URL
#define TEST_AGAINST_SERVER
#endif

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

    void mkpath();
    void splitpath();

# ifdef TEST_AGAINST_SERVER
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
    void etag();
    void etag_data();
    void syncDirectory();
    void syncDirectory_data();
#endif // TEST_AGAINST_SERVER
    void cleanup() {}
    void cleanupTestCase() {}

private:
    void createDavClients();
    void echoToFile(const QString& text, const QString& filename);
    QByteArray catFile(const QString &filename);
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

void WebDAVSynchronizerTest::mkpath()
{
    QCOMPARE(WebDAVSynchronizer::mkpath(""), QString(""));
    QCOMPARE(WebDAVSynchronizer::mkpath("/"), QString(""));
    QCOMPARE(WebDAVSynchronizer::mkpath(("./foo/bar/")), QString("foo/bar"));
    QCOMPARE(WebDAVSynchronizer::mkpath(("./foo/bar")), QString("foo/bar"));
    QCOMPARE(WebDAVSynchronizer::mkpath(("./foo//bar")), QString("foo/bar"));
}

void WebDAVSynchronizerTest::splitpath()
{
    QCOMPARE(WebDAVSynchronizer::splitpath(""),
             std::make_tuple(QString(""), QString("")));
    QCOMPARE(WebDAVSynchronizer::splitpath("foo"),
             std::make_tuple(QString(""), QString("foo")));
    QCOMPARE(WebDAVSynchronizer::splitpath("foo/bar"),
             std::make_tuple(QString("foo"), QString("bar")));
}

# ifdef TEST_AGAINST_SERVER

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
    echoToFile("Hello World\n", dir.path() + "/sample.txt");

    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
    davClient->setDirectory(dir.path());
    davClient->setRemoteDirectory(dirName);
    QVERIFY(davClient->upload("sample.txt"));
    echoToFile("Changed hello World\n", dir.path() + "/sample.txt");
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
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    echoToFile("Hello World\n", dir.path() + "/sample2.txt");
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
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
    davClient->setDirectory(dir.path());
    davClient->setRemoteDirectory(dirName);
    QVERIFY(davClient->upload("sample.txt"));

    QTemporaryDir dir2;
    davClient->setDirectory(dir2.path());
    QVERIFY(davClient->download("sample.txt"));
    QCOMPARE(catFile(dir2.path() + "/sample.txt"),
             QByteArray("Hello World\n"));
    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression("Upload failed with code 404"));
    QVERIFY(!davClient->download("sample2.txt"));
    davClient->setDirectory(dir.path());
    echoToFile("Foo Bar Baz", dir.path() + "/sample.txt");
    QVERIFY(davClient->upload("sample.txt"));
    davClient->setDirectory(dir2.path());
    QVERIFY(davClient->download("sample.txt"));
    QCOMPARE(catFile(dir2.path() + "/sample.txt"),
             QByteArray("Foo Bar Baz"));
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
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
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

void WebDAVSynchronizerTest::etag()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    QTemporaryDir dir;
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
    davClient->setDirectory(dir.path());
    davClient->setRemoteDirectory(dirName);

    auto dirEtag = davClient->etag("/");
    QVERIFY(dirEtag != "");

    QString uploadEtag;
    QVERIFY(davClient->upload("sample.txt", &uploadEtag));
    QThread::sleep(1);
    auto dirEtag2 = davClient->etag("/");
    QVERIFY(dirEtag2 != "");
    QVERIFY(dirEtag != dirEtag2);

    auto fileEtag = davClient->etag("sample.txt");
    QVERIFY(fileEtag != "");
    QCOMPARE(fileEtag, uploadEtag);
    auto fileEtag_alt = davClient->etag("/sample.txt");
    QVERIFY(fileEtag_alt != "");
    QVERIFY(fileEtag == fileEtag_alt);
    echoToFile("Changed hello World\n", dir.path() + "/sample.txt");
    QVERIFY(davClient->upload("sample.txt", &uploadEtag));
    QThread::sleep(1);

    auto fileEtag2 = davClient->etag("sample.txt");
    QVERIFY(fileEtag2 != "");
    QVERIFY(fileEtag != fileEtag2);
    QCOMPARE(fileEtag2, uploadEtag);

    auto dirEtag3 = davClient->etag("/");
    QVERIFY(dirEtag3 != "");
    QVERIFY(dirEtag3 != dirEtag2);

    QTest::ignoreMessage(
                QtWarningMsg,
                QRegularExpression("QNetworkReply::NetworkError"
                                   "\\(ContentNotFoundError\\)"));
    QVERIFY(davClient->etag("sample2.txt") == "");
    QTest::ignoreMessage(
                QtWarningMsg,
                QRegularExpression("QNetworkReply::NetworkError"
                                   "\\(ContentNotFoundError\\)"));
    QVERIFY(davClient->etag("foo/bar/") == "");
}

void WebDAVSynchronizerTest::etag_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::syncDirectory()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);

    QTemporaryDir dir1;
    QTemporaryDir dir2;

    echoToFile("Hello World\n", dir1.path() + "/sample.txt");
    QDir(dir1.path()).mkdir("foo");

    auto dirName = QUuid::createUuid().toString();
    QVERIFY(davClient->mkdir(dirName));
    davClient->setDirectory(dir1.path());
    davClient->setRemoteDirectory(dirName);

    QVERIFY(davClient->syncDirectory("/"));
    davClient->setDirectory(dir2.path());
    QVERIFY(davClient->syncDirectory("/"));
    QCOMPARE(catFile(dir2.path() + "/sample.txt"), QByteArray("Hello World\n"));
    QVERIFY(QDir(dir2.path() + "/foo").exists());

    QVERIFY(QFile::remove(dir2.path() + "/sample.txt"));
    QVERIFY(QDir(dir1.path()).remove("foo"));
    QVERIFY(davClient->syncDirectory("/"));

    davClient->setDirectory(dir1.path());
    QVERIFY(davClient->syncDirectory("/"));
    QVERIFY(!QFile::exists(dir1.path() + "/sample.txt"));
    QVERIFY(!QFile::exists(dir1.path() + "/foo"));
}

void WebDAVSynchronizerTest::syncDirectory_data()
{
    createDavClients();
}

#endif // TEST_AGAINST_SERVER


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

void WebDAVSynchronizerTest::echoToFile(const QString& text,
                                        const QString& filename)
{
    QFile file(filename);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write(text.toUtf8());
    file.close();
}

QByteArray WebDAVSynchronizerTest::catFile(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    auto result = file.readAll();
    file.close();
    return result;
}

QTEST_MAIN(WebDAVSynchronizerTest)
#include "test_webdavsynchronizer.moc"
