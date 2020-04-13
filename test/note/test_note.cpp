#include "datamodel/note.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>

class NoteTest : public QObject
{
    Q_OBJECT

private slots:

    void initTestCase() {}
    void cleanupTestCase() {}
};

QTEST_MAIN(NoteTest)
#include "test_note.moc"
