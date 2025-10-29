

QT += core gui concurrent charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# OpenCV настройки (проверьте пути)
macx {
    # Для Homebrew на Apple Silicon
    INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4
    LIBS += -L/opt/homebrew/opt/opencv/lib \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_imgcodecs \
        -lopencv_video \
        -lopencv_objdetect
} else {
    # Для других систем
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abstractui.cpp \
    cameramodewidget.cpp \
    factoryui.cpp \
    filemodewidget.cpp \
    fromcamui.cpp \
    fromfileui.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    abstractui.h \
    cameramodewidget.h \
    factoryui.h \
    filemodewidget.h \
    fromcamui.h \
    fromfileui.h \
    mainwindow.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
