---
layout: post
title: OpenTodoList 3.2 Has Been Released
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
date: 2017-10-31T00:00:00+01:00
---

A new release - 3.2 - has just been made available.

Changelog
=========

{% capture images %}
    /images/Screenshot_20171031-152721.png
{% endcapture %}
{% include gallery images=images caption="The app running on Android." cols=1 %}

This version focuses once more on making the app usable on Android: First of all, on newer releases of Android, login credentials for synchronization are now properly restored when the app starts. Additionally, the search functionality has been refactored as the previous implementation used to crash on Android (with some probability of other platforms also having the same bug, just occurring less frequently). Besides, there are also minor changes which affect all platforms:

*   When creating a new library, it is now immediately opened.
*   Restarting the app re-opens the last active library (and tag).
*   The images in the backend selection when creating a new library now scale with the available width, making the screen look nice on a wider variety of screen sizes.
*   In case the app cannot load login credentials on start up, a notification is shown to the user to inform him about that fact.
*   And finally: Update notifications now point to the GitHub page, as this is where the downloads are actually hosted.

Downloads
=========

You can download pre-compiled binaries of the app for Windows (32 and 64 bit), Linux and macOS on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.1.0). For Android, you can install the app via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist&hl=en) (note that the release might be in the beta stage for the first few days, so either join the beta testers via Google Play or wait for the update to be rolled out to everyone later).

Known Issues and Limitations
============================

Currently, there is no way to cancel a sync once it has been started. Hence, especially on Android you might rather want to send the app to background instead of terminating it. Also keep in mind that there is no background sync yet: As soon as you terminate the app, synchronization will stop.