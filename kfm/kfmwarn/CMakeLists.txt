include_directories(
    ${QT_INCLUDE_DIR}
    ${KDE1_INCLUDE_DIR}
    )

set(KFMWARN_SRCS main.cpp)

add_executable(kfmwarn ${KFMWARN_SRCS})
target_link_libraries(kfmwarn
    ${QT_LIBRARIES}
    ${X11_LIBRARIES}
    ${KDE1_KDECORE}
    )

install(TARGETS kfmwarn RUNTIME DESTINATION ${KDE1_BINDIR})

