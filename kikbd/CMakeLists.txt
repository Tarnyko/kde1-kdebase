add_subdirectory(doc)
add_subdirectory(maps)

include_directories(
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
    )

set(KIKBD_SRCS kikbd.cpp keytrans.cpp keymap.cpp kikbdconf.cpp
    kobjconf.cpp kconfobjs.cpp)

set(KIKBD_MOC kconfobjs.moc kobjconf.moc keymap.moc kikbd.moc kikbdconf.moc widgets.moc)

QT1_WRAP_MOC(MOC_FILES ${KIKBD_MOC})

add_executable(kikbd ${KIKBD_SRCS} ${MOC_FILES})
target_link_libraries(kikbd
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kikbd RUNTIME DESTINATION ${KDE1_BINDIR})

set(KCMIKBD_SRCS widgets.cpp kcmikbd.cpp kikbdconf.cpp kobjconf.cpp kconfobjs.cpp)

add_executable(kcmikbd ${KCMIKBD_SRCS} ${MOC_FILES})
target_link_libraries(kcmikbd
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    ${KDE1_KDEUI}
    )

install(TARGETS kcmikbd RUNTIME DESTINATION ${KDE1_BINDIR})

install_icon(kikbd.xpm.xpm "")
install(FILES kikbd.kdelnk DESTINATION ${KDE1_APPSDIR}/System)
install(FILES kcmikbd.kdelnk DESTINATION ${KDE1_APPSDIR}/Settings/Input_Devices)
install(FILES kikbdrc DESTINATION ${KDE1_CONFDIR})
