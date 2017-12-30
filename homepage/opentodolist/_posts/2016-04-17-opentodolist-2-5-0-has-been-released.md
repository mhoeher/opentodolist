---
layout: post
title: OpenTodoList 2.5.0 Has Been Released
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
date: 2016-04-17T00:00:00+02:00
---

We are proud to present another release of OpenTodoList. Please head over to the [application page](https://www.rpdev.net/wordpress/apps/opentodolist/) to find the download links.

Changelog
=========

This release again brings some fixes in the installer of the application. Hence, especially on Windows you should completely re-install the app in order to ensure that future updates work out of the box. Besides this, there are some minor fixes in the UI here and there: The hover effect was not working properly when the app ran in non-compact mode on Desktops. On Android, when creating todos and tasks potentially the incomplete input text was used as title. Finally, todo titles were not wrapped in the library page. Besides these fixes, this release also brings a great new feature: **Tags**. You can now add arbitrary many tags to each of your top level items. These tags can be used to filter todos: In the library selector, you can not only select one of your libraries but also one of the used tags. If you do, only the items having this tag attached will be shown. This is a great help to keep your library ordered even when the number of items starts to grow.

{% capture images %}
    /images/OpenTodoList_tags_editor.png
    /images/OpenTodoList_tags_without_filtering.png
    /images/OpenTodoList_tags_with_filtering.png
{% endcapture %}
{% include gallery images=images caption="Tags can be used to structure your libraries." cols=3 %}
