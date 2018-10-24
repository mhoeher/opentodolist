#ifndef GETITEMQUERY_H
#define GETITEMQUERY_H

#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"


/**
 * @brief Load a single item from the cache.
 */
class GetItemQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit GetItemQuery(QObject *parent = nullptr);

    QUuid uid() const;
    void setUid(const QUuid &uid);


signals:

    void itemLoaded(const QVariant &data);

public slots:

protected:
    // ItemsQuery interface
    void run() override;

private:

    QUuid m_uid;

};

#endif // GETITEMQUERY_H
