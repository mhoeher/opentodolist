#ifndef ABSTRACTITEMMODEL_H
#define ABSTRACTITEMMODEL_H

#include <QObject>
#include <QAbstractListModel>

class AbstractListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AbstractListModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}
    virtual ~AbstractListModel() {}
    
signals:
    void itemPropertiesChanged();
    
};

#endif // ABSTRACTITEMMODEL_H
