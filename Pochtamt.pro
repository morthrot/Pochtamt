greaterThan(QT_MAJOR_VERSION, 4) {
    DEFINES += AM_USING_QT5
    }
else {
    DEFINES += AM_USING_QT4
    }

TEMPLATE		= app
CONFIG			+= qt warn_off release console
CONFIG			-= app_bundle
QT			-= gui
QT			+= network

FORMS			=

HEADERS			= Pochtamt.h

SOURCES			= Main.cpp \
			  Pochtamt.cpp

RESOURCES		=
TARGET			= Pochtamt
OBJECTS_DIR		= .temp
MOC_DIR			= .temp
UI_DIR			= .temp
UI_HEADERS_DIR		= .temp
UI_SOURCES_DIR		= .temp

