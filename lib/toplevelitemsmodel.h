#ifndef TOPLEVELITEMSMODEL_H
#define TOPLEVELITEMSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QPointer>

// Forward declarations:
class Library;
class TopLevelItem;

class TopLevelItemsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Library* library READ library WRITE setLibrary NOTIFY libraryChanged)
    
public:
    enum Roles 
    {
        ObjectRole = Qt::UserRole
    };
    
    Q_ENUM(Roles)

    TopLevelItemsModel(QObject *parent = nullptr);
    
    Library* library() const;
    void setLibrary(Library* library);
    
    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;
    
signals:
    
    void libraryChanged();
    
private:
    
    QPointer<Library> m_library;
    
private slots:
    
    void resetModel();
    void itemChanged(TopLevelItem *item);
    
    
};

#endif // TOPLEVELITEMSMODEL_H
