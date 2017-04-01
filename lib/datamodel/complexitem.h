#ifndef COMPLEXITEM_H
#define COMPLEXITEM_H

#include "item.h"

#include <QDateTime>
#include <QObject>

/**
 * @brief Complex items.
 *
 * The ComplexItem class is used to model more complex items. This class introduces some additional
 * properties that are not required by the simpler Item class but that nevertheless are common to
 * most other item types.
 */
class ComplexItem : public Item
{
    Q_OBJECT

    Q_PROPERTY(QDateTime dueTo READ dueTo WRITE setDueTo NOTIFY dueToChanged)
    Q_PROPERTY(QString notes READ notes WRITE setNotes NOTIFY notesChanged)
public:

    explicit ComplexItem(QObject *parent = nullptr);
    explicit ComplexItem(const QString &filename, QObject *parent = nullptr);
    explicit ComplexItem(const QDir &dir, QObject* parent = nullptr);
    virtual ~ComplexItem();

    QDateTime dueTo() const;
    void setDueTo(const QDateTime &dueTo);

    QString notes();
    void setNotes(const QString &notes);

signals:

    void dueToChanged();
    void notesChanged();

public slots:

protected:

private:

    QDateTime m_dueTo;
    QString   m_notes;

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
};

typedef QSharedPointer<ComplexItem> ComplexItemPtr;

#endif // COMPLEXITEM_H
