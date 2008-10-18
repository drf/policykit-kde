FIND_PACKAGE(Qt3 REQUIRED)

ADD_SUBDIRECTORY(doc)

ADD_DEFINITIONS(${QT_DEFINITIONS})
LINK_DIRECTORIES(${QT_QT_LIBRARY})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    /usr/include/dbus-1.0/qt3 /usr/include/dbus-1.0 /usr/lib/dbus-1.0/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus)

SET (
    dbus_sources
    qdbusconnection.cpp
    qdbuserror.cpp
    qdbusintegrator.cpp
    qdbusmarshall.cpp
    qdbusmessage.cpp
    qdbusproxy.cpp
    qdbusserver.cpp
    qdbusdataconverter.cpp
    qdbusdatalist.cpp
    qdbusdata.cpp
    qdbusobjectpath.cpp
    )

ADD_CUSTOM_TARGET(moc_target ALL ${QT_MOC_EXECUTABLE} -o qdbusserver.moc ${CMAKE_CURRENT_SOURCE_DIR}/dbus/qdbusserver.h
    COMMAND ${QT_MOC_EXECUTABLE} -o qdbusconnection.moc ${CMAKE_CURRENT_SOURCE_DIR}/qdbusconnection_p.h
    COMMAND ${QT_MOC_EXECUTABLE} -o qdbusproxy.moc ${CMAKE_CURRENT_SOURCE_DIR}/dbus/qdbusproxy.h)

ADD_LIBRARY(dbus-1-qt3 SHARED ${dbus_sources})

ADD_DEPENDENCIES(dbus-1-qt3 moc_target)

TARGET_LINK_LIBRARIES(dbus-1-qt3 ${QT_LIBRARIES} dbus-1)
SET_TARGET_PROPERTIES(dbus-1-qt3 PROPERTIES
    VERSION 0.2)

INSTALL(TARGETS dbus-1-qt3 LIBRARY DESTINATION lib)
