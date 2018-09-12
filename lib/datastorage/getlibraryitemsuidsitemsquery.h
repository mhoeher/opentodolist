#ifndef GETLIBRARYITEMSUIDSITEMSQUERY_H
#define GETLIBRARYITEMSUIDSITEMSQUERY_H

#include <QObject>
#include <QQueue>
#include <QUuid>
#include <QSet>

#include "datastorage/itemsquery.h"

class Library;

class GetLibraryItemsUIDsItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit GetLibraryItemsUIDsItemsQuery(QObject *parent = nullptr);

    void addLibrary(const Library *library);

signals:

    void uidsAvailable(QSet<QUuid> uids);

public slots:

private:

    QQueue<QUuid> m_itemQueue;

    // ItemsQuery interface
protected:
    void run() override;
};

#endif // GETLIBRARYITEMSUIDSITEMSQUERY_H
