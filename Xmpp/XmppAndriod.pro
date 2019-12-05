#-------------------------------------------------
#
# Project created by QtCreator 2019-11-13T17:00:19
#
#-------------------------------------------------

QT       += core gui \
         multimedia \
         sql  xml

#==解决安卓NDK较新版本的库qt可能找不到(libc++)
QMAKE_LINK += -nostdlib++
DEFINES += ANDRIOD
#==================

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleXmpp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

#-----3rd QXMPP-----
DEFINES += WITH_SPEEX=OFF\
        WITH_OPUS=OFF\
        WITH_THEORA=OFF\
        WITH_VPX=OFF\
        BUILD_SHARED=OFF

INCLUDEPATH += qxmpp \
            qxmpp/base \
            qxmpp/client \
            qxmpp/server

SOURCES += \
        ebxmppclient.cpp \
        historyrecords.cpp \
        loginform.cpp \
        imageoutputs.cpp \
        loggerview.cpp \
        main.cpp \
        mainwindow.cpp \
        stackedwidget.cpp \
        uirosterview.cpp \
        rosterdelegate.cpp \
    imageview.cpp

HEADERS += \
        ebxmppclient.h \
        historyrecords.h \
        loginform.h \
        imageoutputs.h \
        loggerview.h \
        mainwindow.h \
        stackedwidget.h \
        uirosterview.h \
        rosterdelegate.h \
    imageview.h

SOURCES += \
        qxmpp/base/QXmppArchiveIq.cpp \
        qxmpp/base/QXmppBindIq.cpp \
        qxmpp/base/QXmppBookmarkSet.cpp \
        qxmpp/base/QXmppByteStreamIq.cpp \
        qxmpp/base/QXmppCodec.cpp \
        qxmpp/base/QXmppConstants.cpp \
        qxmpp/base/QXmppDataForm.cpp \
        qxmpp/base/QXmppDiscoveryIq.cpp \
        qxmpp/base/QXmppElement.cpp \
        qxmpp/base/QXmppEntityTimeIq.cpp \
        qxmpp/base/QXmppHttpUploadIq.cpp \
        qxmpp/base/QXmppIbbIq.cpp \
        qxmpp/base/QXmppIq.cpp \
        qxmpp/base/QXmppJingleIq.cpp \
        qxmpp/base/QXmppLogger.cpp \
        qxmpp/base/QXmppMamIq.cpp \
        qxmpp/base/QXmppMessage.cpp \
        qxmpp/base/QXmppMixIq.cpp \
        qxmpp/base/QXmppMixItem.cpp \
        qxmpp/base/QXmppMucIq.cpp \
        qxmpp/base/QXmppNonSASLAuth.cpp \
        qxmpp/base/QXmppPingIq.cpp \
        qxmpp/base/QXmppPresence.cpp \
        qxmpp/base/QXmppPubSubIq.cpp \
        qxmpp/base/QXmppRegisterIq.cpp \
        qxmpp/base/QXmppResultSet.cpp \
        qxmpp/base/QXmppRosterIq.cpp \
        qxmpp/base/QXmppRpcIq.cpp \
        qxmpp/base/QXmppRtcpPacket.cpp \
        qxmpp/base/QXmppRtpChannel.cpp \
        qxmpp/base/QXmppRtpPacket.cpp \
        qxmpp/base/QXmppSasl.cpp \
        qxmpp/base/QXmppSessionIq.cpp \
        qxmpp/base/QXmppSocks.cpp \
        qxmpp/base/QXmppStanza.cpp \
        qxmpp/base/QXmppStream.cpp \
        qxmpp/base/QXmppStreamFeatures.cpp \
        qxmpp/base/QXmppStreamInitiationIq.cpp \
        qxmpp/base/QXmppStreamManagement.cpp \
        qxmpp/base/QXmppStun.cpp \
        qxmpp/base/QXmppUtils.cpp \
        qxmpp/base/QXmppVCardIq.cpp \
        qxmpp/base/QXmppVersionIq.cpp \
        qxmpp/client/QXmppArchiveManager.cpp \
        qxmpp/client/QXmppBookmarkManager.cpp \
        qxmpp/client/QXmppCallManager.cpp \
        qxmpp/client/QXmppCarbonManager.cpp \
        qxmpp/client/QXmppClient.cpp \
        qxmpp/client/QXmppClientExtension.cpp \
        qxmpp/client/QXmppConfiguration.cpp \
        qxmpp/client/QXmppDiscoveryManager.cpp \
        qxmpp/client/QXmppEntityTimeManager.cpp \
        qxmpp/client/QXmppInvokable.cpp \
        qxmpp/client/QXmppMamManager.cpp \
        qxmpp/client/QXmppMessageReceiptManager.cpp \
        qxmpp/client/QXmppMucManager.cpp \
        qxmpp/client/QXmppOutgoingClient.cpp \
        qxmpp/client/QXmppRemoteMethod.cpp \
        qxmpp/client/QXmppRosterManager.cpp \
        qxmpp/client/QXmppRpcManager.cpp \
        qxmpp/client/QXmppTransferManager.cpp \
        qxmpp/client/QXmppUploadRequestManager.cpp \
        qxmpp/client/QXmppVCardManager.cpp \
        qxmpp/client/QXmppVersionManager.cpp \
#        qxmpp/server/QXmppDialback.cpp \
#        qxmpp/server/QXmppIncomingClient.cpp \
#        qxmpp/server/QXmppIncomingServer.cpp \
#        qxmpp/server/QXmppOutgoingServer.cpp \
#        qxmpp/server/QXmppPasswordChecker.cpp \
#        qxmpp/server/QXmppServer.cpp \
#        qxmpp/server/QXmppServerExtension.cpp \
#        qxmpp/server/QXmppServerPlugin.cpp \


HEADERS += \
        qxmpp/base/QXmppArchiveIq.h \
        qxmpp/base/QXmppBindIq.h \
        qxmpp/base/QXmppBookmarkSet.h \
        qxmpp/base/QXmppByteStreamIq.h \
        qxmpp/base/QXmppCodec_p.h \
        qxmpp/base/QXmppConstants_p.h \
        qxmpp/base/QXmppDataForm.h \
        qxmpp/base/QXmppDiscoveryIq.h \
        qxmpp/base/QXmppElement.h \
        qxmpp/base/QXmppEntityTimeIq.h \
        qxmpp/base/QXmppGlobal.h \
        qxmpp/base/QXmppHttpUploadIq.h \
        qxmpp/base/QXmppIbbIq.h \
        qxmpp/base/QXmppIq.h \
        qxmpp/base/QXmppJingleIq.h \
        qxmpp/base/QXmppLogger.h \
        qxmpp/base/QXmppMamIq.h \
        qxmpp/base/QXmppMessage.h \
        qxmpp/base/QXmppMixIq.h \
        qxmpp/base/QXmppMixItem.h \
        qxmpp/base/QXmppMucIq.h \
        qxmpp/base/QXmppNonSASLAuth.h \
        qxmpp/base/QXmppPingIq.h \
        qxmpp/base/QXmppPresence.h \
        qxmpp/base/QXmppPubSubIq.h \
        qxmpp/base/QXmppRegisterIq.h \
        qxmpp/base/QXmppResultSet.h \
        qxmpp/base/QXmppRosterIq.h \
        qxmpp/base/QXmppRpcIq.h \
        qxmpp/base/QXmppRtcpPacket.h \
        qxmpp/base/QXmppRtpChannel.h \
        qxmpp/base/QXmppRtpPacket.h \
        qxmpp/base/QXmppSasl_p.h \
        qxmpp/base/QXmppSessionIq.h \
        qxmpp/base/QXmppSocks.h \
        qxmpp/base/QXmppStanza.h \
        qxmpp/base/QXmppStanza_p.h \
        qxmpp/base/QXmppStream.h \
        qxmpp/base/QXmppStreamFeatures.h \
        qxmpp/base/QXmppStreamInitiationIq_p.h \
        qxmpp/base/QXmppStreamManagement_p.h \
        qxmpp/base/QXmppStun.h \
        qxmpp/base/QXmppStun_p.h \
        qxmpp/base/QXmppUtils.h \
        qxmpp/base/QXmppVCardIq.h \
        qxmpp/base/QXmppVersionIq.h \
        qxmpp/client/QXmppArchiveManager.h \
        qxmpp/client/QXmppBookmarkManager.h \
        qxmpp/client/QXmppCallManager.h \
        qxmpp/client/QXmppCarbonManager.h \
        qxmpp/client/QXmppClient.h \
        qxmpp/client/QXmppClientExtension.h \
        qxmpp/client/QXmppConfiguration.h \
        qxmpp/client/QXmppDiscoveryManager.h \
        qxmpp/client/QXmppEntityTimeManager.h \
        qxmpp/client/QXmppInvokable.h \
        qxmpp/client/QXmppMamManager.h \
        qxmpp/client/QXmppMessageReceiptManager.h \
        qxmpp/client/QXmppMucManager.h \
        qxmpp/client/QXmppOutgoingClient.h \
        qxmpp/client/QXmppRemoteMethod.h \
        qxmpp/client/QXmppRosterManager.h \
        qxmpp/client/QXmppRpcManager.h \
        qxmpp/client/QXmppTransferManager.h \
        qxmpp/client/QXmppTransferManager_p.h \
        qxmpp/client/QXmppUploadRequestManager.h \
        qxmpp/client/QXmppVCardManager.h \
        qxmpp/client/QXmppVersionManager.h \
#----- end ---------


FORMS += \
        loginform.ui \
        mainwindow.ui \
        stackedwidget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    icons.qrc

DISTFILES += \
    andriod_sources/AndroidManifest.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/andriod_sources

