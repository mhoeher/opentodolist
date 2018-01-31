---
layout: post
title: OpenTodoList 3.5 Has Been Released
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
date: 2018-01-31T00:00:00+01:00
---

This is the first version of OpenTodoList release in 2018. And of course, we start with some cool new features!


## Changelog

* Localization support: The app can not be translated into other languages. Currently, translations to German are available.
* Selecting images to add to a library is now much improved: On Android, you can now select an image e.g. from the Gallery. On other platforms, a native file selection dialog is used instead.
* A sync is now triggered when you change items in a library. This ensures that changes end up on a server as quick as possibe and can be synchronized to your other devices.
* You can now set due dates on your items.
* This due date is used to populate the newly added *schedule view* each library has. This view quickly summarizes items that are due in the near future (and of course ones that are overdue).
* When you add new items, a notification is shown which let's you quickly open the newly created item (by clicking it or using the `Ctrl+O` keyboard shortcut).
* The app is now build regularly in the continuous test environment. In addition, the release versions are build there as well, which guarantees that they are build in a consistent way.
* OpenSSL libraries (required to allow the app to talk to NextCloud/ownCloud/WebDAV servers via the secure HTTPS protocol) are now build on the fly during the CIs. This is an important precondition for including the app in the free F-Droid app store.
* Installers for the Visual Studio based build of the app are now also build during the CI on Windows.
* The app can now be build with [CMake](https://cmake.org/) for most supported platforms.


{% capture images1 %}
    /images/v3.5/empty-library-page.png
{% endcapture %}
{% include gallery images=images1 caption="There have been some refinements in the various pages. For example, if a library is empty, you now get some nice hints on what to do next." %}

{% capture images2 %}
    /images/v3.5/select_due_date.png
{% endcapture %}
{% include gallery images=images2 caption="Items now can be assigned a due date." %}

{% capture images3 %}
    /images/v3.5/scheduled_todos.png
{% endcapture %}
{% include gallery images=images3 caption="The due date is usually shown in item listings e.g. inside a todo list." %}

{% capture images4 %}
    /images/v3.5/schedule_view.png
{% endcapture %}
{% include gallery images=images4 caption="In addition, there is a schedule view, which shows you items that are soon due (or already overdue)." %}



## Downloads

Please find the download links for major platforms on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.5.0). For Android, the release is available via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).


## Known Issues

* For Windows, two flavors of the app are provided: One flavor is cross-compiled binaries (i.e. the Windows release files are actually built on Linux, which makes deployment much easier). However, these binaries require a working OpenGL driver, which sometimes is not the case (especially, when running Windows inside a Virtual Machine). In this case, please use the installers which are built natively on Windows (the ones which have the **MSVC** part in their name).