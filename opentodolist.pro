TEMPLATE = subdirs
CONFIG += ordered

qrc.depends =
qrc.commands = perl $$PWD/bin/mk-qrc.pl \
               --dir $$PWD/OpenTodoList/qml/ \
               --base $$PWD/OpenTodoList/ \
               --out $$PWD/OpenTodoList/OpenTodoList.qrc
QMAKE_EXTRA_TARGETS += qrc

OTHER_FILES += \
  README.md \
  COPYING \
  Doxyfile \
  doc/installers-howto.md \
  .gitlab-ci.yml \
  bin/linux-deploy-qt.pl \
  bin/mk-installer.pl \
  bin/mk-pri.pl \
  bin/mk-qrc.pl \
  bin/mk-linux-installer-x86_64.sh \
  templates/installer/config/config.xml \
  templates/installer/config/controller.js \
  templates/installer/packages/net.rpdev.OpenTodoList/meta/script.js \
  templates/installer/packages/net.rpdev.OpenTodoList/meta/package.xml


SUBDIRS += \
    lib \
    app \
    test

include(config.pri)
# include(installer.pri)
