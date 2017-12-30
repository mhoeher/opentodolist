---
layout: post
title: "Time for Testing: Some Android Binaries of OpenTodoList"
modified:
categories: 
description:
tags: [News]
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2013-11-03T00:00:00+01:00
---

After the porting of OpenTodoList to the upcoming Qt 5.2 library, I managed to resume to reducing the number of entries in the [OpenTodoList issue tracker](https://gitlab.com/rpdev/opentodolist/issues). The result: A much improved OpenTodoList especially with fixes for Android ;) So, except of a ["minor issue"](https://bugreports.qt-project.org/browse/QTBUG-32878) in Qt 5.2 itself, it should work now in a halfway usable way. If you like to test it on your own - I prepared some ready to use binaries that can be installed on Android. Right now (well... sorry, I'm biased here) these are only available for ARM v7. So, if you feel ready just [download the APK](http://rpdev.net/public/opentodolist/OpentTodoList-armv7-debug.apk) and install it. Just some notes:

*   OpenTodoList is still very young. Use it on your own risk ;) Should you stumble over some bugs (or things you'd like to see improved), please don't hesitate to visit the issue tracker mentioned above.
*   The APK is not specially signed. Apart from allowing installation from unknown sources, you will also have to accept unsigned packages. Sorry for that, I will look into this next.

Please let me know in case you run into any problems with testing and have a lot of fun :)