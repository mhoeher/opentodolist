#ifndef FILTERMODEL_H
#define FILTERMODEL_H

#include <QSortFilterProxyModel>
#include <QJSValue>

class FilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QJSValue filterFunction READ filterFunction WRITE setFilterFunction NOTIFY filterFunctionChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    explicit FilterModel(QObject *parent = nullptr);
    virtual ~FilterModel();
    
    // QSortFilterProxyModel interface
    QJSValue filterFunction() const;
    void setFilterFunction(const QJSValue& filterFunction);
    
    /**
       @brief Returns the number of items in the model.
     */
    int count() const { return rowCount(QModelIndex()); }
    
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    
public slots:
    
    void resetFilter();
    
signals:
    
    /**
       @brief The filter function has changed.
     */
    void filterFunctionChanged();
    
    /**
       @brief The number of items in the model has changed.
     */
    void countChanged();
    
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    
private:
    mutable QJSValue m_filterFunction;
};

#endif // FILTERMODEL_H
