cmake_minimum_required(VERSION 3.16)

# Project name and version
project(simple VERSION 1.0 LANGUAGES CXX)

# Find the Qt6 package
find_package(Qt6
    COMPONENTS Quick QuickTest REQUIRED)

set(AUTOMOC ON)
set(AUTORCC ON)

# Add the executable
qt_add_executable(qml-test-recorder-simple
    main.cpp
)

qt_add_qml_module(simplemodule
    URI QmlTestRecorder.Simple
    VERSION 1.0
    RESOURCE_PREFIX /qml
    OUTPUT_DIRECTORY QmlTestRecorder/Simple
    DEPENDENCIES
        QtQuick
    QML_FILES
        Simple.qml
        SimpleWindow.qml
)

# Link Qt libraries
target_link_libraries(qml-test-recorder-simple PRIVATE Qt6::Quick)

# Specify the C++ standard
set_target_properties(qml-test-recorder-simple PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

### QML testing ###
qt_add_executable(qml-test-recorder-test_simple
    test/main.cpp
)

target_compile_definitions(qml-test-recorder-test_simple PRIVATE QUICK_TEST_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test")

qt_add_resources(qml-test-recorder-test_simple "qmls"
    PREFIX "/qmls"
    FILES
    test/tst_Simple.qml)

target_link_libraries(qml-test-recorder-test_simple
    PRIVATE
    Qt6::Test
    Qt6::Quick
    Qt6::Qml
    Qt6::QuickTest
)
