---
layout: post
title: OpenTodoList 3.6.2 Has Been Released
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
date: 2018-03-01T00:00:00+01:00
---

This is another bug fix release on top of v3.6.

## Changelog

* We now also distribute non-Qt libraries in the AppImage. This ensures that the app runs on a wider set of Linux systems than before.

Again, this version is not relevant for non-Linux users.


## Downloads

Please find the download links for major platforms on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.6.2). For Android, the release is available via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).


## Known Issues

* For Windows, two flavors of the app are provided: One flavor is cross-compiled binaries (i.e. the Windows release files are actually built on Linux, which makes deployment much easier). However, these binaries require a working OpenGL driver, which sometimes is not the case (especially, when running Windows inside a Virtual Machine). In this case, please use the installers which are built natively on Windows (the ones which have the **MSVC** part in their name).
