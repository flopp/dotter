QT += core gui widgets svg

TARGET = dotter

TEMPLATE = app

SOURCES += \
    abortwidget.cpp \
    app.cpp \
    config.cpp \
    layoutProcess.cpp \
    main.cpp \
    mainwindow.cpp \
    svgview.cpp

HEADERS += \
    abortwidget.h \
    app.h \
    config.h \
    layoutProcess.h \
    mainwindow.h \
    svgview.h

FORMS += \
    abortwidget.ui \
    mainwindow.ui
