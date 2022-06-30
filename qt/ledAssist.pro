QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qblepage.cpp \
    qctrlpage.cpp \
    qwifipage.cpp

HEADERS += \
    mainwindow.h \
    qblepage.h \
    qctrlpage.h \
    qwifipage.h

FORMS += \
    mainwindow.ui \
    qblepage.ui \
    qctrlpage.ui \
    qwifipage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

#set app icon
RC_FILE += appicon.rc
