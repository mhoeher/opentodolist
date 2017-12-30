---
layout: post
title: "A Short Sign of Life: Near to Feature Parity"
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
date: 2014-06-26T00:00:00+02:00
---

Well... it's again some time since the last update on OpenTodoList. Back then, the status was that the app was coming from a phase of refactoring. That usually means... regressions (feature wise). Good news is... meanwhile we're nearly where we were before the restructuring :) Having a look at OpenTodoLists' [Issue Tracker](https://gitlab.com/rpdev/opentodolist/issues), nearly all issues for re-implementing one of the previous features are solved. In fact, only the "moving of todos" one is still open.

{% capture images %}
    /images/otl-scheduled-todos.png
{% endcapture %}
{% include gallery images=images caption="Scheduled Todos" cols=1 %}

But not only "old" features get in; also some new stuff, like e.g. the _scheduled todos_ view: It shows in a condensed way the todos due to the current day and week as well as some of the todos scheduled for later. Apart from implementing the remaining open issues (and fixing bugs, of course), my personal roadmap somehow looks like this:

*   Some kind of beta phase :) I want to provide pre-compiled binaries not only for Android but also Windows and Linux (and maybe others). That way, maybe the one or other could join testing the app.
*   Start implementing the ownCloud backend: This was actually the big target I had in mind when starting OpenTodoList. That would finally allow to safely store your todos on you home server as well as share with others.

Finally, the obligatory [Download Link for the Android ARMv7 APK](http://rpdev.net/public/opentodolist/OpentTodoList-armv7-debug.apk), just in case you happen to have such a device ;)