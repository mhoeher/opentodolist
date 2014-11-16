#ifndef TASK_H
#define TASK_H

#include "core/opentodolistinterfaces.h"

#include <QObject>

namespace OpenTodoList {

namespace DataModel {

class Task : public QObject, public ITask
{
    Q_OBJECT
    Q_PROPERTY(int id READ id NOTIFY idChanged)
    Q_PROPERTY(bool hasId READ hasId NOTIFY idChanged)
    Q_PROPERTY(QUuid uuid READ uuid NOTIFY uuidChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
    Q_PROPERTY(double weight READ weight WRITE setWeight NOTIFY weightChanged)
    Q_PROPERTY(QVariantMap metaAttributes READ metaAttributes NOTIFY metaAttributesChanged)
    Q_PROPERTY(QUuid todoUuid READ todoUuid NOTIFY todoUuidChanged)

public:
    explicit Task(QObject *parent = 0);

    int id() const;
    void setId( int id );
    bool hasId() const;

    // ITask interface
    QUuid uuid() const override;
    void setUuid(const QUuid &uuid) override;
    QString title() const override;
    void setTitle(const QString &title) override;
    bool done() const override;
    void setDone(bool done) override;
    double weight() const override;
    void setWeight(double weight) override;
    QVariantMap metaAttributes() const override;
    void setMetaAttributes(const QVariantMap &metaAttributes) override;
    QUuid todoUuid() const override;
    void setTodoUuid( const QUuid &uuid ) override;

    QVariant toVariant() const;
    void fromVariant( const QVariant &task );

signals:

    void idChanged();
    void uuidChanged();
    void titleChanged();
    void doneChanged();
    void weightChanged();
    void metaAttributesChanged();
    void todoUuidChanged();

public slots:

private:

    int   m_id;
    bool m_hasId;
    QUuid m_uuid;
    QString m_title;
    bool m_done;
    double m_weigth;
    QVariantMap m_metaAttributes;
    QUuid m_todoUuid;


};

} // namespace DataModel
} // namespace OpenTodoList

#endif // TASK_H