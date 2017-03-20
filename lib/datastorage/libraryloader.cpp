#include "libraryloader.h"

#include <QDebug>
#include <QJsonDocument>
#include <QVariantMap>

/**
 * @brief Constructor.
 */
LibraryLoader::LibraryLoader(QObject *parent) : QObject(parent),
    m_thread(),
    m_worker(new LibraryLoaderWorker())
{
    qRegisterMetaType<ItemPtr>();
    m_thread.start();
    m_worker->moveToThread(&m_thread);
    connect(m_worker, &LibraryLoaderWorker::itemLoaded, this, &LibraryLoader::itemLoaded);
    connect(m_worker, &LibraryLoaderWorker::scanFinished, this, &LibraryLoader::scanFinished);
}

/**
 * @brief Destructor.
 */
LibraryLoader::~LibraryLoader()
{
    m_worker->m_stop = true;
    m_thread.quit();
    if (!m_thread.wait(5000)) {
        m_thread.terminate();
        m_thread.wait(5000);
    }
    delete m_worker;
}

/**
 * @brief The directory to scan for items.
 */
QString LibraryLoader::directory() const
{
    return m_directory;
}

/**
 * @brief Set the directory to scan for items.
 */
void LibraryLoader::setDirectory(const QString& directory)
{
    m_directory = directory;
}

/**
 * @brief Scan the directory for items.
 *
 * This triggers a scan of the directory for any kind of known item.
 * For each item discovered, the @p itemLoaded() signal is emitted.
 */
void LibraryLoader::scan()
{
    QMetaObject::invokeMethod(m_worker, "scan", Qt::QueuedConnection,
                              Q_ARG(const QString&, m_directory),
                              Q_ARG(QObject*, thread()));
}

/**
 * @brief Constructor.
 */
LibraryLoaderWorker::LibraryLoaderWorker() : QObject(),
    m_stop(false)
{
}

/**
 * @brief Scan the @p directory for items, moving any created one to the @p targetThread.
 */
void LibraryLoaderWorker::scan(const QString& directory, QObject* targetThread)
{
    QDir dir(directory);
    for (auto entry : dir.entryList({QString("*.%1").arg(Item::FileNameSuffix)}, QDir::Files)) {
        if (m_stop) {
            break;
        }
        auto filename = dir.absoluteFilePath(entry);
        QFile file(filename);
        if (file.open(QIODevice::ReadOnly)) {
            QJsonParseError error;
            auto map = QJsonDocument::fromJson(file.readAll(), &error).toVariant().toMap();
            if (error.error == QJsonParseError::NoError) {
                auto item = Item::createItem(map);
                if (item != nullptr) {
                    item->moveToThread(static_cast<QThread*>(targetThread));
                    emit itemLoaded(ItemPtr(item));
                }
            } else {
                qWarning() << "Failed to parse item file" << filename << ":" << error.errorString();
            }
            file.close();
        } else {
            qWarning() << "Failed to open item file" << filename << ":" << file.errorString();
        }
    }
    emit scanFinished();
}
