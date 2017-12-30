---
layout: post
title: OpenTodoList 2.7.0 Has Been Released
modified:
categories: 
description:
tags: []
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2016-05-10T00:00:00+02:00
---

We just released version 2.7.0 of OpenTodoList. Please find the various download links at the [app page](https://www.rpdev.net/wordpress/apps/opentodolist/). This release mostly fixes some bugs, but also introduces some changes in the user interface:

Changelog
=========

The list of tags in a library did not get properly updated. This bug is effective only when using e.g. a sync client to access a library across various devices. In addition, there were some minor fixes in the user interface affecting the library side bar as well as the todo and task listings. The notes editor also received some updates, making formatting of text more robust and fixing some bugs there as well. Most notably, if you tried to format some text which is HTML/XML formatted, the tags have been removed - this behavior is gone now. The library side bar now also has a shortcut to the help page. This allowed us to remove the menu completely for touch devices (currently only Android), as it anyway was not very useful on these platforms.

{% capture images %}
    /images/OpenTodoList_text_editor.png
{% endcapture %}
{% include gallery images=images caption="The note editor component in action." cols=1 %}

Finally, re-scanning of libraries has been improved a bit: If a change in one file is detected, a short delay has been added, so that if e.g. a sync is running in the background, the operation can complete before the app tries to re-load the items from disk.
