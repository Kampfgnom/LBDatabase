TARGET = lbdatabase
TEMPLATE = lib
CONFIG += staticlib
QT += sql
DEFINES *= QT_USE_QSTRINGBUILDER
INCLUDEPATH += $$PWD/include
DESTDIR = $$PWD/../lib

HEADERS += \
    old/taskscheduler.h \
    old/table.h \
    old/row.h \
    old/mappingattribute.h \
    old/listattribute.h \
    old/databaseattribute.h \
    old/database.h \
    old/attribute.h \
    old/singleton.h \
    old/tablemodel.h \
    old/relation.h \
    old/attributeowner.h \
    old/relationindex.h \
    \
    LBDatabase.h \
    database.h \
    table.h \
    singleton.h \
    attribute.h \
    row.h \
    databaseattribute.h \
    relation.h \
    tablemetadata.h \
    attributemetadata.h \
    databaseattributemetadata.h

SOURCES += \
    old/taskscheduler.cpp \
    old/table.cpp \
    old/row.cpp \
    old/mappingattribute.cpp \
    old/listattribute.cpp \
    old/databaseattribute.cpp \
    old/database.cpp \
    old/attribute.cpp \
    old/singleton.cpp \
    old/tablemodel.cpp \
    old/relation.cpp \
    old/attributeowner.cpp \
    old/relationindex.cpp \
    \
    database.cpp \
    table.cpp \
    attribute.cpp \
    row.cpp \
    databaseattribute.cpp \
    relation.cpp \
    tablemetadata.cpp \
    attributemetadata.cpp \
    databaseattributemetadata.cpp

