---
layout: post
title: Refactoring, Refactoring, Refactoring...
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
date: 2014-05-11T00:00:00+02:00
---

So it has been quite a while since the last update on OpenTodoList. But - as always - this by far does not mean there's nothing going one. In fact, especially recently there's been a lot of refactoring, both in the app's backend infrastructure and the user interface. The results... a hopefully even more flexible architecture that'll allow easy implementation of new backends (there's still only the XML backend which stores todos locally, but that'll change soon hopefully) and have a more resource friendly behavior of the app in general. Also, the UI has been redone completely to have to ensure it is running better on smartphone (and tablet) devices. But also on a Desktop computer OpenTodoList should better adapt to a user's settings. For example, the font size (and thus generally size of any graphical component) is taken from global settings. Except for some "minor" things, most of the features of the old implementation are available meanwhile. So, if you would like to give it a test, just install the [APK on your Android device by clicking here](http://rpdev.net/public/opentodolist/OpentTodoList-armv7-debug.apk) (Note: You'll have to enable installation of apps from unknown locations - sorry for that, this also should be solved sometime soon). If you are on a Desktop system, you'll have to compile the binaries on your own. The sources can be found on [Gitorious](https://gitorious.org/opentodolist). You should use Qt 5.2.1 (or later). By the way: In theory the app should also work on iOS and on Blackberry devices... but, that's untested ;) Should you have the chance to try out compilation and testing, I'd appreciate any feedback). Finally (as usual) some screenshots on the new version :)


{% capture images %}
    /images/otl-refactoring-todolistview.png
    /images/otl-refactoring-tododetails.png
    /images/otl-refactoring-editor.png
    /images/otl-refactoring-datepicker.png
{% endcapture %}
{% include gallery images=images caption="Several examples of the refactored user interface." cols=4 %}
