#define WEBDAV_SYNCHRONIZER_TEST
#include "sync/webdavsynchronizer.h"
#include "sync/webdavclient.h"

#include "sync/webdavsynchronizer.h"

#include "datamodel/library.h"
#include "datamodel/note.h"
#include "datamodel/image.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

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
    void synchronize();
    void synchronize_data();
    void findExistingEntries();
    void findExistingEntries_data();
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
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::remoteDirectoryChanged);
    QCOMPARE(sync.remoteDirectory(), QString(""));
    sync.setRemoteDirectory("/foo/");
    QCOMPARE(sync.remoteDirectory(), QString("/foo/"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::disableCertificateCheck()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::disableCertificateCheckChanged);
    QVERIFY(!sync.disableCertificateCheck());
    sync.setDisableCertificateCheck(true);
    QVERIFY(sync.disableCertificateCheck());
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::username()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::usernameChanged);
    QCOMPARE(sync.username(), QString(""));
    sync.setUsername("sample.user");
    QCOMPARE(sync.username(), QString("sample.user"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::password()
{
    WebDAVSynchronizer sync;
    QSignalSpy spy(&sync, &WebDAVSynchronizer::passwordChanged);
    QCOMPARE(sync.password(), QString(""));
    sync.setPassword("fooBarBas123!");
    QCOMPARE(sync.password(), QString("fooBarBas123!"));
    QCOMPARE(spy.count(), 1);
}

void WebDAVSynchronizerTest::mkpath()
{
    QCOMPARE(WebDAVClient::mkpath(""), QString(""));
    QCOMPARE(WebDAVClient::mkpath("/"), QString(""));
    QCOMPARE(WebDAVClient::mkpath(("./foo/bar/")), QString("foo/bar"));
    QCOMPARE(WebDAVClient::mkpath(("./foo/bar")), QString("foo/bar"));
    QCOMPARE(WebDAVClient::mkpath(("./foo//bar")), QString("foo/bar"));
}

void WebDAVSynchronizerTest::splitpath()
{
    QCOMPARE(WebDAVClient::splitpath(""),
             std::make_tuple(QString(""), QString("")));
    QCOMPARE(WebDAVClient::splitpath("foo"),
             std::make_tuple(QString(""), QString("foo")));
    QCOMPARE(WebDAVClient::splitpath("foo/bar"),
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
    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(davClient->createDAVClient(davClient)->mkdir(dirName));
}

void WebDAVSynchronizerTest::mkdir_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::upload()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dav = davClient->createDAVClient(davClient);
    QTemporaryDir dir;
    echoToFile("Hello World\n", dir.path() + "/sample.txt");

    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    dav->setDirectory(dir.path());
    dav->setRemoteDirectory(dirName);
    QVERIFY(dav->upload("sample.txt"));
    echoToFile("Changed hello World\n", dir.path() + "/sample.txt");
    QVERIFY(dav->upload("sample.txt"));

    QTest::ignoreMessage(QtWarningMsg, QRegularExpression("Failed to open.*for reading"));
    QVERIFY(!dav->upload("sample2.txt"));
}

void WebDAVSynchronizerTest::upload_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::entryList()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dav = davClient->createDAVClient(davClient);
    QTemporaryDir dir;
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    echoToFile("Hello World\n", dir.path() + "/sample2.txt");
    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    dav->setDirectory(dir.path());
    dav->setRemoteDirectory(dirName);
    QVERIFY(dav->upload("sample.txt"));
    QVERIFY(dav->upload("sample2.txt"));
    dav->mkdir("subdir");
    bool ok;
    auto entryList = dav->entryList(".", &ok);
    QVERIFY(ok);
    QCOMPARE(entryList.length(), 4);
    QSet<QString> expected = {".", "subdir", "sample.txt", "sample2.txt"};
    QSet<QString> got = {entryList[0].name, entryList[1].name, entryList[2].name, entryList[3].name};
    QCOMPARE(got, expected);
    for (auto entry : entryList) {
        if (entry.name == "." || entry.name == "subdir") {
            QCOMPARE(entry.type, WebDAVClient::Directory);
        } else {
            QCOMPARE(entry.type, WebDAVClient::File);
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
    auto dav = davClient->createDAVClient(davClient);
    QTemporaryDir dir;
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    dav->setDirectory(dir.path());
    dav->setRemoteDirectory(dirName);
    QVERIFY(dav->upload("sample.txt"));

    QTemporaryDir dir2;
    dav->setDirectory(dir2.path());
    QVERIFY(dav->download("sample.txt"));
    QCOMPARE(catFile(dir2.path() + "/sample.txt"),
             QByteArray("Hello World\n"));
    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression("Upload failed with code 404"));
    QVERIFY(!dav->download("sample2.txt"));
    dav->setDirectory(dir.path());
    echoToFile("Foo Bar Baz", dir.path() + "/sample.txt");
    QVERIFY(dav->upload("sample.txt"));
    dav->setDirectory(dir2.path());
    QVERIFY(dav->download("sample.txt"));
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
    auto dav = davClient->createDAVClient(davClient);
    QTemporaryDir dir;
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    dav->setDirectory(dir.path());
    dav->setRemoteDirectory(dirName);
    QVERIFY(dav->upload("sample.txt"));
    QVERIFY(dav->deleteEntry("sample.txt"));
    QTest::ignoreMessage(QtWarningMsg,
                         QRegularExpression("Deleting entry failed with code 404"));
    QVERIFY(!dav->deleteEntry("sample.txt"));
    QVERIFY(dav->upload("sample.txt"));
    dav->setRemoteDirectory("");
    QVERIFY(dav->deleteEntry(dirName));
    QVERIFY(dav->mkdir(dirName));
    QVERIFY(dav->deleteEntry(dirName));
}

void WebDAVSynchronizerTest::deleteEntry_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::etag()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dav = davClient->createDAVClient(davClient);
    QTemporaryDir dir;
    echoToFile("Hello World\n", dir.path() + "/sample.txt");
    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    dav->setDirectory(dir.path());
    dav->setRemoteDirectory(dirName);

    auto dirEtag = dav->etag("/");
    QVERIFY(dirEtag != "");

    QString uploadEtag;
    QVERIFY(dav->upload("sample.txt", &uploadEtag));
    QThread::sleep(1);
    auto dirEtag2 = dav->etag("/");
    QVERIFY(dirEtag2 != "");
    QVERIFY(dirEtag != dirEtag2);

    auto fileEtag = dav->etag("sample.txt");
    QVERIFY(fileEtag != "");
    QCOMPARE(fileEtag, uploadEtag);
    auto fileEtag_alt = dav->etag("/sample.txt");
    QVERIFY(fileEtag_alt != "");
    QVERIFY(fileEtag == fileEtag_alt);
    echoToFile("Changed hello World\n", dir.path() + "/sample.txt");
    QVERIFY(dav->upload("sample.txt", &uploadEtag));
    QThread::sleep(1);

    auto fileEtag2 = dav->etag("sample.txt");
    QVERIFY(fileEtag2 != "");
    QVERIFY(fileEtag != fileEtag2);
    QCOMPARE(fileEtag2, uploadEtag);

    auto dirEtag3 = dav->etag("/");
    QVERIFY(dirEtag3 != "");
    QVERIFY(dirEtag3 != dirEtag2);

    QTest::ignoreMessage(
                QtWarningMsg,
                QRegularExpression("QNetworkReply::NetworkError"
                                   "\\(ContentNotFoundError\\)"));
    QVERIFY(dav->etag("sample2.txt") == "");
    QTest::ignoreMessage(
                QtWarningMsg,
                QRegularExpression("QNetworkReply::NetworkError"
                                   "\\(ContentNotFoundError\\)"));
    QVERIFY(dav->etag("foo/bar/") == "");
}

void WebDAVSynchronizerTest::etag_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::syncDirectory()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dav = davClient->createDAVClient(davClient);

    QTemporaryDir dir1;
    QTemporaryDir dir2;

    echoToFile("Hello World\n", dir1.path() + "/sample.txt");
    QDir(dir1.path()).mkdir("foo");

    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    dav->setDirectory(dir1.path());
    dav->setRemoteDirectory(dirName);

    QVERIFY(dav->syncDirectory("/"));

    dav->setDirectory(dir2.path());
    QVERIFY(dav->syncDirectory("/"));
    QCOMPARE(catFile(dir2.path() + "/sample.txt"), QByteArray("Hello World\n"));
    QVERIFY(QDir(dir2.path() + "/foo").exists());

    QVERIFY(QFile::remove(dir2.path() + "/sample.txt"));
    QVERIFY(QDir(dir2.path()).rmdir("foo"));
    QVERIFY(dav->syncDirectory("/"));

    dav->setDirectory(dir1.path());
    QVERIFY(dav->syncDirectory("/"));
    QVERIFY(!QFile::exists(dir1.path() + "/sample.txt"));
    QVERIFY(!QDir(dir1.path()).exists("foo"));
}

void WebDAVSynchronizerTest::syncDirectory_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::synchronize()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dav = davClient->createDAVClient(davClient);

    QTemporaryDir dir1;
    QTemporaryDir dir2;

    Library library(dir1.path());
    library.save();
    library.addNote();
    library.addImage();
    auto todoList = library.addTodoList();
    auto todo = todoList->addTodo();
    todo->addTask();

    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    davClient->setDirectory(dir1.path());
    davClient->setRemoteDirectory(dirName);
    QList<bool> syncState;
    QVERIFY(!davClient->synchronizing());
    connect(davClient, &WebDAVSynchronizer::synchronizingChanged, [&]() {
        syncState << davClient->synchronizing();
    });
    davClient->synchronize();
    QVERIFY(!davClient->synchronizing());
    QCOMPARE(syncState.count(), 2);
    QVERIFY(syncState[0]);
    QVERIFY(!syncState[1]);

    davClient->setDirectory(dir2.path());
    davClient->synchronize();

    Library library2(dir2.path());
    QSignalSpy loadingFinished(&library2, &Library::loadingFinished);
    QVERIFY(library2.load());
    QVERIFY(loadingFinished.wait());
    QThread::sleep(1);
    QCOMPARE(library2.topLevelItems()->count(), 3);
    QCOMPARE(library2.todos()->count(), 1);
    QCOMPARE(library2.tasks()->count(), 1);

    // Sync without changes should be "invariant"
    davClient->synchronize();
    davClient->synchronize();
    davClient->synchronize();


    Library library3(dir2.path());
    QSignalSpy loadingFinished2(&library3, &Library::loadingFinished);
    QVERIFY(library3.load());
    QVERIFY(loadingFinished2.wait());
    QThread::sleep(1);
    QCOMPARE(library3.topLevelItems()->count(), 3);
    QCOMPARE(library3.todos()->count(), 1);
    QCOMPARE(library3.tasks()->count(), 1);
}

void WebDAVSynchronizerTest::synchronize_data()
{
    createDavClients();
}

void WebDAVSynchronizerTest::findExistingEntries()
{
    QFETCH(QObject*, client);
    auto davClient = static_cast<WebDAVSynchronizer*>(client);
    auto dav = davClient->createDAVClient(davClient);

    QTemporaryDir dir1;
    QTemporaryDir dir2;
    QList<QUuid> uids;

    {
        Library lib(dir1.path());
        lib.setName("foo");
        lib.save();
        uids.append(lib.uid());
    }
    {
        Library lib(dir2.path());
        lib.setName("bar");
        lib.save();
        uids.append(lib.uid());
    }

    auto dirName = QUuid::createUuid().toString() + "-" + __func__;
    QVERIFY(dav->mkdir(dirName));
    QVERIFY(dav->mkdir(dirName + "/lib1.otl"));
    QVERIFY(dav->mkdir(dirName + "/OpenTodoList"));
    QVERIFY(dav->mkdir(dirName + "/OpenTodoList/lib2.otl"));

    davClient->setDirectory(dir1.path());
    davClient->setRemoteDirectory(dirName + "/lib1.otl");
    davClient->synchronize();

    davClient->setDirectory(dir2.path());
    davClient->setRemoteDirectory(dirName + "/OpenTodoList/lib2.otl");
    davClient->synchronize();

    davClient->setRemoteDirectory(dirName);
    QSignalSpy spy(davClient, &WebDAVSynchronizer::existingLibrariesChanged);
    davClient->findExistingLibraries();
    QVERIFY(spy.wait());
    auto existingLibs = davClient->existingLibraries();
    QCOMPARE(existingLibs.length(), 2);
    SynchronizerExistingLibrary lib1 =
            existingLibs.at(0).value<SynchronizerExistingLibrary>();
    SynchronizerExistingLibrary lib2 =
            existingLibs.at(1).value<SynchronizerExistingLibrary>();
    QVERIFY(lib1.name() == "foo" || lib2.name() == "foo");
    QVERIFY(lib1.name() == "bar" || lib2.name() == "bar");
    QVERIFY(lib1.name() != lib2.name());
    QVERIFY(uids.contains(lib1.uid()));
    QVERIFY(uids.contains(lib2.uid()));
    QVERIFY(lib1.uid() != lib2.uid());
    for (auto lib : existingLibs) {
        auto l = lib.value<SynchronizerExistingLibrary>();
        if (l.name() == "foo") {
            QVERIFY(l.path() == "lib1.otl");
        } else {
            QVERIFY(l.path() == "OpenTodoList/lib2.otl");
        }
    }
}

void WebDAVSynchronizerTest::findExistingEntries_data()
{
    createDavClients();
}


#endif // TEST_AGAINST_SERVER


void WebDAVSynchronizerTest::createDavClients()
{
    QTest::addColumn<QObject*>("client");

#ifdef NEXTCLOUD_URL
    {
        auto client = new WebDAVSynchronizer();
        client->setServerType(WebDAVSynchronizer::NextCloud);
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
