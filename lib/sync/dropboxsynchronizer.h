#ifndef DROPBOXSYNCHRONIZER_H
#define DROPBOXSYNCHRONIZER_H

#include <QObject>
#include <QString>
#include <QDateTime>

#include "synchronizer.h"

class DropboxSynchronizer : public Synchronizer
{
    Q_OBJECT

    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)

public:
    explicit DropboxSynchronizer(QObject* parent = nullptr);

    // Synchronizer interface
    void synchronize() override;
    void stopSync() override;
    void findExistingLibraries() override;
    QVariantMap toMap() const override;
    void fromMap(const QVariantMap& map) override;
    void setAccount(Account* account) override;

    const QString& accessToken() const;
    void setAccessToken(const QString& newAccessToken);

signals:

    void accessTokenChanged();

private:
    // For OAuth
    QString m_accessToken;
};

#endif // DROPBOXSYNCHRONIZER_H
