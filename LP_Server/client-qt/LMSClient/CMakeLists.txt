cmake_minimum_required(VERSION 3.19)
project(LMSClient LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network PrintSupport)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_standard_project_setup()
endif()

set(LMSCLIENT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    windows/loginwindow.h
    windows/loginwindow.cpp
    windows/loginwindow.ui
    api/apiclient.h
    api/apiclient.cpp
    widgets/coursecard.h
    widgets/coursecard.cpp
    pages/dashboardpage.h
    pages/dashboardpage.cpp
    pages/deadlinespage.h
    pages/deadlinespage.cpp
    pages/adminauditpage.h
    pages/adminauditpage.cpp
    pages/admingroupspage.h
    pages/admingroupspage.cpp
    pages/adminuserspage.h
    pages/adminuserspage.cpp
    pages/coursespage.h
    pages/coursespage.cpp
    pages/teachercreatecoursepage.h
    pages/teachercreatecoursepage.cpp
    pages/teachercoursebuilderpage.h
    pages/teachercoursebuilderpage.cpp
    pages/teachertesteditorpage.h
    pages/teachertesteditorpage.cpp
    pages/teacherstudentspage.h
    pages/teacherstudentspage.cpp
    pages/teacheranalyticspage.h
    pages/teacheranalyticspage.cpp
    pages/coursedetailspage.h
    pages/coursedetailspage.cpp
    pages/testrunnerpage.h
    pages/testrunnerpage.cpp
    pages/attemptspage.h
    pages/attemptspage.cpp
    pages/profilepage.h
    pages/profilepage.cpp
    resources/resources.qrc
    models/sessiondata.h
    models/coursemodel.h
    models/coursestudentmodel.h
    models/lessonmodel.h
    models/materialmodel.h
    models/testmodel.h
    models/questionmodel.h
    models/attemptmodel.h
    models/adminauditmodel.h
    models/adminusermodel.h
    models/adminoverviewmodel.h
    models/teacheranalyticsmodel.h
    models/teacherstudentattemptmodel.h
)

set(APP_ICON_MACOS "${CMAKE_CURRENT_SOURCE_DIR}/resources/icons/lms-app.icns")
if (EXISTS "${APP_ICON_MACOS}")
    set_source_files_properties("${APP_ICON_MACOS}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    list(APPEND LMSCLIENT_SOURCES "${APP_ICON_MACOS}")
endif()

if (QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(LMSClient
        WIN32 MACOSX_BUNDLE
        ${LMSCLIENT_SOURCES}
    )
else()
    add_executable(LMSClient ${LMSCLIENT_SOURCES})
endif()

target_link_libraries(LMSClient
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::PrintSupport
)

set_target_properties(LMSClient PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "local.lms.client"
    MACOSX_BUNDLE_BUNDLE_NAME "LMS Client"
    MACOSX_BUNDLE_DISPLAY_NAME "LMS Client"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
    MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
    MACOSX_BUNDLE_ICON_FILE "lms-app.icns"
)

install(TARGETS LMSClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

set(CPACK_PACKAGE_NAME "lms-client")
set(CPACK_PACKAGE_VENDOR "LMS Platform")
set(CPACK_PACKAGE_CONTACT "admin@lms.local")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Qt client for LMS Platform")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_DEBIAN_PACKAGE_SECTION "education")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "LMS Platform")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6")
set(CPACK_GENERATOR "DEB")
include(CPack)
