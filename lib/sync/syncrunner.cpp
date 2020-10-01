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

#include "syncrunner.h"

#include <QLoggingCategory>

#include "syncjob.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.SyncRunner", QtWarningMsg);

/**
 * @brief Create a sync runner.
 *
 * Creates a new sync runner. The runner will execute the given @p job.
 * The runner takes ownership of the job.
 */
SyncRunner::SyncRunner(QSharedPointer<SyncJob> job) : QRunnable(), m_job(job)
{
    Q_CHECK_PTR(job);
}

/**
 * @brief Destructor.
 */
SyncRunner::~SyncRunner() {}

/**
 * @brief Run the sync job.
 */
void SyncRunner::run()
{
    qCDebug(log) << "Run sync job" << m_job;
    m_job->execute();
}
