#ifndef TODOLIST_H
#define TODOLIST_H

#include "toplevelitem.h"

#include <QObject>


class TodoList : public TopLevelItem
{
    Q_OBJECT
public:

    explicit TodoList(QObject* parent = nullptr);
    explicit TodoList(const QString &filename, QObject *parent = 0);
    virtual ~TodoList();

signals:

public slots:

protected:

private:

private slots:

};

#endif // TODOLIST_H
