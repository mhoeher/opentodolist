#ifndef INSERTORUPDATEITEMSQUERY_H
#define INSERTORUPDATEITEMSQUERY_H

#include <QObject>
#include <QUuid>

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datastorage/itemsquery.h"

// Forward declaration:
class Item;

class InsertOrUpdateItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit InsertOrUpdateItemsQuery(QObject *parent = nullptr);

signals:

public slots:

    void add(Item *item);
    void add(Library* library);

protected:
    // ItemsQuery interface
    void run() override;

private:

    QList<ItemCacheEntry> m_itemEntries;
    QList<LibraryCacheEntry> m_libEntries;

};

#endif // INSERTORUPDATEITEMSQUERY_H
