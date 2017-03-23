#include "itemssortfiltermodel.h"

ItemsSortFilterModel::ItemsSortFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_filterFunction(),
    m_sortFunction()
{
}

QJSValue ItemsSortFilterModel::filterFunction() const
{
    return m_filterFunction;
}

void ItemsSortFilterModel::setFilterFunction(QJSValue filter)
{
    if (filter.isCallable()) {
        m_filterFunction = filter;
        invalidateFilter();
        emit filterFunctionChanged();
    }
}

QJSValue ItemsSortFilterModel::sortFunction() const
{
    return m_sortFunction;
}

void ItemsSortFilterModel::setSortFunction(QJSValue sort)
{
    if (sort.isCallable()) {
        m_sortFunction = sort;
        emit sortFunctionChanged();
    }
}
