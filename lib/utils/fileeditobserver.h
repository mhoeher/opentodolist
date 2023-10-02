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

#ifndef UTILS_FILEEDITOBSERVER_H_
#define UTILS_FILEEDITOBSERVER_H_

#include <QObject>
#include <QUrl>

class FileEditObserver : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUrl fileUrl READ fileUrl WRITE setFileUrl NOTIFY fileUrlChanged FINAL)

public:
    explicit FileEditObserver(QObject* parent = nullptr);

    QUrl fileUrl() const;
    void setFileUrl(const QUrl& newFileUrl);

public slots:

    void notify(const QUrl& url);

signals:

    void fileUrlChanged();
    void fileChanged(const QUrl& url);

private:
    QUrl m_fileUrl;
};

#endif // UTILS_FILEEDITOBSERVER_H_
