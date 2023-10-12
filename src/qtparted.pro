QT+=core gui widgets

#LIBS
unix:LIBS += -ldl -lparted

# Executable name
TARGET    = qtparted


# Header files
HEADERS   =  qtparted.h          \
             qp_common.h         \
             qp_settings.h       \
             qp_exttools.h       \
             qp_libparted.h      \
             qp_filesystem.h     \
             qp_fswrap.h         \
             qp_window.h         \
             qp_dlgcreate.h      \
             qp_dlgresize.h      \
             qp_dlgprogress.h    \
             qp_dlgformat.h      \
             qp_dlgconfig.h      \
             qp_partlist.h       \
             qp_listview.h       \
             qp_listchart.h      \
             qp_partition.h      \
             qp_partwidget.h     \
             qp_extended.h       \
             qp_drivelist.h      \
             qp_navview.h        \
             qp_diskview.h       \
             qp_sizepart.h       \
             qp_actlist.h        \
             qp_combospin.h      \
             qp_devlist.h        \
             qp_spinbox.h        \
             qp_dlgdevprop.h     \
             qp_debug.h          \
             statistics.h




# Source files
SOURCES   =  main.cpp            \
             qp_common.cpp       \
             qp_settings.cpp     \
             qp_exttools.cpp     \
             qp_libparted.cpp    \
             qp_filesystem.cpp   \
             qp_fswrap.cpp       \
             qp_window.cpp       \
             qp_dlgcreate.cpp    \
             qp_dlgresize.cpp    \
             qp_dlgprogress.cpp  \
             qp_dlgformat.cpp    \
             qp_dlgconfig.cpp    \
             qp_partlist.cpp     \
             qp_listview.cpp     \
             qp_listchart.cpp    \
             qp_partition.cpp    \
             qp_partwidget.cpp   \
             qp_extended.cpp     \
             qp_drivelist.cpp    \
             qp_navview.cpp      \
             qp_diskview.cpp     \
             qp_sizepart.cpp     \
             qp_actlist.cpp      \
             qp_combospin.cpp    \
             qp_spinbox.cpp      \
             qp_devlist.cpp      \
             qp_dlgdevprop.cpp   \
             qp_debug.cpp        \
             statistics.cpp


# Qt Designer interfaces
INTERFACES = qp_ui_create.ui     \
             qp_ui_format.ui     \
             qp_ui_resize.ui     \
             qp_ui_progress.ui   \
             qp_ui_devprop.ui

# Translations
TRANSLATIONS = ../ts/qtparted_ca.ts \
               ../ts/qtparted_cs.ts \
               ../ts/qtparted_de.ts \
               ../ts/qtparted_es.ts \
               ../ts/qtparted_fi.ts \
               ../ts/qtparted_fr.ts \
               ../ts/qtparted_it.ts \
               ../ts/qtparted_pl.ts \
               ../ts/qtparted_ru.ts \
               ../ts/qtparted_sv.ts \
               ../ts/qtparted_ua.ts

