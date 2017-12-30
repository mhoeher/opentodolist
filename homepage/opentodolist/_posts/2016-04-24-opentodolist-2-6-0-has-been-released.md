---
layout: post
title: OpenTodoList 2.6.0 Has Been Released
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
date: 2016-04-24T00:00:00+02:00
---

We are proud to announce the release of OpenTodoList 2.6.0. Once more, this is a release with fixes some issues present in past versions, but also brings significant new features. Head over to the [app page](https://www.rpdev.net/wordpress/apps/opentodolist/) to find the download links for your platform.

Changelog
=========

The previous version of the app introduced tags as a means for organizing your libraries. Version 2.6 builds on top of this feature. When you currently view the items in a tag and create a new item, than the current tag is automatically attached to this new item. Also, the tag completion has been fixed for todo list and image items, where previously the completion was not implemented. Finally, the app now also caches the used tags in a library. You will notice this when restarting the app: If you have more than one library, then previously you first had to activate a library before the used tags in it where available as options in the sidebar.

{% capture images %}
    /images/OpenTodoList_searching_in_todolist.png
{% endcapture %}
{% include gallery images=images caption="Searching inside a todo list." cols=1 %}

Besides tags, there is another big set of changes that comes with this new release: Searching functionality is finally back. Tags already were intended to make working with lots of items in a library easier. However, sometimes this is not enough. In this new release you can search items in both a library as well as inside a todo list page. Searching also works recursively, that means if your criteria matches a task, than the todo and todo list will be shown as matching items when you trigger a search in the appropriate pages. There also were some minor and not so minor UI fixes and changes. Most notably is probably the way todos and tasks now are created. Instead of using a shortcut or pressing a tool button, the list of todos/tasks now have an inline editor that lets you create new items. This significantly reduces the number of clicks or keyboard shortcuts required to create new todos or tasks - especially when creating several items in a go. Additionally, the list of todos in a todo list is now sorted by the todos' done status. Finished todos are hidden by default with an option to show them when required, so you can concentrate on your open work items.