#ifndef ITEMSSORTFILTERMODEL_H
#define ITEMSSORTFILTERMODEL_H

#include <QDateTime>
#include <QJSValue>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QUuid>

class Item;
class Task;
class Todo;

class ItemsSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit ItemsSortFilterModel(QObject *parent = nullptr);

    int count() const;

signals:

    void countChanged();

};

#endif // ITEMSSORTFILTERMODEL_H
