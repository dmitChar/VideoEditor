# Добавьте эти строки в НАЧАЛО файла:
TARGET = videoEditor
TEMPLATE = app

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

macx {
    # Определяем версию macOS
    MACOSX_VERSION = $$system("sw_vers -productVersion")
    isEmpty(MACOSX_VERSION) {
        MACOSX_VERSION = 12.0
    }

    QMAKE_MACOSX_DEPLOYMENT_TARGET = $$MACOSX_VERSION

    # Явно указываем использовать системный SDK
    QMAKE_APPLE_DEVICE_ARCHS = x86_64 arm64
    QMAKE_MAC_SDK = macosx

    # Флаги для игнорирования устаревших фреймворков
    QMAKE_LFLAGS += -Wl,-w
}
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc
