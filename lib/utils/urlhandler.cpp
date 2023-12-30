/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
 *
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "urlhandler.h"

#include <QDesktopServices>
#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
#    include <QJniEnvironment>
#    include <QJniObject>
#    include <QGuiApplication>
#    include <QCoreApplication>
#    include <QTimer>
#endif

#include "datamodel/item.h"
#include "datamodel/library.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.UrlHandler", QtDebugMsg);

const char* UrlHandler::Scheme = "opentodolist";

const char* UrlHandler::DropboxAuthHost = "dropbox.auth";
const char* UrlHandler::ItemHost = "open.item";
const char* UrlHandler::LibraryHost = "open.library";

const char* UrlHandler::DropboxAuthUrl = "opentodolist://dropbox.auth";

/**
 * @class UrlHandler
 * @brief Handle custom and deep links into the application.
 *
 * This class is used to handle custom application links with the scheme opentodolist://...
 *
 * These links can be used for various purposes:
 *
 * - Passing authentication information back, e.g. when loggin in using OAuth.
 * - Handling of deep links to app content.
 *
 * Usually, exactly one instance of this class shall be created per application. The most recently
 * created instance can be accessed via the instance() static method.
 */

/**
 * @brief The most recently created instance of the UrlHandler.
 */
QPointer<UrlHandler> UrlHandler::s_instance = nullptr;

/**
 * @brief Constructor.
 *
 * Creates a new instance of the UrlHandler. Usually, per app, only one such object is required.
 * Every time a new object of that class is created, the static instance() is updated to that one.
 */
UrlHandler::UrlHandler(QObject* parent) : QObject { parent }
{
    s_instance = this;
    qCDebug(log) << "UrlHandler has been created";
}

/**
 * @brief Initialize the URL handling.
 *
 * This sets up some required handling to get app links right depending on the concrete platform.
 */
void UrlHandler::initialize()
{
    QDesktopServices::setUrlHandler(Scheme, this, "handleUrl");
    setupAndroidAppLinkHandler();
    qCDebug(log) << "UrlHandler has been initialized";
}

/**
 * @brief The most recent instance of the handler that has been created..
 */
UrlHandler* UrlHandler::instance()
{
    return s_instance;
}

/**
 * @brief Create a link to the given @p item.
 */
QUrl UrlHandler::createDeepLink(Item& item)
{
    QUrl result;
    result.setScheme(Scheme);
    result.setHost(ItemHost);
    result.setPath("/" + item.uid().toString(QUuid::WithoutBraces));
    return result;
}

/**
 * @brief Create a deep link to the given @p library.
 */
QUrl UrlHandler::createDeepLink(Library& library)
{
    QUrl result;
    result.setScheme(Scheme);
    result.setHost(LibraryHost);
    result.setPath("/" + library.uid().toString(QUuid::WithoutBraces));
    return result;
}

/**
 * @brief Extract a UUID from a @p url.
 *
 * The assumption is that the UUID is the first part of the path of the URL. In this case, it
 * is parsed and returned. In case the URL is not compliant, an invalid UUID is returned instead.
 */
QUuid UrlHandler::uidFromUrl(const QUrl& url)
{
    auto path = url.path();
    auto parts = path.split("/", Qt::SkipEmptyParts);
    if (parts.length() >= 1) {
        return QUuid::fromString(parts.first());
    } else {
        return QUuid();
    }
}

/**
 * @brief Handle incoming URLs.
 *
 * This slot is invoked for every incoming @p url. By default, for any such URL, the urlReceived()
 * signal is emitted once and no further processing happens. Exceptions are the following:
 *
 * - If the host name equals "dropbox.auth", the dropboxAuthReceived() signal is emitted instead.
 * - If the host name equals "open.item", the itemLinkReceived() signal is emitted instead.
 * - If the host name equals "open.library", the libraryLinkReceived() signal is emitted instead.
 */
void UrlHandler::handleUrl(const QUrl& url)
{
    auto host = url.host();
    if (host == DropboxAuthHost) {
        qCDebug(log) << "Received Dropbox OAuth callback";
        emit dropboxAuthReceived(url);
    } else if (host == ItemHost) {
        auto uid = uidFromUrl(url);
        if (!uid.isNull()) {
            qCDebug(log) << "Received link" << url << "to item with uid" << uid;
            emit itemLinkReceived(url, uid);
        } else {
            qCWarning(log) << "Received invalid link to item" << url;
        }
    } else if (host == LibraryHost) {
        auto uid = uidFromUrl(url);
        if (!uid.isNull()) {
            qCDebug(log) << "Received link" << url << "to library with uid" << uid;
            emit libraryLinkReceived(url, uid);
        } else {
            qCWarning(log) << "Received invalid link to library" << url;
        }
    } else {
        qCDebug(log) << "Received custom link" << url;
        emit urlReceived(url);
    }
}

void UrlHandler::setupAndroidAppLinkHandler()
{
#ifdef Q_OS_ANDROID
    // On Android, the app specific URL is first received by the Java part and
    // cached there. We need to wait until the app becomes active again and then
    // get the URLs from the Java part.
    auto guiApp = qobject_cast<QGuiApplication*>(QCoreApplication::instance());
    if (guiApp) {
        qCDebug(log) << "App has been created - setting up handler of applicationStateChanged";
        connect(guiApp, &QGuiApplication::applicationStateChanged, this,
                [=](Qt::ApplicationState state) {
                    if (state == Qt::ApplicationActive) {
                        auto activity =
                                QJniObject(QNativeInterface::QAndroidApplication::context());
                        if (activity.isValid()) {
                            auto handleExceptions = [=]() {
                                QJniEnvironment env;
                                if (env->ExceptionCheck()) {
                                    qCWarning(log) << "An exception occurred during "
                                                      "interfacing with Java.";
                                    env->ExceptionDescribe();
                                    env->ExceptionClear();
                                    return true;
                                }
                                return false;
                            };

                            auto numPendingAppLinks =
                                    activity.callMethod<jint>("getPendingAppLinksCount");
                            if (handleExceptions()) {
                                return;
                            }

                            for (int i = 0; i < numPendingAppLinks; ++i) {
                                auto appLink = activity.callObjectMethod(
                                        "getPendingAppLink", "(I)Ljava/lang/String;", i);
                                if (handleExceptions()) {
                                    return;
                                }
                                qCDebug(log) << "Received app link:" << appLink.toString();
                                QDesktopServices::openUrl(QUrl(appLink.toString()));
                            }

                            activity.callMethod<void>("clearPendingAppLinks");
                            if (handleExceptions()) {
                                return;
                            }
                        }
                    }
                });
    }
#endif
}
