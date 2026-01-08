import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtCore
import QmlTestRecorder

Window {
    id: testcaseWindowId

    property Item rootItem;
    property alias rootItemId: filterId.rootObjectId

    width: 200
    height: 200
    x: 1000
    y: 1000
    visible: true

    onXChanged: windowSettings.setValue("x", x);
    onYChanged: windowSettings.setValue("y", y);
    onWidthChanged: windowSettings.setValue("width", width)
    onHeightChanged: windowSettings.setValue("height", height)

    function tryFindObject(testcase, chain) {
        testcase.tryVerify(function() { return ObjectFinder.findObjectByChain(rootItem.parent, chain) !== null }, 1000, `Can't find ${chain}`)
        return ObjectFinder.findObjectByChain(rootItem.parent, chain);
    }

    Component.onCompleted: {
        // Restore the window position and size
        x = windowSettings.value("x", 1000);
        y = windowSettings.value("y", 1000);
        width = windowSettings.value("width", 400);
        height = windowSettings.value("height", 200);
    }

    // Settings object to store window properties
    Settings {
        id: windowSettings
        category: "TestcaseRecorder"
    }

    EventFilter {
        id: filterId

        onClicked: function(item, point) {
            clickDebuggerId.click(item, point.x, point.y)
        }

        Component.onCompleted:  {
            filterId.addIgnore(testcaseWindowId.contentItem)
        }
    }

    ClickDebugger {
        id: clickDebuggerId
        parent: testcaseWindowId.rootItem
        animationEnabled: true
    }


    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        RowLayout {
            Label {
                text: "RootId:"
            }

            TextField {
                Layout.fillWidth: true
                text: filterId.rootObjectId
                onTextChanged: {
                    filterId.rootObjectId = text
                }
            }
        }

        RowLayout {
            Button {
                text: filterId.rootObject == null ? "Start" : "Stop"
                onClicked: {
                    if(filterId.rootObject != null) {
                        filterId.rootObject = null
                    } else {
                        filterId.rootObject = testcaseWindowId.rootItem
                    }
                }
            }

            Button {
                text: "Clear"
                onClicked: {
                    filterId.clear();
                }
            }

            Button {
                text: "Copy"
                onClicked: {
                    textAreaId.selectAll();
                    textAreaId.copy();
                }
            }
        }

        ScrollView {
            id: scrollViewId
            Layout.fillHeight: true
            Layout.fillWidth: true
            TextArea {
                id: textAreaId
                selectByMouse: true
                selectByKeyboard: true
                readOnly: true
                text: filterId.output
            }
        }
    }
}
