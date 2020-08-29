#include "nextcloudloginflow.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QNetworkRequest>
#include <QUrlQuery>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.NextCloudLoginFlow", QtDebugMsg);

static bool isValidFlowKickoffResponse(const QJsonDocument& doc, QString& error)
{
    if (!doc.isObject()) {
        error = "Response should be an object";
        return false;
    }
    auto obj = doc.object();
    if (!obj.contains("login")) {
        error = "Response does not contain a login URL";
        return false;
    }
    if (!obj.value("login").isString()) {
        error = "Login URL is not a string";
        return false;
    }
    if (!obj.contains("poll")) {
        error = "Response does not contain flow polling information";
        return false;
    }
    if (!obj.value("poll").isObject()) {
        error = "Poll information is not an object";
        return false;
    }
    auto poll = obj.value("poll").toObject();
    if (!poll.contains("token")) {
        error = "Poll information is missing token";
        return false;
    }
    if (!poll.value("token").isString()) {
        error = "Poll token is not a string";
        return false;
    }
    if (!poll.contains("endpoint")) {
        error = "Poll endpoint is missing";
        return false;
    }
    if (!poll.value("endpoint").isString()) {
        error = "Poll endpoint is not a string";
        return false;
    }
    return true;
}

static bool isValidPollResultResponse(const QJsonDocument& doc, QString& error)
{
    if (!doc.isObject()) {
        error = "Poll result must be an object";
        return false;
    }
    auto obj = doc.object();
    if (!obj.contains("server")) {
        error = "Poll result must contain the server URL";
        return false;
    }
    if (!obj.contains("loginName")) {
        error = "Poll result must contain the login name";
        return false;
    }
    if (!obj.contains("appPassword")) {
        error = "Poll result must contain the app password";
        return false;
    }
    return true;
}

NextCloudLoginFlow::NextCloudLoginFlow(QObject* parent)
    : QObject(parent),
      m_networkAccessManager(nullptr),
      m_pollTimer(nullptr),
      m_token(),
      m_endpoint(),
      m_ignoreSslErrors(false)
{
}

/**
 * @brief Indicates if the login flow is currently running.
 */
bool NextCloudLoginFlow::flowRunning() const
{
    return m_networkAccessManager != nullptr;
}

/**
 * @brief Start the login flow.
 *
 * This starts the login flow on the NextCloud instance running at @p url.
 * Returns true if the flow could be started successfully or false otherwise (e.g. if a flow
 * is currently already ongoing).
 */
bool NextCloudLoginFlow::startLoginFlow(const QUrl& url)
{
    qCDebug(log) << "Started NextCloud login flow v2 against" << url;

    if (m_networkAccessManager != nullptr) {
        // Flow already runs - return immediately.
        qCWarning(log) << "A login flow is already running";
        return false;
    }

    m_networkAccessManager = new QNetworkAccessManager(this);
    m_networkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
    m_networkAccessManager->setAutoDeleteReplies(true);
    QNetworkRequest request;
    auto urlString = url.toString();
    if (!urlString.endsWith("/")) {
        urlString += "/";
    }
    auto loginUrl = urlString + "index.php/login/v2";
    request.setUrl(QUrl(loginUrl));
    request.setHeader(QNetworkRequest::UserAgentHeader, "OpenTodoList");
    auto reply = m_networkAccessManager->post(request, QByteArray());
    if (reply) {
        prepareReply(reply);
        connect(reply, &QNetworkReply::finished, this, &NextCloudLoginFlow::onFlowKickoffFinished);
        return true;
    } else {
        // Fatal, should probably not happen:
        qCWarning(log) << "Failed to kick off login flow v2";
        delete m_networkAccessManager;
        m_networkAccessManager = nullptr;
        return false;
    }
}

/**
 * @brief Shall SSL errors be ignored.
 *
 * If this property is set to true, SSL errors are ignored. This can be used e.g. when
 * running a server with a self signed certificate.
 *
 * By default, it is false, i.e. SSL errors will lead to a failure of the flow.
 */
bool NextCloudLoginFlow::ignoreSslErrors() const
{
    return m_ignoreSslErrors;
}

void NextCloudLoginFlow::setIgnoreSslErrors(bool ignoreSslErrors)
{
    if (m_ignoreSslErrors != ignoreSslErrors) {
        m_ignoreSslErrors = ignoreSslErrors;
        emit ignoreSslErrorsChanged();
    }
}

void NextCloudLoginFlow::prepareReply(QNetworkReply* reply)
{
    connect(reply, &QNetworkReply::sslErrors, this, &NextCloudLoginFlow::onSslErrors);
}

void NextCloudLoginFlow::onSslErrors(const QList<QSslError>& errors)
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (m_ignoreSslErrors) {
            reply->ignoreSslErrors();
        } else {
            qCWarning(log) << "SSL errors occurred:" << errors;
        }
    } else {
        qCWarning(log) << "onSslErrors called from something else than a QNetworkReply";
    }
}

/**
 * @brief Kicking off the flow finished.
 *
 * This is called once the flow kick off is done. If everything went well, we should not have
 * received a JSON blob with the required meta information to poll the server and open a browser for
 * the user. If not, the flow finished already with an error.
 */
void NextCloudLoginFlow::onFlowKickoffFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        if (reply->error() != QNetworkReply::NoError) {
            qCWarning(log) << "Error during login flow kick off:" << reply->errorString();
            finish();
        } else {
            auto data = reply->readAll();
            QJsonParseError parseError;
            auto doc = QJsonDocument::fromJson(data, &parseError);
            if (parseError.error != QJsonParseError::NoError) {
                qCWarning(log) << "Invalid JSON data:" << parseError.errorString();
                finish();
            } else {
                QString validationError;
                if (!isValidFlowKickoffResponse(doc, validationError)) {
                    qCWarning(log) << "Invalid JSON data:" << validationError;
                    finish();
                } else {
                    auto login = doc.object().value("login").toString();
                    emit receivedLoginUrl(login);
                    m_token = doc.object().value("poll").toObject().value("token").toString();
                    m_endpoint = doc.object().value("poll").toObject().value("endpoint").toString();
                    poll();
                }
            }
        }
    } else {
        qCWarning(log) << "onFlowKickoffFinished called from something else than a QNetworkReply";
    }
}

void NextCloudLoginFlow::onPollFinished()
{
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply) {
        auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        switch (statusCode) {
        case 200:
            parsePollResult(reply->readAll());
            break;
        case 404:
            // Expected, wait for some time and retry:
            if (!m_pollTimer) {
                m_pollTimer = new QTimer(this);
                m_pollTimer->setInterval(1000);
                m_pollTimer->setSingleShot(true);
                connect(m_pollTimer, &QTimer::timeout, this, &NextCloudLoginFlow::poll);
            }
            m_pollTimer->start();
            break;
        default:
            qCWarning(log) << "Unexpected network return code from polling endpoint:" << statusCode;
            qCWarning(log) << "Error:" << reply->errorString();
            finish();
            break;
        }
    } else {
        qCWarning(log) << "onPollFinished called from something else than a QNetworkReply";
    }
}

void NextCloudLoginFlow::poll()
{
    if (m_networkAccessManager) {
        QUrlQuery query;
        query.addQueryItem("token", m_token);
        QNetworkRequest request;
        request.setUrl(m_endpoint);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        auto reply = m_networkAccessManager->post(request, query.toString().toUtf8());
        if (!reply) {
            qCWarning(log) << "Failed to create poll reply";
            finish();
        } else {
            prepareReply(reply);
            connect(reply, &QNetworkReply::finished, this, &NextCloudLoginFlow::onPollFinished);
        }
    } else {
        qCWarning(log) << "poll called without flow ongoing";
    }
}

void NextCloudLoginFlow::parsePollResult(const QByteArray& data)
{
    QJsonParseError parseError;
    auto doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error == QJsonParseError::NoError) {
        QString validationError;
        if (isValidPollResultResponse(doc, validationError)) {
            auto obj = doc.object();
            emit receivedLogin(obj.value("loginName").toString(),
                               obj.value("appPassword").toString(), obj.value("server").toString());
            qCDebug(log) << "Successfully finished login flow";
            finished();
        } else {
            qCWarning(log) << "Invalid poll result received from server:" << validationError;
            finish();
        }
    } else {
        qCWarning(log) << "Invalid result returned from poll endpoint:" << parseError.errorString();
        finish();
    }
}

void NextCloudLoginFlow::finish()
{
    delete m_pollTimer;
    m_pollTimer = nullptr;
    delete m_networkAccessManager;
    m_networkAccessManager = nullptr;
    emit finished();
}
