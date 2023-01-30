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

#ifndef DATASTORAGE_LOADLIBRARYQUERY_H_
#define DATASTORAGE_LOADLIBRARYQUERY_H_

#include <QString>
#include <QUuid>

#include "itemsquery.h"

class LoadLibraryQuery : public ItemsQuery
{
    Q_PROPERTY(QString directory READ directory WRITE setDirectory)
    Q_PROPERTY(QUuid libraryId READ libraryId WRITE setLibraryId)
public:
    explicit LoadLibraryQuery(QObject* parent = nullptr);

    // ItemsQuery interface
    QString directory() const;
    void setDirectory(const QString& newDirectory);

    QUuid libraryId() const;
    void setLibraryId(const QUuid& newLibraryId);

signals:
    void directoryChanged();
    void libraryIdChanged();

protected:
    void run() override;

private:
    QString m_directory;
    QUuid m_libraryId;
};

#endif // DATASTORAGE_LOADLIBRARYQUERY_H_
