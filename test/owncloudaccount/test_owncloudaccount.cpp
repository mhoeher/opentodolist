#include "sync/owncloudaccount.h"

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "../test-utils.h"

class OwnCloudAccountTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void login();
  void login_data();
  void cleanup() {}
  void cleanupTestCase() {}
};

void OwnCloudAccountTest::login()
{
    QFETCH(QUrl, url);

    WebDAVAccount account;
    account.setBaseUrl(url.toString());
    account.setUsername(url.userName());
    account.setPassword(url.password());
    QVERIFY(!account.online());
    QSignalSpy loggingInChanged(&account, &WebDAVAccount::loggingInChanged);
    account.login();
    QCOMPARE(loggingInChanged.count(), 1);
    QSignalSpy onlineChanged(&account, &WebDAVAccount::onlineChanged);
    QSignalSpy loginFinished(&account, &WebDAVAccount::loginFinished);
    QVERIFY(loginFinished.wait());
    QVERIFY(loginFinished.at(0).at(0).toBool()); // 'success' shall be true
    QCOMPARE(loggingInChanged.count(), 2);
    QCOMPARE(onlineChanged.count(), 1);
    QVERIFY(account.online());
}

void OwnCloudAccountTest::login_data()
{
    addWebDAVServerTestUrls<WebDAVAccount::OwnCloud>();
}

QTEST_MAIN(OwnCloudAccountTest)
#include "test_owncloudaccount.moc"
