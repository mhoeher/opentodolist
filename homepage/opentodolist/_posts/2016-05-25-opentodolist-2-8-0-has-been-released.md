---
layout: post
title: OpenTodoList 2.8.0 Has Been Released
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
date: 2016-05-25T00:00:00+02:00
---

We are proud to present another release of OpenTodoList. As always, please head over to the [application page](https://www.rpdev.net/wordpress/apps/opentodolist/) to find an installer for your platform.

Changelog
=========

{% capture images %}
    /images/OpenTodoList_items_style-300x241.png
{% endcapture %}
{% include gallery images=images caption="Notes, images and todo lists now visually differ from each other inside the library view." cols=1 %}

The visually most noticeable change in version 2.8 is that the items inside a library now visually differ much more from each other than before. In earlier versions, all items were represented by a small sticky note, the various types of items having different contents of course. Still, e.g. the difference between notes and todo lists was sometimes not very clear. This changes now, with the three kinds of items each using their own, distinct styling. Besides this visual improvement, there are some more changes as well. Most important is a bug fix in the library sidebar: In compact mode (i.e. when you had to slide in the bar from the left side of the screen), it was not possible to scroll vertically. This might have gone unnoticed for most users, but as soon as one uses more libraries and tags inside each, the list of items to be displayed grows and then this bug definitely hurt. Editing of item titles also got some refinement. Editing now is done in a separate dialog. This improves the text input a lot especially on Android, where before editing of item titles was kind of difficult with very long titles. Last but not least, another new feature made it into this version: Inside a library, items now have a context menu, which can be used to quickly interact with the items without having to change into their details. You can just right click an item to rename it, delete it, manage assigned tags and change the color of an item!