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

#ifndef SYNC_SYNQCLIENTSYNCHRONIZER_H_
#define SYNC_SYNQCLIENTSYNCHRONIZER_H_

#include "synchronizer.h"

namespace SynqClient {
class DirectorySynchronizer;
}

class SynqClientSynchronizer : public Synchronizer
{
    Q_OBJECT
public:
    explicit SynqClientSynchronizer(QObject* parent = nullptr);

    void stopSync() override;

signals:
    void stopRequested();

protected:
    void setupDirectorySynchronizer(SynqClient::DirectorySynchronizer& sync);
    void runDirectorySynchronizer(SynqClient::DirectorySynchronizer& sync);
};

#endif // SYNC_SYNQCLIENTSYNCHRONIZER_H_
