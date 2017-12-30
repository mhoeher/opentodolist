---
layout: post
title: OpenTodoList 2.4.0 Has Been Released
modified:
categories: 
description:
tags: [Release, Windows, Linux, macOS]
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2016-04-10T00:00:00+02:00
---

We are proud to present version 2.4.0 of OpenTodoList. This version fixes some bugs as well as introduces new features and improvements to the app. Please head over to the [OpenTodoList page](https://www.rpdev.net/wordpress/apps/opentodolist/) to find the download links.

Changelog
=========

*   Fixed selection of folders (for the **Open Local Library **functionality) - this was not working after some refactoring in the last version.
*   The **About Application** page now also shown the version number of the app.
*   Added a check which prevents item from being shown multiple times - this could happen when you are using some sync clients (such as the ownCloud desktop client) to sync your OpenTodoList library with the cloud in the background.
*   Fixed a bug when creating new items on Android (potentially an incomplete title was used).
*   Auto-saving of notes: When editing an item's notes, your work is now saved on the fly, preventing potential data loss.
*   Improvements in the sidebar: The sidebar has seen some general overhaul. Additionally, it compact mode you can slide it it from the screen edge.
*   Added a check to prevent wrong library syncs (could cause issues when you have multiple synced libraries).
*   On the library page, todo lists now only show undone todos.