import QtQuick
import QmlTestRecorder.Simple

Window {
    visible: true

    width: simple.width
    height: simple.height

    Simple {
        id: simple
    }
}
