---
layout: page
title: About
share: true
---

# About OpenTodoList

OpenTodoList is a todo list and note taking application. It is designed to run on a variety of operating systems, including both Desktop systems like Linux, macOS and Windows as well as mobile systems such as Android.

## Getting Organized: Libraries, Todo Lists, Notes and Images

In OpenTodoList, you organize your information in *libraries*. A library can contain arbitrary many items. Currently, you can create todo lists, notes and images.

**Todo Lists** are the most complex type of items. They can in turn contain arbitrary todos. Each todo in turn can have arbitrary many tasks. Todo lists are a great way to track your daily tasks, be it on work to stay organize your day to day work items or privately, e.g. to maintain your shopping list.

**Notes** are a great way to store arbitrary textual information in a library, like code or other text snippets or simple reminders. They are useful when the information your want to store does not need to be broken down further.

**Images** let you store pictures if your library. For example, if you wrote something on a whiteboard, slate or a sheat of paper, just take a photo and add it to your library.


## Synchronize your Data with NextCloud, ownCloud and Other Services

Nowadays, we not only work with one device but many: People usually have a private computer or laptop. In addition, a lot of us have a second device for their work. And finally, we also use smartphones and tablets a lot. To be really useful, it is important that todo list and notes can be synchronized across these devices.

OpenTodoList provides several strategies to achive synchronization. First of all, the app has support for some cloud platforms built in. Currently, we support the following servers to synchronize your data:

* **NextCloud.**
* **ownCloud.**
* **Generic WebDAV servers.** Note that there are a lot of different WebDAV implementations out there, some of them not following the WebDAV standard as closely as others. Hence, if you encounter issues synchronizing with some servers, please report these incidents.

In addition, as a Library in OpenTodoList is simply a directory consisting of several files and sub-directories, you can use any third party synchronization tool (e.g. DropBox) to synchronize it between your devices.


## Keep Control of Your Data

In contrary to some other todo list applications, OpenTodoList does not force you to use a specific cloud provider to synchronize your data. We support NextCloud and ownCloud as built in sync providers. You can host these services on your own or other, trusted hardware. In addition, as libraries are just directories and files, you can use any trusted third party sync service of your choice.

This makes OpenTodoList a great app when it is important to prevent data to be stored on potentially untrusted cloud services (which often is the case with confitential company information).


## Open Source

![GNU GPL v3]({{ site.url }}/images/gplv3-127x51.png)
{: .image-right}

OpenTodoList is licensed under the terms of the [GNU General Public License, Version 3 or later](https://www.gnu.org/licenses/gpl-3.0-standalone.html). This means that everyone can get the source code of the application, study it to understand what it is doing (and if it is complying to their specific requirements) and apply changes on their own. Want to add support for another cloud storage provider? Have some ideas on how to improvde the user interface? Just head over to GitLab and [fork the project to get started](https://gitlab.com/rpdev/opentodolist/forks/new).