---
layout: post
title: OpenTodoList 3.6 Has Been Released
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
date: 2018-02-27T00:00:00+01:00
---

February is nearly over - so here comes this month's OpenTodoList release. This version mainly brings some fixes in the user interface, together with some few improvements.

## Changelog

* A bug has been fixed which sometimes prevented users from reordering tasks (and possible todos).
* Fixed some issues where labels of text would not wrap on smaller displays.
* Inside the schedule view now also a sync indication is shown.
* Removed misleading "nothing here yet" labels in the library view when you have a search which does not match any item.
* A sync is now triggered when items are deleted.
* A "type" icon is now shown in the library side bar, indicating if its a local or synced library.
* Improved scrolling within the notes editor on Android.
* Fixed a bug that when deleting a todo list the page would remain open.


{% capture images1 %}
    /images/v3.6/library_symbols.png
{% endcapture %}
{% include gallery images=images1 caption="There is now a symbol next to each library indicating if it is a local one or synced to some servers." %}


## Downloads

Please find the download links for major platforms on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.6.0). For Android, the release is available via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).


## Known Issues

* For Windows, two flavors of the app are provided: One flavor is cross-compiled binaries (i.e. the Windows release files are actually built on Linux, which makes deployment much easier). However, these binaries require a working OpenGL driver, which sometimes is not the case (especially, when running Windows inside a Virtual Machine). In this case, please use the installers which are built natively on Windows (the ones which have the **MSVC** part in their name).