QT += xml
TARGET = TaigaWizard
TEMPLATE = app
DESTDIR = bin
HEADERS += configxmlfile.h \
    configinifile.h \
    confighelper.h \
    confighandler.h \
    configfile.h \
    wizard.h \
    databasedialog.h \
    dialog.h \
    regionwizard.h \
    connectionstringhelper.h \
    WizardInterface.h
SOURCES += wizard.cpp \
    main.cpp \
    configxmlfile.cpp \
    configinifile.cpp \
    confighelper.cpp \
    confighandler.cpp \
    configfile.cpp \
    databasedialog.cpp \
    dialog.cpp \
    regionwizard.cpp \
    connectionstringhelper.cpp
FORMS += wizard.ui \
    databasedialog.ui \
    dialog.ui \
    regionwizard.ui
