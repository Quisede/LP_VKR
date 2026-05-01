cmake_minimum_required(VERSION 3.19)
project(LMSClient LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network)

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
    pages/coursespage.h
    pages/coursespage.cpp
    pages/teachercreatecoursepage.h
    pages/teachercreatecoursepage.cpp
    pages/teachercoursebuilderpage.h
    pages/teachercoursebuilderpage.cpp
    pages/teacherstudentspage.h
    pages/teacherstudentspage.cpp
    pages/coursedetailspage.h
    pages/coursedetailspage.cpp
    pages/testrunnerpage.h
    pages/testrunnerpage.cpp
    pages/attemptspage.h
    pages/attemptspage.cpp
    pages/profilepage.h
    pages/profilepage.cpp
    models/sessiondata.h
    models/coursemodel.h
    models/coursestudentmodel.h
    models/lessonmodel.h
    models/materialmodel.h
    models/testmodel.h
    models/questionmodel.h
    models/attemptmodel.h
)

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
)

install(TARGETS LMSClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_generate_deploy_app_script(
        TARGET LMSClient
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
endif()
