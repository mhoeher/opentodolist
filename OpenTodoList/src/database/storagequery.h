#ifndef TODOLISTSTORAGEQUERY_H
#define TODOLISTSTORAGEQUERY_H

#include "core/opentodolistinterfaces.h"

#include <QObject>
#include <QQueue>
#include <QVariantMap>

namespace OpenTodoList {

namespace DataBase {

class DatabaseWorker;

class StorageQuery : public QObject
{
    Q_OBJECT
    friend class DatabaseWorker;
public:
    explicit StorageQuery(QObject *parent = 0);
    virtual ~StorageQuery();

    virtual void beginRun();
    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );
    virtual void endRun();
    virtual bool hasNext() const;

    static ITodoList* todoListFromRecord( const QVariantMap &record );
    static ITodo* todoFromRecord( const QVariantMap &record );

signals:

public slots:

protected:

    DatabaseWorker *worker() const;

private:

    DatabaseWorker *m_worker;

};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGEQUERY_H
