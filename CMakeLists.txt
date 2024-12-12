cmake_minimum_required(VERSION 3.21.1)

project(QmlTestRecorder VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6
    COMPONENTS
    Core
    Gui
    Quick
    Test
    REQUIRED)


set(CMAKE_AUTOMOC ON)

# set_target_properties(QmlTestRecorder PROPERTIES
#     AUTOMOC ON
#     AUTORCC ON
#     AUTOUIC ON
# )

# Generate the export header
# generate_export_header(QmlTestRecorder
#     BASE_NAME QML_TEST_RECORDER_LIB
#     EXPORT_MACRO_NAME QML_TEST_RECORDER_LIB_EXPORT
#     EXPORT_FILE_NAME QmlTestRecorder-export.h
# )

qt_add_library(QmlTestRecorder SHARED)

# Collect source files
file(GLOB qml_test_recorder_CPP_FILES "src/*.cpp")
file(GLOB qml_test_recorder_H_FILES "src/*.h")

message(STATUS "CPP Files: ${qml_test_recorder_CPP_FILES}")

# Make the file paths relative to the project directory
set(qml_test_recorder_RELATIVE_CPP_FILES "")
foreach(file ${qml_test_recorder_CPP_FILES})
    file(RELATIVE_PATH relFile "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
    list(APPEND qml_test_recorder_RELATIVE_CPP_FILES "${relFile}")
endforeach()

set(qml_test_recorder_RELATIVE_H_FILES "")
foreach(file ${qml_test_recorder_H_FILES})
    file(RELATIVE_PATH relFile "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
    list(APPEND qml_test_recorder_RELATIVE_H_FILES "${relFile}")
endforeach()

# Get all QML and JS files
file(GLOB qml_test_recorder_QML_FILES "*.qml" "/*.js")

# Make the remaining QML files relative to the project directory
set(qml_test_recorder_RELATIVE_QML_FILES "")
foreach(file ${qml_test_recorder_QML_FILES})
    file(RELATIVE_PATH relFile "${CMAKE_CURRENT_SOURCE_DIR}" "${file}")
    message(STATUS "Adding QML file: ${relFile}")
    list(APPEND qml_test_recorder_RELATIVE_QML_FILES "${relFile}")
endforeach()

qt_add_qml_module(QmlTestRecorder
    URI QmlTestRecorder
    VERSION 1.0
    RESOURCE_PREFIX /qml
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/QmlTestRecorder
    DEPENDENCIES
        QtQuick
        QtQuick.Controls
    SOURCES
        ${qml_test_recorder_RELATIVE_CPP_FILES}
        ${qml_test_recorder_RELATIVE_H_FILES}
    QML_FILES
        ${qml_test_recorder_RELATIVE_QML_FILES}
)

# Link the library with required Qt modules
target_include_directories(QmlTestRecorder PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(QmlTestRecorder
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Quick
        Qt::Test
)

target_sources(QmlTestRecorder PRIVATE ${qml_test_recorder_CPP_FILES} ${qml_test_recorder_H_FILES})

add_subdirectory(example/simple)
