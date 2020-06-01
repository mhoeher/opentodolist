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

#ifndef SYNC_SYNCRUNNER_H_
#define SYNC_SYNCRUNNER_H_

#include <QObject>
#include <QRunnable>

class SyncJob;

/**
 * @brief Run sync jobs.
 */
class SyncRunner : public QRunnable
{
public:
    explicit SyncRunner(SyncJob* job);
    virtual ~SyncRunner();

    // QRunnable interface
    void run() override;

private:
    SyncJob* m_job;
};

#endif // SYNC_SYNCRUNNER_H_
