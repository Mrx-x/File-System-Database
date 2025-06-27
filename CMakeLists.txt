cmake_minimum_required(VERSION 3.16)

project(File-System-Database VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
        File-System-Database/main.cpp
        File-System-Database/mainwindow.cpp
        File-System-Database/mainwindow.h
        File-System-Database/mainwindow.ui
        File-System-Database/src/Scanner/DirectoryScanner.cpp
        File-System-Database/src/Model/ScannerItem.cpp
        File-System-Database/src/Controller/ScanController.cpp
        File-System-Database/src/Database/SQLiteDatabaseService.cpp
)

SET(HEADERS
    File-System-Database/include/Scanner/IScanner.h
    File-System-Database/include/Scanner/DirectoryScanner.h
    File-System-Database/include/Model/ScanItem.h
    File-System-Database/include/Controller/ScanController.h
    File-System-Database/include/Database/IDatabaseService.h
    File-System-Database/include/Database/SQLiteDatabaseService.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(File-System-Database
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${HEADERS}
    )
else()
    add_executable(File-System-Database
        ${PROJECT_SOURCES}
        ${HEADERS}
    )
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/File-System-Database/include)

target_link_libraries(File-System-Database PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

include(GNUInstallDirs)
install(TARGETS File-System-Database
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(File-System-Database)
endif()
