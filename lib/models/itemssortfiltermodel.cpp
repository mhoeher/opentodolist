#include "itemssortfiltermodel.h"

#include "itemsmodel.h"

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
    m_filterFunction = filter;
    invalidateFilter();
    emit filterFunctionChanged();
}

QJSValue ItemsSortFilterModel::sortFunction() const
{
    return m_sortFunction;
}

void ItemsSortFilterModel::setSortFunction(QJSValue sort)
{
    m_sortFunction = sort;
    emit sortFunctionChanged();
}

bool ItemsSortFilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_parent);
    bool result = true;
    if (m_filterFunction.isCallable()) {
        QJSValueList args;
        args.append(source_row);
        auto ret = m_filterFunction.call(args);
        if (ret.isBool()) {
            result = ret.toBool();
        }
    }
    return result;
}

bool ItemsSortFilterModel::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
{
    bool result = true;
    if (m_sortFunction.isCallable()) {
        QJSValueList args;
        args.append(source_left.row());
        args.append(source_right.row());
        auto ret = m_sortFunction.call(args);
        if (ret.isBool()) {
            result = ret.toBool();
        }
    }
    return result;
}
