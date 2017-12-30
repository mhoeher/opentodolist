---
layout: post
title: OpenTodoList 3.1 Has Been Released
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
date: 2017-10-26T00:00:00+02:00
---

Just a few days after the release v3.0 of the app, here comes another one!

Changelog
=========

{% capture images %}
    /images/Screenshot_2017OpenTodoList-Android-1026-094104-169x300.png
{% endcapture %}
{% include gallery images=images caption="Screenshot of the app running on Android." cols=1 %}

This time, most of the changes relate to the Android port of the app. The most notable change: The Material style is now in use, which better integrates the app into the system. Another change is regarding secure WebDAV: The app now comes with OpenSSL bundled, which is required to allow access to secured (HTTPS) WebDAV servers (which should be normal nowadays!). Finally, this release fixes the type selection when creating a new library. On some platforms - especially ones with high resolution screens - that part of the app was next to unusable. This should be much better now on all supported platforms.

Downloads
=========

You can download pre-compiled binaries of the app for Windows (32 and 64 bit), Linux and macOS on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.1.0). For Android, you can install the app via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist&hl=en) (note that the app is in Beta still, so you have to join the beta program for it in order to install it right now).

Known Issues and Limitations
============================

The app is still in heavy development, especially after the recent refactoring. So be prepared for some smaller and larger issues. Especially, on some Android devices, the app might forget the WebDAV password between restarts. To resume sync, you have to go into the sync settings of a library and re-enter the password.