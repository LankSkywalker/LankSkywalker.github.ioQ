QT       += core network xml sql

lessThan(QT_MAJOR_VERSION, 5) {
    QT   += gui
} else {
    QT   += widgets
}

macx {
    TARGET = Mupen64Plus
} else {
    TARGET = mupen64plus
}

TEMPLATE = app
macx:ICON = dist/macosx/mupen64plus.icns
win32:RC_FILE = dist/windows/icon.rc


SOURCES += src/main.cpp \
    src/cheatparse.cpp \
    src/common.cpp \
    src/core.cpp \
    src/mainwindow.cpp \
    src/error.cpp \
    src/plugin.cpp \
    src/sdl.cpp \
    src/settings.cpp \
    src/config/configcontrolcollection.cpp \
    src/config/keyspec.cpp \
    src/dialogs/aboutguidialog.cpp \
    src/dialogs/cheatdialog.cpp \
    src/dialogs/cheattree.cpp \
    src/dialogs/configeditor.cpp \
    src/dialogs/downloaddialog.cpp \
    src/dialogs/gamesettingsdialog.cpp \
    src/dialogs/inputdialog.cpp \
    src/dialogs/logdialog.cpp \
    src/dialogs/pluginconfigdialog.cpp \
    src/dialogs/settingsdialog.cpp \
    src/emulation/emulation.cpp \
    src/emulation/emuthread.cpp \
    src/emulation/glwindow.cpp \
    src/emulation/vidext.cpp \
    src/osal/osal_dynamiclib.c \
    src/roms/romcollection.cpp \
    src/roms/thegamesdbscraper.cpp \
    src/views/gridview.cpp \
    src/views/listview.cpp \
    src/views/tableview.cpp \
    src/views/widgets/clickablewidget.cpp \
    src/views/widgets/treewidgetitem.cpp

HEADERS += src/global.h \
    src/cheatparse.h \
    src/common.h \
    src/core.h \
    src/mainwindow.h \
    src/error.h \
    src/plugin.h \
    src/sdl.h \
    src/settings.h \
    src/config/configcontrolcollection.h \
    src/config/keyspec.h \
    src/dialogs/aboutguidialog.h \
    src/dialogs/cheatdialog.h \
    src/dialogs/cheattree.h \
    src/dialogs/configeditor.h \
    src/dialogs/downloaddialog.h \
    src/dialogs/gamesettingsdialog.h \
    src/dialogs/inputdialog.h \
    src/dialogs/logdialog.h \
    src/dialogs/pluginconfigdialog.h \
    src/dialogs/settingsdialog.h \
    src/emulation/emulation.h \
    src/emulation/emuthread.h \
    src/emulation/glwindow.h \
    src/emulation/vidext.h \
    src/osal/osal_dynamiclib.h \
    src/roms/romcollection.h \
    src/roms/thegamesdbscraper.h \
    src/views/gridview.h \
    src/views/listview.h \
    src/views/tableview.h \
    src/views/widgets/clickablewidget.h \
    src/views/widgets/treewidgetitem.h

RESOURCES += resources/mupen64plus.qrc

FORMS += src/dialogs/gamesettingsdialog.ui \
    src/dialogs/settingsdialog.ui \
    src/dialogs/inputdialog.ui

TRANSLATIONS += resources/locale/fr.ts

win32|macx|linux_quazip_static {
    CONFIG += staticlib
    DEFINES += QUAZIP_STATIC
    LIBS += -lz

    #Download quazip source and copy the quazip directory to project as quazip5
    SOURCES += quazip5/*.cpp
    SOURCES += quazip5/*.c
    HEADERS += quazip5/*.h
} else {
    lessThan(QT_MAJOR_VERSION, 5) {
        LIBS += -lquazip
    } else {
        # Debian distributions use a different library name for Qt5 quazip
        system("which dpkg > /dev/null 2>&1") {
            system("dpkg -l | grep libquazip-qt5-dev | grep ^ii > /dev/null") {
                LIBS += -lquazip-qt5
            } else {
                LIBS += -lquazip5
            }
        } else {
            LIBS += -lquazip5
        }
    }
}

INCLUDEPATH += /usr/include/SDL2
LIBS += -lSDL2

INCLUDEPATH += /usr/local/include/mupen64plus
LIBS += -lmupen64plus
LIBS += -ldl
CONFIG += c++11
