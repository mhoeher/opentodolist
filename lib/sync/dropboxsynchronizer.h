/*
 * Copyright 2022-2023 Martin Hoeher <martin@rpdev.net>
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

#ifndef SYNC_DROPBOXSYNCHRONIZER_H_
#define SYNC_DROPBOXSYNCHRONIZER_H_

#include <QObject>
#include <QString>
#include <QDateTime>

#include "synqclientsynchronizer.h"

class DropboxSynchronizer : public SynqClientSynchronizer
{
    Q_OBJECT

    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)

public:
    explicit DropboxSynchronizer(QObject* parent = nullptr);

    // Synchronizer interface
    void synchronize() override;
    void fromMap(const QVariantMap& map) override;
    void setAccount(Account* account) override;

    const QString& accessToken() const;
    void setAccessToken(const QString& newAccessToken);

signals:

    void accessTokenChanged();

private:
    // For OAuth
    QString m_accessToken;
};

#endif // SYNC_DROPBOXSYNCHRONIZER_H_
