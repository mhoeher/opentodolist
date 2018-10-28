#ifndef GETLIBRARYQUERY_H
#define GETLIBRARYQUERY_H

#include <QObject>
#include <QUuid>

#include "datastorage/itemsquery.h"

class GetLibraryQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit GetLibraryQuery(QObject *parent = nullptr);

    QUuid uid() const;
    void setUid(const QUuid &uid);

signals:

    void libraryLoaded(const QVariant &data);

public slots:

protected:
    // ItemsQuery interface
    void run() override;

private:

    QUuid m_uid;
};

#endif // GETLIBRARYQUERY_H
