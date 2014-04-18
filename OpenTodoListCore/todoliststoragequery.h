#ifndef TODOLISTSTORAGEQUERY_H
#define TODOLISTSTORAGEQUERY_H

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

signals:

public slots:

};

#endif // TODOLISTSTORAGEQUERY_H
