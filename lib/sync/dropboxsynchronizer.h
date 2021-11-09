#ifndef DROPBOXSYNCHRONIZER_H
#define DROPBOXSYNCHRONIZER_H

#include <QObject>

#include "synchronizer.h"

class DropboxSynchronizer : public Synchronizer
{
    Q_OBJECT
public:
    explicit DropboxSynchronizer(QObject* parent = nullptr);

signals:

private:
    QString m_refreshToken;

    // Synchronizer interface
public:
    void validate() override;
    void synchronize() override;
    void stopSync() override;
    void findExistingLibraries() override;
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;
    void setAccount(Account* account) override;
};

#endif // DROPBOXSYNCHRONIZER_H
