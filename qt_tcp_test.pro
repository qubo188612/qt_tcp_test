QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT       += core gui network

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        client.cpp \
        main.cpp \
        server.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    client.h \
    global.h \
    server.h

#modbustcp库的添加
INCLUDEPATH += /home/qubo/modbus/libmodbus/install/include \
#INCLUDEPATH += /home/qubo/modbus/libmodbus/src

LIBS += /home/qubo/modbus/libmodbus/install/lib/libmodbus.so


FORMS += \
    client.ui \
    server.ui
