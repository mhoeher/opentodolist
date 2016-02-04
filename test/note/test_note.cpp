#include "note.h"

#include <QObject>
#include <QTemporaryDir>
#include <QTest>


class NoteTest : public QObject
{
  Q_OBJECT
  
private slots:
  
  void initTestCase() {}
  void testConstructor();
  void cleanupTestCase() {}
  
};


void NoteTest::testConstructor()
{
  QTemporaryDir dir;
  QVERIFY2(dir.isValid(), "Unable to create temporary directory.");  
  auto *note = new Note(dir.path());
  Q_CHECK_PTR(note);
  delete note;
}


QTEST_MAIN(NoteTest)
#include "test_note.moc"
