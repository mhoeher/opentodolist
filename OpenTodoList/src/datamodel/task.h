#ifndef TASK_H
#define TASK_H

#include "core/opentodolistinterfaces.h"

#include <QObject>

namespace OpenTodoList {

namespace DataModel {

class Task : public QObject, public ITask
{
    Q_OBJECT
public:
    explicit Task(QObject *parent = 0);

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

signals:

    void titleChanged();
    void doneChanged();
    void weightChanged();
    void metaAttributesChanged();

public slots:

private:

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
