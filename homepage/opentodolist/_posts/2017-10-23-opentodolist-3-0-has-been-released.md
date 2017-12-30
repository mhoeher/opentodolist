---
layout: post
title: OpenTodoList 3.0 Has Been Released
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
date: 2017-10-23T00:00:00+02:00
---

It has been quite some time, but finally we are there: OpenTodoList 3.0 has been released! Please find the download links links for a variety of platforms (Windows, Linux, MacOS and Android) on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.0.0).

Changelog
=========

{% capture images %}
    /images/opentodolist-3.0.png
{% endcapture %}
{% include gallery images=images caption="When creating a new library, you can now create ones syncted to WebDAV servers like NextCloud or ownCloud." cols=1 %}

Version 3.0 is the result of some heavy restructuring under the hood. Most of the changes are not directly visible to the user. However, there are two notable changes: First, the library used to display controls like buttons, menus and pages has been changed from [Qt Quick Controls](https://github.com/qt/qtquickcontrols) to [Qt Quick Controls 2](https://github.com/qt/qtquickcontrols2). This change should mainly benefit the mobile version of the app, however, it also allows some improvements (like a better drawer). Second, the app now comes with built-in WebDAV Sync support. This means: When you create a new library, you can connect to a WebDAV server (like NextCloud, ownCloud or a generic WebDAV server) and create your library there. The app then takes care to sync your local library with a copy on the server. This is especially useful on mobile devices, where it is rather cumbersome or even impossible to set up third party services to do the sync.

Important Notes
===============

The app should work well on the supported desktop platforms Linux, macOS and Windows (both 32 and 64bit). The new version has also been build for Android, but currently is only available if you join the beta channel for the app via Google Play. There are some known issues with the app right now on Android, so be careful when updating. Please note that the format used to store items on disk changed. When upgrading, the app will migrate your existing libraries to the new format. However, you cannot easily go back to the previous version once you migrated.