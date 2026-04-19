QT       += core gui network concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    PstoeditQtGuiMain.cpp \
    PstoeditQtGui.cpp

HEADERS += \
    PstoeditQtGui.h

FORMS += \
    PstoeditQtGui.ui

RESOURCES += PstoeditQtGui.qrc

LIBS +=  -Wl,-rpath,`pwd`/../../src/.libs/ -L../../src/.libs -lpstoedit
QMAKE_LFLAGS += $$(LDFLAGS)
QMAKE_CFLAGS += $$(CFLAGS)
QMAKE_CXXFLAGS += $$(CXXFLAGS)

INCLUDEPATH += ../../src

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
