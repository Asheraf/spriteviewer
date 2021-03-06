TEMPLATE = app
QT = gui core
CONFIG += qt release warn_on console
DESTDIR = bin
RCC_DIR = o
OBJECTS_DIR = o
MOC_DIR = o
UI_DIR = o
HEADERS = src/sprite.h src/spr_viewer.h src/pal_edit.h
SOURCES = src/main.cpp src/sprite.cpp src/spr_viewer.cpp src/pal_edit.cpp
RESOURCES += ui/resources.qrc
FORMS += ui/spr_viewer.ui ui/pal_edit.ui
RC_FILE = ui/icono.rc
TRANSLATIONS = ui/i18n/spritev_en.ts \
               ui/i18n/spritev_es.ts
