---
layout: post
title: OpenTodoList 3.6.1 Has Been Released
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
date: 2018-02-28T00:00:00+01:00
---

With the previous release just being one day old, here comes a bug fix release.

## Changelog

* It seems that the AppImage is not working well on Linux distributions with more recent software selections. In particular, newer OpenSSL versions seems to cause issues and prevent e.g. synchronization against a WebDAV server secured with HTTPS. This new release includes a compatible OpenSSL version within the AppImage, so most HTTPS connections should work out of the box.

As you can see, this release is only relevant for Linux AppImage users. Other users can upgrade as well, but there are no changes to be expected for them.


## Downloads

Please find the download links for major platforms on [GitHub](https://github.com/mhoeher/opentodolist/releases/tag/3.6.1). For Android, the release is available via [Google Play](https://play.google.com/store/apps/details?id=net.rpdev.opentodolist).


## Known Issues

* For Windows, two flavors of the app are provided: One flavor is cross-compiled binaries (i.e. the Windows release files are actually built on Linux, which makes deployment much easier). However, these binaries require a working OpenGL driver, which sometimes is not the case (especially, when running Windows inside a Virtual Machine). In this case, please use the installers which are built natively on Windows (the ones which have the **MSVC** part in their name).
