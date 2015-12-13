set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

include_directories(
    BEFORE
        ${CMAKE_SOURCE_DIR}/knode
)


set( knodetest_SRCS
  knodetest.cpp
)
add_executable( knodetest
  ${knodetest_SRCS}
)
ecm_mark_as_test(knode-knodetest)
target_link_libraries( knodetest
    knodecommon
    KF5::IdentityManagement
    KF5::KCMUtils
    KF5::KDELibs4Support
    KF5::Libkdepim
    KF5::Mime
    Qt5::Test
    Qt5::Core
)
