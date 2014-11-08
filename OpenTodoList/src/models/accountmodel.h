#ifndef OPENTODOLIST_MODELS_ACCOUNTMODEL_H
#define OPENTODOLIST_MODELS_ACCOUNTMODEL_H

#include "datamodel/account.h"

#include "database/database.h"
#include "database/queries/readaccount.h"

#include <QAbstractListModel>
#include <QList>

namespace OpenTodoList {
namespace Models {

using namespace DataModel;
using namespace DataBase;

class AccountModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(OpenTodoList::DataBase::Database* database READ database WRITE setDatabase NOTIFY databaseChanged)
public:
    explicit AccountModel(QObject *parent = 0);

    Database *database() const;
    void setDatabase(Database *database);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

signals:

    void databaseChanged();

public slots:

    void refresh();

private:

    Database           *m_database;
    QList<Account*>     m_accounts;

private slots:

    void addAccount( const QVariant &account );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_ACCOUNTMODEL_H
