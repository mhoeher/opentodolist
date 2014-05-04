#ifndef TODOLISTSTORAGEQUERY_H
#define TODOLISTSTORAGEQUERY_H

#include "opentodolistinterfaces.h"

#include <QObject>
#include <QVariantMap>

class TodoListStorageQuery : public QObject
{
    Q_OBJECT
public:
    explicit TodoListStorageQuery(QObject *parent = 0);
    virtual ~TodoListStorageQuery();

    virtual void beginRun();
    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );
    virtual void endRun();

    static TodoListStruct todoListFromRecord( const QVariantMap &record );
    static TodoStruct todoFromRecord( const QVariantMap &record );

signals:

public slots:

};

/**
   @brief A simple query for getting todo lists by their ID
 */
class TodoListByIdQuery : public TodoListStorageQuery
{
    Q_OBJECT
public:
    explicit TodoListByIdQuery( const QString &backend, const QString &todoListId, QObject* parent = 0 );
    virtual ~TodoListByIdQuery();

    virtual bool query( QString &query, QVariantMap &args );
    virtual void recordAvailable( const QVariantMap &record );

signals:

    void todoListAvailable( const QString &backend, const TodoListStruct &list );

private:

    QString m_backend;
    QString m_todoListId;
};

#endif // TODOLISTSTORAGEQUERY_H
