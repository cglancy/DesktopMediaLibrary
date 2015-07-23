TEMPLATE = app

QT += qml quick network widgets

SOURCES += main.cpp \
    maincontroller.cpp \
    video.cpp \
    categorynode.cpp \
    utility.cpp \
    treemodel.cpp \
    listmodel.cpp \
    textindex.cpp \
    videofile.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    maincontroller.h \
    video.h \
    categorynode.h \
    utility.h \
    treemodel.h \
    listmodel.h \
    textindex.h \
    videofile.h
