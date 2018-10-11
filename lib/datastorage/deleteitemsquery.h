#ifndef DELETEITEMSQUERY_H
#define DELETEITEMSQUERY_H

#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

class Item;
class Library;

class DeleteItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit DeleteItemsQuery(QObject *parent = nullptr);

    void deleteItem(const Item *item);
    void deleteLibrary(const Library *library);

signals:

    /**
     * @brief The item and all of its children have been deleted.
     *
     * This signal is emitted to indicate that the deletion of the item and
     * all of its children has finished.
     */
    void itemDeleted();

private:

    struct ItemToDelete {
        QUuid uid;
        bool isLibrary;
    };

    QList<ItemToDelete> m_itemsToDelete;

    // ItemsQuery interface
protected:
    void run() override;
};

#endif // DELETEITEMSQUERY_H
