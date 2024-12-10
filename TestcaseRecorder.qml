import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import MapWhere.Test
import QtTest

Window {
    id: testcaseWindowId

    property Item rootItem;
    property alias rootItemId: filterId.rootObjectId

    width: 1000
    height: 800
    x: 500
    y: 100
    visible: true

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
        parent: rootItem
        animationEnabled: true
    }

//    TestCase {
//        name: "FieldEditTest"
//        when: windowShown

//        function test_record() {
//            console.log("Recording, this will wait for a long time");
//            wait(10000000);
//        }
//    }

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
                        filterId.rootObject = rootItem
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
//                anchors.fill: parent
                selectByMouse: true
                selectByKeyboard: true
                readOnly: true
                text: filterId.output
            }
        }
    }
}
