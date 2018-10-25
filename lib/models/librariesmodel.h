#ifndef LIBRARIESMODEL_H
#define LIBRARIESMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QUuid>

class Cache;
class Library;

/**
 * @brief A model which makes the libraries available.
 *
 * This class implements a model which make the libraries defined in an
 * application's cache available.
 */
class LibrariesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Cache* cache READ cache WRITE setCache NOTIFY cacheChanged)
public:
    enum {
        LibraryRole = Qt::UserRole
    };

    explicit LibrariesModel(QObject *parent = nullptr);

    Cache *cache() const;
    void setCache(Cache *cache);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:

    /**
     * @brief The cache property has changed.
     */
    void cacheChanged();

public slots:

private:

    QPointer<Cache> m_cache;
    QHash<QUuid, Library*> m_libraries;
    QList<QUuid> m_uids;

    void fetch();

private slots:

    void librariesAvailable(QVariantList libraries);
    void libraryChanged();

};

#endif // LIBRARIESMODEL_H
