#ifndef LIBRARIESITEMSQUERY_H
#define LIBRARIESITEMSQUERY_H

#include <QObject>

#include "datamodel/library.h"
#include "datastorage/itemsquery.h"

class LibrariesItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit LibrariesItemsQuery(QObject *parent = nullptr);

    bool includeCalculatedValues() const;
    void setIncludeCalculatedValues(bool includeCalculatedValues);

signals:

    /**
     * @brief The libraries have been loaded.
     *
     * This signal is emitted to indicate that the libraries have been
     * loaded from the cache. The @p libraries list contains the cache
     * entries, which can be turned to library objects by calling
     * Library::decache() on each entry.
     */
    void librariesAvailable(QVariantList libraries);

    // ItemsQuery interface
protected:
    void run() override;

private:

    bool m_includeCalculatedValues;
};

#endif // LIBRARIESITEMSQUERY_H
