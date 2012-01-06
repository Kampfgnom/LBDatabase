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
    attributevalue.h \
    relation.h \
    property.h \
    propertyvalue.h \
    relationvalue.h \
    relationvalue_p.h \
    relationvalueright.h \
    relationvalueleft.h

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
    attributevalue.cpp \
    relation.cpp \
    property.cpp \
    propertyvalue.cpp \
    relationvalue.cpp \
    relationvalueright.cpp \
    relationvalueleft.cpp

OTHER_FILES +=

