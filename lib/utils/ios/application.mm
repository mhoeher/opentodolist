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

#include "application.h"

#include <QLoggingCategory>
#include <QUrl>

#import <Foundation/Foundation.h>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Application.iOS", QtDebugMsg);


void Application::disableIOSBackup()
{
    auto librariesFolder = Library::defaultLibrariesLocation().toNSString();
    auto url = [NSURL fileURLWithPath:(librariesFolder)];
    NSNumber *excludedFromBackup = @(YES);
    NSError *error = nil;
    if (![url setResourceValue:excludedFromBackup forKey:NSURLIsExcludedFromBackupKey error:&error]) {
        QString errorMessage;
        if (error) {
            errorMessage = QString::fromNSString(error.description);
        }
        qCWarning(log) << "Failed to exclude libraries folder from backup:" << errorMessage;
    } else {
        qCDebug(log) << "Successfully excluded libraries folder from backup.";
    }
}
