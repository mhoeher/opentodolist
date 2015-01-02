#ifndef OPENTODOLIST_MODELS_ACCOUNTMODEL_H
#define OPENTODOLIST_MODELS_ACCOUNTMODEL_H

#include "datamodel/account.h"

#include "models/private/objectmodel.h"

#include "database/database.h"
#include "database/queries/readaccount.h"

#include <QAbstractListModel>
#include <QList>

namespace OpenTodoList {
namespace Models {

using namespace DataModel;
using namespace DataBase;
using namespace Models::Private;

class AccountModel : public ObjectModel
{
    Q_OBJECT
public:
    explicit AccountModel(QObject *parent = 0);

signals:


public slots:

    // ObjectModel interface
protected:
    void connectToDatabase();
    void disconnectFromDatabase();
    StorageQuery *createQuery() const;

private:

private slots:

    void addAccount( const QVariant &account );
    void removeAccount( const QVariant &account );

};

} // namespace Models
} // namespace OpenTodoList

#endif // OPENTODOLIST_MODELS_ACCOUNTMODEL_H
