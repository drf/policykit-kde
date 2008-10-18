PROJECT (PolicyKit-kde)

# use FindKDE3 module of cmake for KDE specific operations
FIND_PACKAGE(KDE3 REQUIRED)

# add definitions, library directories and header directories for KDE and QT
ADD_DEFINITIONS(${QT_DEFINITIONS} ${KDE3_DEFINITIONS})

LINK_DIRECTORIES(${KDE3_LIB_DIR} dbus-qt3-backport)

INCLUDE_DIRECTORIES(${KDE3_INCLUDE_DIR}
    ${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    /usr/include/dbus-1.0/qt3
    /usr/include/dbus-1.0
    /usr/lib/dbus-1.0/include
    ${CMAKE_SOURCE_DIR}/dbus-qt3-backport
    ${CMAKE_SOURCE_DIR}/dbus-qt3-backport/dbus
    /usr/include/PolicyKit)

# specify sources
SET (
    sources
    authdialog.cpp
    policykitkde.cpp
    service.cpp
    main.cpp
    debug.cpp
    )

# moc files will be created automatically
KDE3_AUTOMOC(${sources})

# generate code from ui file
KDE3_ADD_UI_FILES(sources authdialogui.ui)

# build executable named policykit-kde
KDE3_ADD_EXECUTABLE(policykit-kde ${sources})

#set exception flag
SET_TARGET_PROPERTIES(policykit-kde
                    PROPERTIES COMPILE_FLAGS -fexceptions)
SET(
  PO_FILES
  ca.po
  de.po
  es.po
  fr.po
  it.po
  nl.po
  pt_BR.po
  pl.po
  sv.po
  tr.po
  )

#generate pot file
ADD_CUSTOM_COMMAND(TARGET policykit-kde
                    POST_BUILD
                    COMMAND xgettext ARGS --no-location --qt -o
                    ${CMAKE_SOURCE_DIR}/po/policykit-kde.pot -ki18n -ktr2i18n -kI18N_NOOP -ktranslate -kaliasLocale
                    -C -x /usr/kde/3.5/include/kde.pot
                    ${CMAKE_CURRENT_SOURCE_DIR}/authdialog.cpp
                    ${CMAKE_BINARY_DIR}/src/authdialogui.cpp)

#merge po files
FOREACH(_po ${PO_FILES})
    GET_FILENAME_COMPONENT(_lang ${_po} NAME_WE)
    SET(_gmofile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
    ADD_CUSTOM_COMMAND(
        TARGET policykit-kde
        POST_BUILD
        COMMAND msgmerge ARGS --quiet --update --no-location --backup=none -s ${CMAKE_SOURCE_DIR}/po/${_po} ${CMAKE_SOURCE_DIR}/po/policykit-kde.pot
        COMMAND msgfmt ARGS -o ${_gmofile} ${CMAKE_SOURCE_DIR}/po/${_po})
    INSTALL(FILES ${_gmofile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME policykit-kde.mo)
    SET(_gmofiles ${_gmofiles} ${_gmofile})
ENDFOREACH(_po ${PO_FILES})

# link executabel with kde libraries
TARGET_LINK_LIBRARIES(policykit-kde
                      ${QT_AND_KDECORE_LIBS}
                      kdeui
                      dbus-1
                      dbus-1-qt3
                      polkit
                      polkit-dbus
                      polkit-grant)

#install policykit-kde executable and service file
INSTALL(TARGETS policykit-kde DESTINATION libexec)
INSTALL(FILES
    ${CMAKE_SOURCE_DIR}/data/kde-org.freedesktop.PolicyKit.AuthenticationAgent.service
    DESTINATION share/dbus-1/services)
