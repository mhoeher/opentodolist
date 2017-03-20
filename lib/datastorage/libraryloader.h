#ifndef LIBRARYLOADER_H
#define LIBRARYLOADER_H

#include <QObject>
#include <QThread>

#include "item.h"

class LibraryLoaderWorker;

class LibraryLoader : public QObject
{
    Q_OBJECT
public:
    explicit LibraryLoader(QObject *parent = 0);
    virtual ~LibraryLoader();

    QString directory() const;
    void setDirectory(const QString& directory);

signals:

    /**
     * @brief An item has been loaded.
     *
     * This signal is emitted to indicate that an @p item has been
     * found when scanning a library directory.
     */
    void itemLoaded(ItemPtr item);

    /**
     * @brief Scanning has finished.
     */
    void scanFinished();

public slots:

    void scan();

private:

    QString m_directory;
    QThread m_thread;
    LibraryLoaderWorker *m_worker;

};

class LibraryLoaderWorker : public QObject
{
    Q_OBJECT

    friend class LibraryLoader;

private:

    bool m_stop;

    LibraryLoaderWorker();

private slots:

    void scan(const QString &directory, QObject* targetThread);

signals:

    void itemLoaded(ItemPtr item);
    void scanFinished();
};

#endif // LIBRARYLOADER_H
