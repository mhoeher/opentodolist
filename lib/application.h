#ifndef APPLICATION_H
#define APPLICATION_H

#include "library.h"

#include <QObject>
#include <QQmlListProperty>
#include <QSettings>
#include <QStringList>
#include <QUrl>
#include <QVariantMap>
#include <QVector>


class Migrator_2_x_to_3_x;

/**
 * @brief The main class of the application
 *
 * The Application class is used as entry point into the OpenTodoList application. It is used
 * as contained class and provides references to other objects. Basically, the Application class
 * models the application, i.e. it is created when the application starts and destroyed once
 * the application is to be closed.
 */
class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Library> libraries READ libraryList NOTIFY librariesChanged)
    Q_PROPERTY(Library* defaultLibrary READ defaultLibrary NOTIFY defaultLibraryChanged)
    Q_PROPERTY(bool updatesAvailable READ updatesAvailable NOTIFY updatesAvailableChanged)
    Q_PROPERTY(bool hasUpdateService READ hasUpdateService CONSTANT)

    friend class Migrator_2_x_to_3_x;
public:

    explicit Application(QObject *parent = 0);
    explicit Application(QSettings *settings, QObject *parent = 0);

    virtual ~Application();

    /**
   * @brief The list of all Library objects present in the application.
   */
    QList<Library*> libraries() const               { return m_libraries; }

    QStringList libraryTypes() const;
    QQmlListProperty<Library> libraryList();

    Q_INVOKABLE void saveValue(const QString &name, const QVariant &value);
    Q_INVOKABLE QVariant loadValue(const QString &name, const QVariant &defaultValue = QVariant());

    Q_INVOKABLE QString readFile(const QString &fileName) const;
    Q_INVOKABLE QVariant find3rdPartyInfos() const;

    Q_INVOKABLE QString urlToLocalFile(const QUrl &url) const;
    Q_INVOKABLE QUrl localFileToUrl(const QString &localFile) const;

    Library *defaultLibrary();

    bool updatesAvailable() const;
    void setUpdatesAvailable(bool updatesAvailable);

    Q_INVOKABLE void checkForUpdates(bool forceCheck = false);
    Q_INVOKABLE void runUpdate();
    bool hasUpdateService() const;

    Q_INVOKABLE QUrl homeLocation() const;
    Q_INVOKABLE bool folderExists(const QUrl &url) const;

signals:

    void librariesChanged();
    void defaultLibraryChanged();
    void updatesAvailableChanged();

public slots:

private:

    QList<Library*>         m_libraries;
    Library                *m_defaultLibrary;
    QSettings              *m_settings;
    bool                    m_loadingLibraries;
    bool                    m_updatesAvailable;

    void saveLibraries();
    void loadLibraries();

    static Library* librariesAt(QQmlListProperty<Library> *property, int index);
    static int librariesCount(QQmlListProperty<Library> *property);

    QString defaultLibraryLocation() const;
    void runMigrations();

    void runUpdateCheck();
    void resetUpdateCheck();
    void runCachedUpdateCheck();
    void saveUpdatesAvailable(bool available);

    void appendLibrary(Library* library);

private slots:

    void onLibraryDeleted(Library *library);

};

#endif // APPLICATION_H
