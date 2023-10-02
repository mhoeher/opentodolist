/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
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

#include "platformshareutils.h"

#include <QTimer>

#include "fileeditobserver.h"

/**
 * @class PlatformShareUtils
 * @brief Base class for platform specific implementation of the share utilies.
 */

/**
 * @brief Constructor.
 */
PlatformShareUtils::PlatformShareUtils(QObject* parent) : QObject { parent }, m_watchedFiles {}
{
    auto timer = new QTimer { this };
    timer->setInterval(10000);
    timer->setSingleShot(false);
    connect(timer, &QTimer::timeout, this, [=]() {
        // remove all observers that "got out of scope":
        auto files = m_watchedFiles.keys(); // Remember watched files
        m_watchedFiles.removeIf([=](QMultiHash<QUrl, QPointer<FileEditObserver>>::Iterator it) {
            return it.value().isNull();
        });
        auto newWatched = m_watchedFiles.keys(); // New list of all watched files
        for (const auto& file : files) {
            // Any file that was previously watched but is no longer went "out of scope" of this
            // utils instance. Unwatch it!
            if (!newWatched.contains(file)) {
                unwatchFile(file);
            }
        }
    });
}

/**
 * @brief Edit a file.
 *
 * This will open the file pointed to by the given @p url. If spcified, the given @p observer will
 * be notified about any changes.
 *
 * @note The class does not take ownership of the observer. It is okay if an observer will be
 * destroyed while the file is still being watched. This class will take care to free resources if
 * necessary. This method is pure abstract - still, concrete subclasses shall call the base class
 * implementation to ensure managing observers works as expected.
 */
void PlatformShareUtils::editFile(const QUrl& url, FileEditObserver* observer)
{
    if (observer != nullptr) {
        m_watchedFiles.insert(url, observer);
    }
};

/**
 * @brief Notify observers about a change of a file.
 *
 * This notifies all observers watching the file pointed to by the given @p url about changes.
 */
void PlatformShareUtils::notifyFileChanged(const QUrl& url)
{
    auto observers = m_watchedFiles.values(url);
    for (auto& observer : observers) {
        if (observer) {
            observer->notify(url);
        }
    }
}
