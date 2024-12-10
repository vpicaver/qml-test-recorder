import QtQuick
import MapWhere
import QtQuick.Controls
import QtQuick.Window
import QtTest
import "qrc:/qml"

Item {
    id: rootId
    objectName: "rootId"

    property alias recorder: recorderId
    property alias main: mainId

    width: 400
    height: 800

    function findObject(testcase, chain) {
        testcase.tryVerify(function() { return finder.findObjectByChain(rootId.parent, chain) !== null }, 1000, `Can't find ${chain}`)
        return finder.findObjectByChain(rootId.parent, chain);
    }

    TestcaseRecorder {
        id: recorderId
        rootItem: mainId
    }

    Main {
        id: mainId
    }

    onParentChanged: {
        rootId.parent.objectName = "rootParent"
    }

}

