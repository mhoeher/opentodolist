---
layout: post
title: OpenTodoList 3.4 Has Been Released
modified:
categories: 
description:
tags: [Release, Linux, Windows, macOS, Android]
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2017-12-28T00:00:00+01:00
---

Just in time before the end of 2017, here comes version 3.4 of OpenTodoList. Besides some usability and user interface improvements, there's one big change: Notes in items are now styled using [Markdown](https://daringfireball.net/projects/markdown/syntax).

Changelog
---------

*   Notes within items can not be formatted using Markdown syntax. This allows to keep the user interface less cluttered while at the same time you have the possibility to style text if needed. Often, it is even quicker to use Markdown than having to use some controls to style text. Just give it a try ;-)
*   The change towards Markdown also implicilty fixes other issues like font sizing problems when using synchronized libraries across several devices. For example, if you created a note with a heading on your HighDPI smartphone and viewed the same later on your medium resolution laptop, the heading would be displayed far too large. This is history now.
*   A todo can now be marked as done or undone from within its page. Previously, if you had a todo open you had to go back to the todo list page and tick/untick the todo there to change its state.
*   The app is now based on Qt 5.10. For you as an end user, this is probably less interesting, but enables some of the other features that are coming with this version.
*   On Windows, the app now uses the Universal style.
*   On all other desktop platforms, the app uses the new Fusion style. This style is a more desktop oriented style, which - in addition - adapts to your set color scheme. For example, if you selected a dark color scheme for your desktop, OpenTodoList now goes black as well!
*   Some fixes and improvements adding new todos and tasks: First, when using the "plus" button to add a new todo or task, the text in the input field was not deleted on Android. In addition, the input field keeps keyboard focus after adding a new todo or task, which makes adding a larger set of new entries much easier.
*   After adding a new library, an initial sync is now run immediately. This makes sure that a newly created synced library is also created on the server. Existing libraries are pulled down, so they appear immediately after the sync finished.
*   The menu bar is back! On desktop systems, there's now a basic menu bar with the most important entries.
*   Dialogs to rename items can now be finished by pressing the Enter or Return key on the keyboard.
*   HTML sequences in item titles are now properly escaped.
*   The app should no longer show update notifications on Android: These were unintentional, because on Android we anyhow get updates via the Play store.

Downloads
---------

Please find the download links for major desktop platforms on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.4.0). For Android, the release is available via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).
