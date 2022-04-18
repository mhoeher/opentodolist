/*
 * Copyright 2022 Martin Hoeher <martin@rpdev.net>
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

#ifndef SYNC_REMOTELIBRARYINFO_H_
#define SYNC_REMOTELIBRARYINFO_H_

#include <QObject>
#include <QString>
#include <QUuid>

/**
 * @brief Information about an existing library on the server.
 *
 * This class encapsulates information about an existing library on a remote server.
 */
class RemoteLibraryInfo
{
    Q_GADGET

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QUuid uid READ uid)

public:
    RemoteLibraryInfo();

    QString name() const;
    void setName(const QString& name);

    QString path() const;
    void setPath(const QString& path);

    QUuid uid() const;
    void setUid(const QUuid& uid);

    bool operator==(const RemoteLibraryInfo& other) const;

private:
    QString m_name;
    QString m_path;
    QUuid m_uid;
};

Q_DECLARE_METATYPE(RemoteLibraryInfo)

#endif // SYNC_REMOTELIBRARYINFO_H_
