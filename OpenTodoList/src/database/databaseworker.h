#ifndef TODOLISTSTORAGEWORKER_H
#define TODOLISTSTORAGEWORKER_H

#include "core/opentodolistinterfaces.h"

#include <QMutex>
#include <QQueue>
#include <QSqlDatabase>
#include <QTemporaryFile>

namespace OpenTodoList {

namespace DataBase {

class StorageQuery;
class DatabaseWorker;

/**
   @brief Executes tasks of the storage in a backgroung thread
 */
class DatabaseWorker : public QObject
{
    Q_OBJECT

public:

    explicit DatabaseWorker();
    virtual ~DatabaseWorker();

    void run( StorageQuery *query );

public slots:

    void init();

signals:

private:

    QSqlDatabase                    m_dataBase;
    QTemporaryFile                  m_dataBaseFile;
    bool                            m_initialized;
    QQueue< StorageQuery* >         m_queue;
    QMutex                          m_queueLock;

    void runSimpleQuery(const QString &query , const QString &errorMsg);

private slots:

    void next();
};

} /* DataBase */

} /* OpenTodoList */

#endif // TODOLISTSTORAGEWORKER_H
