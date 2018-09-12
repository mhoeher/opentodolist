#include "libraryloader.h"

#include <QDebug>
#include <QVariantMap>

#include "datamodel/library.h"

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
    connect(m_worker, &LibraryLoaderWorker::itemLoaded,
            this, &LibraryLoader::itemLoaded);
    connect(m_worker, &LibraryLoaderWorker::scanFinished,
            this, &LibraryLoader::scanFinished);
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
                              Q_ARG(QUuid, m_libraryId),
                              Q_ARG(const QString&, m_directory),
                              Q_ARG(QObject*, thread()));
}

QUuid LibraryLoader::libraryId() const
{
    return m_libraryId;
}

void LibraryLoader::setLibraryId(const QUuid &libraryId)
{
    m_libraryId = libraryId;
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
void LibraryLoaderWorker::scan(const QUuid &libraryId, const QString& directory, QObject* targetThread)
{
    auto years = Library::years(directory);
    for (auto year : years) {
        auto months = Library::months(directory, year);
        for (auto month : months) {
            QDir dir(directory + "/" + year + "/" + month);
            QString suffix = "*." + Item::FileNameSuffix;
            for (auto entry : dir.entryList({suffix}, QDir::Files)) {
                auto item = Item::createItemFromFile(dir.absoluteFilePath(entry));
                auto topLevelItem = qobject_cast<TopLevelItem*>(item);
                if (topLevelItem != nullptr) {
                    topLevelItem->setLibraryId(libraryId);
                }
                item->moveToThread(static_cast<QThread*>(targetThread));
                emit itemLoaded(ItemPtr(item));
            }
        }
    }
    emit scanFinished();
}
