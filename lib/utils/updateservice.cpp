#include "updateservice.h"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTimer>
#include <QVersionNumber>


Q_LOGGING_CATEGORY(updateService,
                   "net.rpdev.opentodolist.UpdateService",
                   QtDebugMsg)


UpdateService::UpdateService(QObject *parent) : QObject(parent)
{
#ifdef WITH_UPDATE_SERVICE
    auto updateTimer = new QTimer(this);
    updateTimer->setInterval(1000*60*60*24);
    updateTimer->setSingleShot(false);
    connect(updateTimer, &QTimer::timeout,
            this, &UpdateService::checkForUpdates);
    updateTimer->start();
    checkForUpdates();
#endif
}

void UpdateService::checkForUpdates()
{
    auto nam = new QNetworkAccessManager(this);
    QUrl updateUrl("https://gitlab.com/api/v4/projects/185664/repository/tags");
    QNetworkRequest req(updateUrl);
    auto reply = nam->get(req);
    if (reply) {
        connect(reply, static_cast<void(QNetworkReply::*)
                (QNetworkReply::NetworkError)>(&QNetworkReply::error),
                [=](QNetworkReply::NetworkError error) {
            qCWarning(updateService) << "Failed to get update information:"
                                     << error;
            reply->deleteLater();
            nam->deleteLater();
        });
        connect(reply, &QNetworkReply::finished, [=]() {
            qCDebug(updateService) << "Received tag list - going to check for "
                                      "most recent version";
            auto doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isArray()) {
                auto tags = doc.toVariant().toList();
                auto currentVersion = QVersionNumber::fromString(VERSION);
                auto latestVersion = currentVersion;
                QString latestTagName;
                for (auto tag : tags) {
                    auto tagName = tag.toMap().value("name", "").toString();
                    if (!tagName.isEmpty()) {
                        auto version = QVersionNumber::fromString(tagName);
                        if (version > currentVersion &&
                                version > latestVersion) {
                            latestTagName = tagName;
                            latestVersion = version;
                        }
                    }
                }
                if (!latestTagName.isEmpty()) {
                    auto updateUrl =
                            QString("https://gitlab.com/rpdev/"
                                    "opentodolist/tags/%1")
                            .arg(latestTagName);
                    qCDebug(updateService) << "Found more recent version:"
                                           << latestTagName
                                           << "dowloadable via"
                                           << updateUrl;
                    emit updateAvailable(latestTagName, QUrl(updateUrl));
                } else {
                    qCDebug(updateService) << "Already running latest version";
                }
            }
            reply->deleteLater();
            nam->deleteLater();
        });
    } else {
        delete nam;
    }
}
