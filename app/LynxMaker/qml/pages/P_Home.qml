import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "../common"

FluScrollablePage {
    id: root

    ColumnLayout {
        spacing: 25
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        // Device list
        Row {
            spacing: 25
            ColumnLayout {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                FluText {
                    text: qsTr("Camera list")
                }
                FluComboBox {
                    id: cameraList
                    model: ConfigureController.cameraList
                    onCurrentTextChanged: {
                    }
                }
            }

            ColumnLayout {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                FluText {
                    text: qsTr("Promotion list")
                }
                FluComboBox {
                    id: promotionList
                    model: ConfigureController.promotionList
                    onCurrentTextChanged: {
                    }
                }
            }

            ColumnLayout {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                FluText {
                    text: qsTr("Reset duration (s)")
                }
                FluSpinBox{
                    id: resetDuration
                    value: 8
                    from: 0
                    to: 60
                    editable: true
                }
            }
        }
        // Serial number
        ColumnLayout {
            spacing: 10
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            FluAutoSuggestBox {
                id: serialNumber
                placeholderText: qsTr("Serial number")
                items: generateSerialNumbers(10, 50)
            }
        }
        // Device details
        FluExpander {
            id: expander
            headerText: qsTr("Open for device details")
            implicitWidth: 300
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Item {
                anchors.fill: parent
                Flickable {
                    id: scrollview
                    width: parent.width
                    height: parent.height
                    contentWidth: width
                    boundsBehavior: Flickable.StopAtBounds
                    contentHeight: text_info.height
                    ScrollBar.vertical: FluScrollBar {
                    }
                    FluText {
                        id: deviceDetails
                        width: scrollview.width
                        wrapMode: Text.WrapAnywhere
                        padding: 14
                    }
                }
            }
        }
        // Buttons
        Row {
            spacing: 25
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            X_Button {
                width: 75
                height: 30
                radius: 5
                text: qsTr("Configure")
                enabled: serialNumber.text.length > 0
                onClicked: {
                    let cameraIpAddress     = cameraList.currentText;
                    let lynxSerialNumber    = serialNumber.text;
                    let promotionIpAddress  = promotionList.currentText;
                    let resetDurationValue  = resetDuration.value;
                    ConfigureController.run(cameraIpAddress, lynxSerialNumber, promotionIpAddress, resetDurationValue)
                }
            }
            X_Button {
                width: 75
                height: 30
                radius: 5
                text: qsTr("Discover")
                onClicked: {
                    ConfigureController.discover();
                }
            }
            X_Button {
                width: 75
                height: 30
                radius: 5
                text: qsTr("Refresh")
                onClicked: {
                    deviceDetails.text = ConfigureController.deviceDetails(cameraList.currentText)
                    expander.expand = true
                }
            }
        }
    }

    function generateSerialNumbers(start, end) {
        const serialNumbers = [];
        for (let i = start; i <= end; i++) {
            const serialNumber = `SN_${i}`;
            serialNumbers.push({
                title: serialNumber
            });
        }
        return serialNumbers;
    }
}
