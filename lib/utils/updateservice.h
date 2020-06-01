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

#ifndef UTILS_UPDATESERVICE_H_
#define UTILS_UPDATESERVICE_H_

#include <QLoggingCategory>
#include <QObject>

class UpdateService : public QObject
{
    Q_OBJECT
public:
    explicit UpdateService(QObject* parent = nullptr);

signals:

    /**
     * @brief An update is available.
     *
     * This signal is emitted to indicate that an update is available. The
     * @p version is the name of the most recent available update. The
     * @p url points to a web page where the new version can be downloaded
     * from.
     */
    void updateAvailable(const QString& version, const QUrl& url);

private slots:

    void checkForUpdates();
};

#endif // UTILS_UPDATESERVICE_H_
