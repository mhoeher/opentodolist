#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include <QLoggingCategory>
#include <QObject>

class UpdateService : public QObject
{
    Q_OBJECT
public:
    explicit UpdateService(QObject *parent = nullptr);

signals:

    /**
     * @brief An update is available.
     *
     * This signal is emitted to indicate that an update is available. The
     * @p version is the name of the most recent available update. The
     * @p url points to a web page where the new version can be downloaded
     * from.
     */
    void updateAvailable(const QString &version, const QUrl &url);

private slots:

    void checkForUpdates();
};


Q_DECLARE_LOGGING_CATEGORY(updateService)


#endif // UPDATESERVICE_H
