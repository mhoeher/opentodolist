#ifndef ITEMSSORTFILTERMODEL_H
#define ITEMSSORTFILTERMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>
#include <QJSValue>

class ItemsSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QJSValue filterFunction READ filterFunction WRITE setFilterFunction NOTIFY filterFunctionChanged)
    Q_PROPERTY(QJSValue sortFunction READ sortFunction WRITE setSortFunction
               NOTIFY sortFunctionChanged)
public:
    explicit ItemsSortFilterModel(QObject *parent = 0);

    QJSValue filterFunction() const;
    void setFilterFunction(QJSValue filterFunction);

    QJSValue sortFunction() const;
    void setSortFunction(QJSValue sortFunction);

signals:

    void filterFunctionChanged();
    void sortFunctionChanged();

public slots:

private:

    QJSValue m_filterFunction;
    QJSValue m_sortFunction;

};

#endif // ITEMSSORTFILTERMODEL_H
