#include "notepage.h"

#include <QDir>
#include <QObject>
#include <QTemporaryDir>
#include <QTest>
#include <QSignalSpy>

class NotePageTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void init() {}
    void persistence();
    void cleanup() {}
    void cleanupTestCase() {}
};

void NotePageTest::persistence()
{
    QTemporaryDir tmpDir;
    QString filename;
    auto noteUid = QUuid::createUuid();
    {
        NotePage page(QDir(tmpDir.path()));
        QSignalSpy spy(&page, &NotePage::changed);
        page.setNotes("Hello World!");
        QCOMPARE(spy.count(), 1);
        page.setNoteUid(noteUid);
        QCOMPARE(spy.count(), 2);
        QVERIFY(page.save());
        filename = page.filename();
    }
    {
        NotePage page(filename);
        QVERIFY(page.load());
        QCOMPARE(page.notes(), "Hello World!");
        QCOMPARE(page.noteUid(), noteUid);
    }
}

QTEST_MAIN(NotePageTest)
#include "test_notepage.moc"
