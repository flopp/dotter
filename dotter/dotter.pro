QT += core gui widgets svg

TARGET = dotter

TEMPLATE = app

SOURCES += \
    abortwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    svgview.cpp

HEADERS += \
    abortwidget.h \
    mainwindow.h \
    svgview.h

FORMS += \
    abortwidget.ui \
    mainwindow.ui
