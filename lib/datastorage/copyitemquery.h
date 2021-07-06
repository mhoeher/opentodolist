#ifndef COPYITEMQUERY_H
#define COPYITEMQUERY_H

#include <QDir>
#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

class Library;
class Item;

class CopyItemQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit CopyItemQuery(QObject* parent = nullptr);

    void copyItem(Item* item, Library* targetLibrary, Item* parentItem = nullptr);

protected:
    // ItemsQuery interface
    void run() override;

private:
    QUuid m_sourceItemUid;
    QUuid m_targetLibraryUid;
    QDir m_targetPath;
    QUuid m_targetParentItemUid;
};

#endif // COPYITEMQUERY_H
