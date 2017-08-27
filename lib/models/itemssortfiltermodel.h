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
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit ItemsSortFilterModel(QObject *parent = 0);

    QJSValue filterFunction() const;
    void setFilterFunction(QJSValue filterFunction);

    QJSValue sortFunction() const;
    void setSortFunction(QJSValue sortFunction);

    int count() const;

signals:

    void filterFunctionChanged();
    void sortFunctionChanged();
    void countChanged();

public slots:

protected:
    // QSortFilterProxyModel interface
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

private:

    mutable QJSValue m_filterFunction;
    mutable QJSValue m_sortFunction;


};

#endif // ITEMSSORTFILTERMODEL_H
