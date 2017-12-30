---
layout: post
title: OpenTodoList 3.3 Has Been Released
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
date: 2017-11-30T00:00:00+01:00
---

While the previous releases introduced great new features like the built-in WebDAV sync as well as major changes e.g. in the user interface (at least under the hood), v3.3 focuses on stabilizing, further automating the build process of the app as well as some smaller user experience improvements. However, some goodies made it in as well of course. 

{% capture images %}
    /images/Screenshot_20171130_224148.png
{% endcapture %}
{% include gallery images=images caption="Drag&drop of todos in action." cols=1 %}

Changelog
---------

*   **Allow drag&drop reordering of todos and tasks**: You can now re-order todos and tasks by long pressing and then moving them. That way, you can prioritize jobs for your daily work or put stuff in your grocery list into the right order.
*   Context menus and some dialogs are now correctly positioned - especially in larger todo or task lists.
*   The tags in the side bar should now always be visible after starting the app (sometimes they were not and you had to restart the app for all tags to appear).
*   Inside a todo list, when a todo contains one or more tasks, the progress (i.e. ratio of open vs. closed tasks) is visualised.
*   **You can now attach files to items:** That way, you can attach any pictures, documents or whatever else you have to items and open the attachments from within the item's page.
*   When deleting a library which is located in the default location, the library files are removed from disk now.
*   **The AppImage now integrates into your desktop environment:** When starting the first time, you are asked if you want to create integrations, which allow you to start the app e.g. from the applications menu.
*   A running sync is now properly cancelled e.g. when you exit the application.
*   When a sync fails, you now get a notification about this.
*   **Clickable links in item titles:** You can now use links in item titles - they will automatically be clickable to open your browser from within the app.
*   The Windows version is now available in a version compiled via Microsoft Visual Studio: If the app crashes on your system during startup, this could be due to issues with the OpenGL implementation of your graphics device. The MSVC version should work around the issue by falling back to DirectX if required.
*   Tasks inside a todo can now be filtered.
*   When installing a new version of the app on Windows, the previous installation is now cleanly removed before the new install.
*   There is now a sync log integrated into the app - this is mainly for debugging purposes on case something goes wrong.
*   Finally, there are quite some changes to further automate the build process on Windows and macOS, ensuring that releases are created in a consistent way all the time.

Downloads
---------

Please find the download links for major desktop platforms on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.3.0). For Android, the release is available via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).