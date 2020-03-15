TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

# DEFINES += m_debug

INCLUDEPATH += C:/Users/78729/Desktop/ffmpeg/include \

LIBS += -LC:/Users/78729/Desktop/ffmpeg/lib \
        -lavcodec \
        -lavformat \
        -lavutil \
        -lswscale \
        -lws2_32

SOURCES += \
        main.cpp \
    httprequest.cpp \
    ffmpegavdecoder.cpp \
    im_buff.cpp

HEADERS += \
    httprequest.h \
    ffmpegavdecoder.h \
    im_buff.h
