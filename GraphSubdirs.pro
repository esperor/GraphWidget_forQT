TEMPLATE = subdirs

SUBDIRS += \
    GraphApp \
    GraphLib \
    GraphTests

GraphApp.depends = GraphLib GraphTests
GraphTests.depends = GraphLib
