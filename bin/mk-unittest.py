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

CMAKE = """\
find_package(Qt${QT_VERSION_MAJOR}Test REQUIRED)
add_executable(
    test_${name.lower()}

    test_${name.lower()}.cpp
)

target_link_libraries(
    test_${name.lower()}

    # Internal libraries:
    opentodolist-common

    # Qt Libraries
    Qt::Test
)

add_test(
    NAME ${name.lower()}
    COMMAND $<TARGET_FILE:test_${name.lower()}> -platform minimal
)
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

    cmake_file = join(unittest_dir, "CMakeLists.txt")
    cpp_file = join(unittest_dir, "test_" + testname.lower() + ".cpp")

    tests_cmake = join(unittests_dir, "CMakeLists.txt")
    tests_pro = join(unittests_dir, "test.pro")

    makedirs(unittest_dir, exist_ok=False)

    with open(cmake_file, "w") as file:
        file.write(Template(CMAKE).render(name=testname))

    with open(cpp_file, "w") as file:
        file.write(Template(CPP).render(name=testname))

    with open(tests_cmake) as file:
        tests_cmake_content = file.read(-1)
    tests_cmake_content += "add_subdirectory(" + testname.lower() + ")\n"
    with open(tests_cmake, "w") as file:
        file.write(tests_cmake_content)


if __name__ == '__main__':
    if len(argv) != 2:
        _help(argv[0])
    else:
        _render(argv[1])
