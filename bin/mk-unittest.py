#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Create a new unit test.
"""

from os import makedirs
from os.path import dirname
from os.path import join

from sys import argv

from mako.template import Template

HELP = """\
Create a new unit test.

Usage:
    {program} UnitTestName

Example:

    {program} SomeClass
"""

PRO = """\
include(../../config.pri)
setupTest(${name.lower()})

include(../../lib/lib.pri)

SOURCES += \
    test_${name.lower()}.cpp
"""

CPP = """\
#include "${name.lower()}.h"

#include <QObject>
#include <QTest>

class ${name}Test : public QObject
{
  Q_OBJECT

private slots:

  void initTestCase() {}
  void init() {}
  void cleanup() {}
  void cleanupTestCase() {}
};


QTEST_MAIN(${name}Test)
#include "test_${name.lower()}.moc"
"""


def _help(program):
    print(HELP.format(program=program))
    exit(1)


def _render(testname):
    unittests_dir = join(dirname(dirname(__file__)), "test")
    unittest_dir = join(unittests_dir, testname.lower())

    pro_file = join(unittest_dir, testname.lower() + ".pro")
    cpp_file = join(unittest_dir, "test_" + testname.lower() + ".cpp")

    tests_pro = join(unittests_dir, "test.pro")

    makedirs(unittest_dir, exist_ok=False)

    with open(pro_file, "w") as file:
        file.write(Template(PRO).render(name=testname))

    with open(cpp_file, "w") as file:
        file.write(Template(CPP).render(name=testname))

    with open(tests_pro) as file:
        tests_pro_conrent = file.read(-1)
    tests_pro_conrent += "SUBDIRS += " + testname.lower() + "\n"
    with open(tests_pro, "w") as file:
        file.write(tests_pro_conrent)


if __name__ == '__main__':
    if len(argv) != 2:
        _help(argv[0])
    else:
        _render(argv[1])
