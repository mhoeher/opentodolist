/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
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

#ifndef DATASTORAGE_LIBRARIESITEMSQUERY_H_
#define DATASTORAGE_LIBRARIESITEMSQUERY_H_

#include <QObject>

#include "datamodel/library.h"
#include "datastorage/itemsquery.h"

class LibrariesItemsQuery : public ItemsQuery
{
    Q_OBJECT
public:
    explicit LibrariesItemsQuery(QObject* parent = nullptr);

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

#endif // DATASTORAGE_LIBRARIESITEMSQUERY_H_
