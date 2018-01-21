#include "models/itemsmodel.h"

#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "datamodel/note.h"

class ItemsModelTest : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init();
  void testAddItems();
  void testDeleteItems();
  void cleanup();
  void cleanupTestCase() {}

private:

  ItemContainer     *m_container;
  ItemsModel        *m_model;
};




void ItemsModelTest::init()
{
    m_container = new ItemContainer();
    m_model = new ItemsModel();
    m_model->setContainer(m_container);
}

void ItemsModelTest::testAddItems()
{
    NotePtr note1(new Note());
    NotePtr note2(new Note());
    QSignalSpy rowsInserted(m_model, &ItemsModel::rowsInserted);
    m_container->addItem(note1);
    QVERIFY(rowsInserted.wait(1000));
    m_container->addItem(note2);
    QVERIFY(rowsInserted.wait(1000));
    QCOMPARE(rowsInserted.at(0).at(1).toInt(), 0);
    QCOMPARE(rowsInserted.at(1).at(1).toInt(), 1);
}

void ItemsModelTest::testDeleteItems()
{
    NotePtr note1(new Note());
    NotePtr note2(new Note());
    NotePtr note3(new Note());

    QSignalSpy rowsInserted(m_model, &ItemsModel::rowsInserted);
    m_container->addItem(note1);
    QVERIFY(rowsInserted.wait(1000));
    m_container->addItem(note2);
    QVERIFY(rowsInserted.wait(1000));
    m_container->addItem(note3);
    QVERIFY(rowsInserted.wait(1000));

    QSignalSpy rowsRemoved(m_model, &ItemsModel::rowsRemoved);
    m_container->deleteItem(note2);
    QVERIFY(rowsRemoved.wait(1000));
    m_container->deleteItem(note1);
    QVERIFY(rowsRemoved.wait(1000));
    m_container->deleteItem(note3);
    QVERIFY(rowsRemoved.wait(1000));
    QCOMPARE(rowsRemoved.at(0).at(1).toInt(), 1);
    QCOMPARE(rowsRemoved.at(1).at(1).toInt(), 0);
    QCOMPARE(rowsRemoved.at(2).at(1).toInt(), 0);
}

void ItemsModelTest::cleanup()
{
    delete m_model;
    delete m_container;
}

QTEST_MAIN(ItemsModelTest)
#include "test_itemsmodel.moc"
