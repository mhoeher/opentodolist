---
layout: post
title: OpenTodoList 2.0.4
modified:
categories: 
description:
tags: [Beta]
image:
  feature:
  credit:
  creditlink:
comments:
share:
date: 2016-02-28T00:00:00+01:00
---

We recently started to write a major rewrite of OpenTodoList. Rewrite this time is literal, the code base basically has been rewritten from scratch. The goal was to simplify the code structure itself as well as moving storage of items to files in the local file system. The idea behind is simple: One can use any sync service (e.g. the ownCloud desktop client) to synchronize libraries to a cloud service.

{% capture images %}
    /images/OpenTodoList_v2.png
{% endcapture %}
{% include gallery images=images caption="OpenTodoList v2: Library Contents View." cols=1 %}

 Speaking of libraries and items: OpenTodoList now also supports new types of top level items - **Notes** and **Images**. Notes are simple text snippets (think of sticky notes) whereas images are images which can have a custom title and also some attached notes. Currently, there are no release binaries for desktop platforms available, so if you'd like to test, please either compile the app from sources or wait a few days until all tasks for the upcoming 2.1 release milestone are met. If you happen to have an Android device, feel free to follow the [beta testing link](https://play.google.com/apps/testing/net.rpdev.opentodolist) and join the tester community. After joining, you'll be able to install OpenTodoList straight from the Google Play Store. Have a lot of fun!