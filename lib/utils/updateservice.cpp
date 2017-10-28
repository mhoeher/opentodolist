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
    checkForUpdates();
}

void UpdateService::checkForUpdates()
{
    auto nam = new QNetworkAccessManager(this);
    QUrl updateUrl("https://api.github.com/repos/mhoeher/opentodolist/releases/latest");
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
            qCDebug(updateService) << "Received reply, checking...";
            auto doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isObject()) {
                auto info = doc.toVariant().toMap();
                if (info.contains("tag_name")) {
                    auto tagName = info.value("tag_name").toString();
                    qDebug(updateService) << "Latest release:" << tagName;
                    auto currentVersion = QVersionNumber::fromString(VERSION);
                    auto latestVersion = QVersionNumber::fromString(tagName);
                    if (latestVersion > currentVersion) {
                        emit updateAvailable(
                                    tagName,
                                    QUrl(info.value("html_url").toString()));
                    } else {
                        qCDebug(updateService)
                                << "Already running latest version";
                    }
                }
            }
            reply->deleteLater();
            nam->deleteLater();
        });
    } else {
        delete nam;
    }
}
