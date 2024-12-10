import QtQuick

Item {
    id: clickId
    z: 999

    property Item target
    property point clickPos;
    property bool animationEnabled: false

    function click(target, x, y) {
        clickId.target = target;
        clickId.clickPos = Qt.point(x, y);
        if(animationEnabled) {
            animationId.restart()
        }
    }

    Rectangle {
        id: placeholder

        x: target ? target.mapToItem(clickId, 0, 0).x : 0
        y: target ? target.mapToItem(clickId, 0, 0).y : 0

        width: target ? target.width : 0
        height: target ? target.height: 0

        color: "green"
        opacity: 0.5
    }

    Rectangle {
        id: clickLocation

        x: target ? target.mapToItem(clickId, clickPos).x : 0
        y: target ? target.mapToItem(clickId, clickPos).y : 0

        width: 10
        height: 10

        radius: 5
        color: "red"
        opacity: 0.0
    }

    ParallelAnimation {
        id: animationId

        NumberAnimation {
            target: clickLocation
            property: "opacity"
            from: 1.0
            to: 0.0
            duration: 1000
        }

        NumberAnimation {
            target: placeholder
            property: "opacity"
            from: 0.5
            to: 0.0
            duration: 2000
        }
    }
}
