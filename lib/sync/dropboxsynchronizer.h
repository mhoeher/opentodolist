#ifndef DROPBOXSYNCHRONIZER_H
#define DROPBOXSYNCHRONIZER_H

#include <QObject>
#include <QString>
#include <QDateTime>

#include "synchronizer.h"

class DropboxSynchronizer : public Synchronizer
{
    Q_OBJECT
public:
    explicit DropboxSynchronizer(QObject* parent = nullptr);

    // Synchronizer interface
    void validate() override;
    void synchronize() override;
    void stopSync() override;
    void findExistingLibraries() override;
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;
    void setAccount(Account* account) override;
signals:

private:
    // For OAuth
    QString m_refreshToken;
    QString m_accessToken;
    QDateTime m_expirationAt;

    // For PKCE Code Flow
    QString m_codeVerifier;
};

#endif // DROPBOXSYNCHRONIZER_H
