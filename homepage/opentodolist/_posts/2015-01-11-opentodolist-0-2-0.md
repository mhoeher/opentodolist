---
layout: post
title: OpenTodoList 0.2.0
modified:
categories: 
description:
tags: [Release, Windows, Linux, macOS, Android, Beta]
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2015-01-11T00:00:00+01:00
---

After quite some time of silence, I finally managed to get the next release of OpenTodoList out. So here finally comes 0.2.0 ;)

Warning: This is BETA
=====================

If you want to give the app a try, please keep in mind that this version should definitely be considered beta status (read also below for some notes). Plese keep this in mind and consider doing backups of your data from time to time.

What's new in 0.2.0
===================

This version is a big step forward compared to 0.1.0. This has several reasons.

Reworked Storage Backend
------------------------

The database part of the application has been completely reworked. This was necessary to be able to implement certain requested features which initially not have been considered. With the new structure, the application should be well prepared also for future requests, as it's quite flexible and uses a well defined protocol for data synchronization. While for an end user, most of this change will be invisible, you will note one thing: Application start up is now lightning fast. This is due to the database is persistent, so restarting the app is much faster now (in the sense that you'll see your data immediately after start up).

Changed Data Model
------------------

This change is somewhat more visible to end users: In pre-0.2.0 versions, the data model of the application somewhat was like this:

*   The app consisted of several so called backends. Each backend implements access to another service which stores your todo lists.
*   Per backend, you could create arbitrary many todo lists.
*   In turn, todo lists contained todos. A todo in turn could contain other todos.

This data model allowed to create arbitrary deeply nested structures. While this might be useful for some cases, it makes first application development and second UI design more difficult. So for this reason, the data model has been simplified and also been enhanced to deal with the forthcoming connections to online services for data storage:

*   On top level, we still have backends.
*   Per backend, you have accounts. Depending on the backend, they might either be more or less invisible (e.g. for the local storage backend, which provides one fixed "account"). However, future backends will provide arbitrary many such accounts to you, so you can connect to multiple accounts/service instances, depending on the concrete storage service.
*   On the next level, we have todo lists.
*   Per todo list, you can have arbitrary many todos.
*   Finally, per todo you can have arbitrary many tasks. Tasks are similar to todos but have fewer properties.

Having this, the data model is limitted to exactly 5 levels of objects, which makes development in the future much easier (and should result in simpler to understand UIs).

User Interface Rewrite
----------------------

The user interface has been reworked. Without going too much into details: The UI should now integrate much better into the operating system, which will especially be visible on Android, where the app was in some situations quite difficult to use before.

Installtion
===========

Installation for Linux, Mac OS X and Windows
--------------------------------------------

To install OpenTodoList, please download the appropriate installer for your operating system. The installer is available in two flavors: One online installer which will download the actual app and one offline installer, which contains the application but also allows you to update to newer versions in the future.

*   [Linux x86_64 Online Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-online-linux-x86_64-0.1.0)
*   [Linux x86_64 Offline Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-offline-linux-x86_64-0.2.0)
*   [Mac OS X Online Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-online-macos-x86_64-0.2.0.dmg)
*   [Mac OS X Offline Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-offline-macos-x86_64-0.2.0.dmg)
*   [Windows 32bit Online Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-online-windows-i386-0.2.0.exe)
*   [Windows 32bit Offline Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-offline-windows-i386-0.2.0.exe)

Android
-------

You can install OpenTodoList also from the Android Play Store. We are currently in a beta phase. In order to be able to install using the Play Store:

1.  Join the [OpenTodoList Beta Users Google+ community.](https://plus.google.com/communities/106580508283710923527)
[](https://plus.google.com/communities/106580508283710923527)3.  [](https://plus.google.com/communities/106580508283710923527)[Opt-in to the beta version by following this link.](https://play.google.com/apps/testing/net.rpdev.opentodolist)

Afterwards, you should find OpenTodoList in the Google Play Store. This data model allowed to create arbitrary deeply nested structures. While this might be useful for some cases, it makes first application development and second UI design more difficult. So for this reason, the data model has been simplified and also been enhanced to deal with the forthcoming connections to online services for data storage:

*   On top level, we still have backends.
*   Per backend, you have accounts. Depending on the backend, they might either be more or less invisible (e.g. for the local storage backend, which provides one fixed "account"). However, future backends will provide arbitrary many such accounts to you, so you can connect to multiple accounts/service instances, depending on the concrete storage service.
*   On the next level, we have todo lists.
*   Per todo list, you can have arbitrary many todos.
*   Finally, per todo you can have arbitrary many tasks. Tasks are similar to todos but have fewer properties.

Having this, the data model is limitted to exactly 5 levels of objects, which makes development in the future much easier (and should result in simpler to understand UIs).

User Interface Rewrite
----------------------

The user interface has been reworked. Without going too much into details: The UI should now integrate much better into the operating system, which will especially be visible on Android, where the app was in some situations quite difficult to use before.

Installtion
===========

Installation for Linux, Mac OS X and Windows
--------------------------------------------

To install OpenTodoList, please download the appropriate installer for your operating system. The installer is available in two flavors: One online installer which will download the actual app and one offline installer, which contains the application but also allows you to update to newer versions in the future.

*   [Linux x86_64 Online Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-online-linux-x86_64-0.1.0)
*   [Linux x86_64 Offline Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-offline-linux-x86_64-0.2.0)
*   [Mac OS X Online Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-online-macos-x86_64-0.2.0.dmg)
*   [Mac OS X Offline Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-offline-macos-x86_64-0.2.0.dmg)
*   [Windows 32bit Online Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-online-windows-i386-0.2.0.exe)
*   [Windows 32bit Offline Installer](http://rpdev.net/public/repositories/qtifw/OpenTodoList-offline-windows-i386-0.2.0.exe)

Android
-------

You can install OpenTodoList also from the Android Play Store. We are currently in a beta phase. In order to be able to install using the Play Store:

1.  Join the [OpenTodoList Beta Users Google+ community.](https://plus.google.com/communities/106580508283710923527)
[](https://plus.google.com/communities/106580508283710923527)3.  [](https://plus.google.com/communities/106580508283710923527)[Opt-in to the beta version by following this link.](https://play.google.com/apps/testing/net.rpdev.opentodolist)

Afterwards, you should find OpenTodoList in the Google Play Store.