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
    table.h

SOURCES += \
    column.cpp \
    database.cpp \
    row.cpp \
    table.cpp

OTHER_FILES +=

