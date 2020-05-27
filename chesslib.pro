QT += gui

CONFIG += c++11 console
CONFIG -= app_bundle

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        board.cpp \
        ecocode.cpp \
        game.cpp \
        game_node.cpp \
        gui_printer.cpp \
        main.cpp \
        move.cpp \
    node_pool.cpp \
        pgn_printer.cpp \
        pgn_reader.cpp \
        polyglot.cpp \
    testcases.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    constants.h \
    arrow.h \
    arrow.h \
    board.h \
    colored_field.h \
    constants.h \
    ecocode.h \
    game.h \
    game_node.h \
    gui_printer.h \
    move.h \
    node_pool.h \
    pgn_printer.h \
    pgn_reader.h \
    polyglot.h \
    testcases.h
