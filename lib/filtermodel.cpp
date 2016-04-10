#include "filtermodel.h"

#include "abstractitemmodel.h"

FilterModel::FilterModel(QObject* parent) : QSortFilterProxyModel(parent)
{
    connect(this, &FilterModel::modelReset, this, &FilterModel::countChanged);
    connect(this, &FilterModel::rowsInserted, [this](const QModelIndex&, int, int) {
        emit this->countChanged();
    });
    connect(this, &FilterModel::rowsRemoved, [this](const QModelIndex&, int, int) {
        emit this->countChanged();
    });
}

FilterModel::~FilterModel()
{
}

bool FilterModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_parent);
    bool result = true;
    if (m_filterFunction.isCallable())
    {
        QJSValueList args;
        args.append(source_row);
        result = m_filterFunction.call(args).toBool();
    }
    return result;
}

QJSValue FilterModel::filterFunction() const
{
    return m_filterFunction;
}

void FilterModel::setFilterFunction(const QJSValue& filterFunction)
{
    m_filterFunction = filterFunction;
    emit filterFunctionChanged();
    invalidateFilter();
}

void FilterModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    QAbstractItemModel *prev = this->sourceModel();
    if (prev) 
    {
        AbstractListModel *alm = dynamic_cast<AbstractListModel*>(prev);
        if (alm) 
        {
            disconnect(alm, &AbstractListModel::itemPropertiesChanged,
                       this, &FilterModel::invalidateFilter);        
        }
    }
    QSortFilterProxyModel::setSourceModel(sourceModel);
    AbstractListModel *alm = dynamic_cast<AbstractListModel*>(sourceModel);
    if (alm) 
    {
        connect(alm, &AbstractListModel::itemPropertiesChanged,
                   this, &FilterModel::invalidateFilter);        
    }
}
