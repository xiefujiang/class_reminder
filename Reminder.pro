QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    config_page.cpp \
    declare.cpp \
    main.cpp \
    reminder_class.cpp \
    unit_edit_page.cpp

HEADERS += \
    config_page.h \
    declare.h \
    reminder_class.h \
    unit_edit_page.h

FORMS += \
    config_page.ui \
    declare.ui \
    reminder_class.ui \
    unit_edit_page.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
