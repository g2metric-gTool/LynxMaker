import QtQuick
import Qt5Compat.GraphicalEffects
import QtQuick.Layouts
import QtQuick.Controls

Item
{
    id: root
    property int radius: 10
    property var hoveredColor: Qt.rgba(6/255,158/255,39/255,1)
    property var baseColor: enabled ? FluTheme.dark ? Qt.rgba(38/255,44/255,54/255,1) : Qt.rgba(251/255,251/255,253/255,1) : disabledColor
    property var disabledColor: FluTheme.dark ? Qt.rgba(39/255,39/255,39/255,1) : "#F3F3F3"

    property string text: ""
    property var font: FluTextStyle.BodyStrong

    signal clicked(var mouse)
    signal doubleClicked(var mouse)

    FluRectangle
    {
        enabled: root.enabled
        anchors.fill: parent
        radius: [root.radius,root.radius,root.radius,root.radius]
        color: _color

        property color _color: ma.containsMouse ? ma.containsPress ? hoveredColor.lighter(1.3) : hoveredColor : baseColor
        Behavior on _color {
            ColorAnimation {
                duration: 100
            }
        }
        FluText
        {
            id: textBtn
            anchors.centerIn: parent
            font: root.font
            text: root.text
        }

    }

    MouseArea
    {
        id: ma
        anchors.fill: parent
        hoverEnabled: true
        enabled: root.enabled

        onClicked: function(mouse) {root.clicked(mouse)}
        onDoubleClicked: function(mouse) {root.doubleClicked(mouse)}
    }
}
