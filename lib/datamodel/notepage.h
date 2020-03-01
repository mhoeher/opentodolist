#ifndef NOTEPAGE_H
#define NOTEPAGE_H

#include <QObject>

#include "complexitem.h"

class NotePage : public ComplexItem
{
    Q_OBJECT
    Q_PROPERTY(QUuid noteUid READ noteUid WRITE setNoteUid NOTIFY noteUidChanged)
public:
    explicit NotePage(QObject *parent = nullptr);
    explicit NotePage(const QString &filename, QObject *parent = nullptr);
    explicit NotePage(const QDir &dir, QObject *parent = nullptr);
    ~NotePage() override;

    QUuid noteUid() const;
    void setNoteUid(const QUuid &noteUid);

    QUuid parentId() const override;

signals:

    void noteUidChanged();

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;

private:
    QUuid m_noteUid;
};

#endif // NOTEPAGE_H
