---
layout: post
title: OpenTodoList 2.2.0 Has Been Released
modified:
categories: 
description:
tags: [Release, Windows, Linux, macOS, Android]
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2016-04-03T00:00:00+02:00
---

{% capture images %}
    /images/Version_2.2.0.png
{% endcapture %}
{% include gallery images=images caption="The new context sensitive toolbar shows icons as required." cols=1 %}

We are proud to announce the latest release of OpenTodoList 2.2.0. This version extends the app by some further basic functionality. Additionally, some bugs have been fixed. You can get the latest release from the [OpenTodoList page](https://www.rpdev.net/wordpress/apps/opentodolist/).

Important Note for Windows Users
================================

In case you installed the app with admin privileges before (e.g. to be able to install it to C:\\Programs Files), then please uninstall the app once and re-install it. This is required to enable a fix which has been applied to the installer/updated.

Changelog
=========

*   Updated the project's README file as it was outdated after the recent refactoring.
*   On Android, the app now uses an alternative file and folder selection, which is more convenient than the Qt built-in file dialog on this platform.
*   The app now listens for file and folder changes in the background. That means, if you use some synchronization tool like e.g. the ownCloud desktop client to sync your library with the cloud, the app now will recognize updated that happen in the background while the app is running. Before, one had to restart the app for such changes to take effect.
*   The installer/updater has been fixed. This mainly affects Windows, the an update of the app would fail. Note that in case you use the app on Windows, you probably have to uninstall it once and then re-install it from scratch. Afterwards, you should be able to use the automatic updater to pull in new versions.
*   Fixed a bug which could cause items not to be saved correctly in case some special characters are used in the item's title.
*   Improved the tool bar: Most icons now are shown only when required. This makes it easier to use especially on mobile devices.
*   Implemented a shortcut for "Create and Open". When creating a new note, todo list or todo, you can now use the **Ctrl+Enter** shortcut to create and immediately open the item.