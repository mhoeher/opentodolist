#include "opentodolistqmlextensionsplugin.h"

#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileSystemWatcher>
#include <QFont>
#include <QFontInfo>
#include <QGuiApplication>
#include <QIcon>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#ifdef OTL_USE_SINGLE_APPLICATION
#include <QtSingleGuiApplication>
#else
#include <QGuiApplication>
#endif

#include <iostream>



class QmlFileSystemWatcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged)

public:

    QmlFileSystemWatcher(const QString baseUrl, QQmlApplicationEngine *engine, QObject *parent = 0) :
        QObject(parent),
        m_baseUrl(baseUrl),
        m_engine(engine),
        m_modified(false)
  #ifdef OPENTODOLIST_DEBUG
      , m_watcher(new QFileSystemWatcher(this))
  #endif
    {
        Q_CHECK_PTR(m_engine);
#ifdef OPENTODOLIST_DEBUG
        Q_CHECK_PTR(m_watcher);
        if (QDir(m_baseUrl).exists()) {
            watchPath();
            connect(m_watcher, &QFileSystemWatcher::fileChanged, [this](const QString &file) {
                qDebug() << "File" << file << "has changed.";
                m_modified = true;
                watchPath();
                emit modifiedChanged();
            });
            connect(m_watcher, &QFileSystemWatcher::directoryChanged, [this](const QString &dir) {
                m_modified = true;
                qDebug() << "Directory" << dir << "has changed.";
                watchPath();
                emit modifiedChanged();
            });
        }
#endif
    }

    bool modified() const { return m_modified; }

public slots:

    void reload() {
#ifdef OPENTODOLIST_DEBUG
        m_engine->clearComponentCache();
        m_engine->load(QUrl(m_baseUrl + "main.qml"));
        m_modified = false;
        emit modifiedChanged();
#endif
    }

signals:

    void modifiedChanged();

private:

    QString                 m_baseUrl;
    QQmlApplicationEngine  *m_engine;
    bool                    m_modified;
#ifdef OPENTODOLIST_DEBUG
    QFileSystemWatcher     *m_watcher;
#endif

    void watchPath() {
#ifdef OPENTODOLIST_DEBUG
        if (!m_watcher->directories().isEmpty()) {
            m_watcher->removePaths(m_watcher->directories());
        }
        if (!m_watcher->files().isEmpty()) {
            m_watcher->removePaths(m_watcher->files());
        }
        m_watcher->addPath(m_baseUrl);
        QDirIterator it(m_baseUrl, QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QString entry = it.next();
            m_watcher->addPath(entry);
        }
#endif
    }

};


int main(int argc, char *argv[])
{
    //qSetMessagePattern("%{file}(%{line}): %{message}");
#if OPENTODOLIST_DEBUG
    QLoggingCategory(0).setEnabled(QtDebugMsg, true);
#endif

#ifdef OTL_USE_SINGLE_APPLICATION
    QtSingleGuiApplication app("OpenTodoList", argc, argv);
#else
    QGuiApplication app(argc, argv);
#endif

    app.setAttribute(Qt::AA_EnableHighDpiScaling);

    QCoreApplication::setApplicationName("OpenTodoList");
    QCoreApplication::setApplicationVersion(VERSION);
    QCoreApplication::setOrganizationDomain("www.rpdev.net");
    QCoreApplication::setOrganizationName("RPdev");

    app.setWindowIcon(QIcon(":/res/OpenTodoList80.png"));

    QCommandLineParser parser;
    parser.setApplicationDescription(
                QCoreApplication::translate("main", "Manage your personal data."));
    parser.addHelpOption();
    parser.addVersionOption();

#ifdef OPENTODOLIST_DEBUG
    QCommandLineOption qmlRootOption = {{"Q", "qml-root"},
                                        QCoreApplication::translate("main", "QML Root Directory"),
                                        QCoreApplication::translate("main", "DIR")};
    parser.addOption(qmlRootOption);
#endif

    // Enable touch screen optimizations
    QCommandLineOption enableTouchOption = {{"T", "enable-touch"},
                                            QCoreApplication::translate(
                                            "main",
                                            "Switch on some optimizations for touchscreens.")};
    parser.addOption(enableTouchOption);

    parser.process(app);

#ifdef OTL_USE_SINGLE_APPLICATION
    if (app.sendMessage("activate")) {
        return 0;
    }
#endif

    QQmlApplicationEngine engine;
    QString qmlBase = "qrc:/";

#ifdef OPENTODOLIST_DEBUG
    if (parser.isSet(qmlRootOption)) {
        qmlBase = QDir(parser.value(qmlRootOption)).canonicalPath() + "/";
        if (!QFile::exists(qmlBase + "main.qml")) {
            qFatal("File main.qml does not exist in %s, probably not a valid OpenTodoList QML dir!",
                   qUtf8Printable(qmlBase));
        }
    }
#endif

    engine.addImportPath(qmlBase);
    OpenTodoListQmlExtensionsPlugin plugin;
    plugin.registerTypes("OpenTodoList");

#ifdef OPENTODOLIST_DEBUG
    QmlFileSystemWatcher watcher(qmlBase, &engine);
    engine.rootContext()->setContextProperty("qmlFileSystemWatcher", &watcher);
#endif


    engine.rootContext()->setContextProperty("debugMode",
                                             QVariant(
                                             #ifdef OPENTODOLIST_DEBUG
                                                 true
                                             #else
                                                 false
                                             #endif
                                                 )
                                             );

    // Enable touch optimizations, this flag is controlled via CLI, additionally, it
    // is set implicitly on some platforms:
    {
        bool enableTouchOptimizations =
        #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_QNX) || defined(Q_OS_WINPHONE)
                true
        #else
                false
        #endif
                ;
        if (parser.isSet(enableTouchOption))
        {
            enableTouchOptimizations = true;
        }
        engine.rootContext()->setContextProperty("enableTouchOptimizations",
                                                 enableTouchOptimizations);
    }
#ifdef OTL_USE_SINGLE_APPLICATION
    engine.rootContext()->setContextProperty("application", &app);
#endif
    engine.rootContext()->setContextProperty("applicationVersion", QVariant(VERSION));
    engine.rootContext()->setContextProperty("defaultFontPixelSize", QFontInfo(QFont()).pixelSize());
    engine.rootContext()->setContextProperty("qmlBaseDirectory", qmlBase);
    engine.load(QUrl(qmlBase + "main.qml"));

    return app.exec();
}

#include "main.moc"
