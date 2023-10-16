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

#include "iosplatformshareutils.h"

#include <QDesktopServices>
#include <QUrl>

#include <UIKit/UIKit.h>

#include "utils/ios/shareutilsdocviewcontroller.h"

// The implementation is based on the ideas from https://github.com/ekke/ekkesSHAREexample.

/**
 * @brief Implementation of PlatformShareUtils::shareFile().
 */
void IOSPlatformShareUtils::openFile(const QUrl& url)
{
    NSURL* fileUrl = url.toNSURL();

    static IOSShareUtilsDocViewController* docViewController = nil;
    if (docViewController != nil) {
        [docViewController removeFromParentViewController];
        [docViewController release];
    }

    UIDocumentInteractionController* docInteractionController = nil;
    docInteractionController =
            [UIDocumentInteractionController interactionControllerWithURL:fileUrl];

    UIViewController* viewController =
            [[[[UIApplication sharedApplication] windows] firstObject] rootViewController];
    if (viewController != nil) {
        docViewController = [[IOSShareUtilsDocViewController alloc] init];
        docViewController.m_url = url;
        docViewController.m_shareUtils = this;

        [viewController addChildViewController:(docViewController)];
        docInteractionController.delegate = docViewController;

        if (![docInteractionController presentPreviewAnimated:(YES)]) {
            emit noAppFound(url);
        }
    }
}

/**
 * @brief Implementation of PlatformShareUtils::editFile().
 */
void IOSPlatformShareUtils::editFile(const QUrl& url, FileEditObserver* observer)
{
    PlatformShareUtils::editFile(url, observer); // For handling watching

    // Same as viewing:
    openFile(url);
}

/**
 * @brief Implementation of PlatformShareUtils::openFolder().
 */
void IOSPlatformShareUtils::openFolder(const QString& path)
{
    Q_UNUSED(path);
}

/**
 * @brief Implementation of PlatformShareUtils::openUrl().
 */
void IOSPlatformShareUtils::openUrl(const QUrl& url)
{
    if (!QDesktopServices::openUrl(url)) {
        emit noBrowserFound(url);
    }
}

/**
 * @brief Implementation of PlatformShareUtils::unwatchFile().
 */
void IOSPlatformShareUtils::unwatchFile(const QUrl& url)
{
    // TODO: Not implemented yet - to be done once editing is implemented for iOS
    Q_UNUSED(url);
}
