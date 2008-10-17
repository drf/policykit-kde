project (PolicyKit-kde)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include (FindPkgConfig)

pkg_check_modules (POLKIT polkit-dbus>=0.7)

if (POLKIT_FOUND)
  add_subdirectory(src)
  install( FILES policykit-kde.desktop DESTINATION ${AUTOSTART_INSTALL_DIR} )
endif (POLKIT_FOUND)
