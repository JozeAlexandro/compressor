QT += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
        algorithm/cAlgorithmHaffman/src/cAlgorithmHaffman.cpp \
        algorithm/cAlgorithmRLE/src/cAlgorithmRLE.cpp \
        cFileWorker/src/cFileWorker.cpp \
        main.cpp \
        windowGUI/src/windowGUI.cpp

HEADERS += \
    algorithm/cAbstractAlgorithm/h/cAbstractAlgorithm.h \
    algorithm/cAlgorithmHaffman/h/cAlgorithmHaffman.h \
    algorithm/cAlgorithmRLE/h/cAlgorithmRLE.h \
    cFileWorker/h/cFileWorker.h \
    common.h \
    windowGUI/h/windowGUI.h

FORMS += \
    windowGUI/src/windowGUI.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/lib/libJournalView/h

LIBS += -L$$PWD"/lib/libJournalView/bin"
LIBS += -llibJournalView

RESOURCES += \
    resource.qrc
