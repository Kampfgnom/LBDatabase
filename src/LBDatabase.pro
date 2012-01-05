TARGET = lbdatabase
TEMPLATE = lib
CONFIG += staticlib
QT += sql
DEFINES *= QT_USE_QSTRINGBUILDER
INCLUDEPATH += $$PWD/include
DESTDIR = $$PWD/../lib

HEADERS += \
    LBDatabase.h \
    column.h \
    database.h \
    row.h \
    table.h \
    entity.h \
    storage.h \
    context.h \
    entitytype.h \
    attribute.h \
    attributevalue.h

SOURCES += \
    column.cpp \
    database.cpp \
    row.cpp \
    table.cpp \
    entity.cpp \
    storage.cpp \
    context.cpp \
    entitytype.cpp \
    attribute.cpp \
    attributevalue.cpp

OTHER_FILES +=

