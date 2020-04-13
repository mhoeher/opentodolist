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

#ifndef SYNC_SYNCJOB_H_
#define SYNC_SYNCJOB_H_

#include <QObject>
#include <QSharedPointer>

class Library;
class Account;

/**
 * @brief A synchronizing job.
 */
class SyncJob : public QObject
{
    Q_OBJECT
public:
    explicit SyncJob(const QString &libraryDirectory, QSharedPointer<Account> account,
                     QObject *parent = nullptr);

    void execute();

signals:

    void syncFinished(QString libraryDirectory);

    void stopRequested();
    void syncError(QString libraryDirectory, const QString &error);

public slots:

    void stop();

private:
    QString m_libraryDirectory;
    QSharedPointer<Account> m_account;

private slots:

    void onSyncError(const QString &error);
};

#endif // SYNC_SYNCJOB_H_
