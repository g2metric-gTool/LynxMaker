import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import "../common"

FluScrollablePage {
    id: root

    Connections {
        target: ConfigureController
        function onErrorOccurred(message) {
            hideLoading()
            showError(message, 3000)
        }
        function onExecutionSuccessful() {
            hideLoading()
            showSuccess("Configuration done!", 3000)
        }
    }

    ColumnLayout {
        spacing: 25
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        // Device list
        Row {
            spacing: 25
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
        }

        Row {
            spacing: 25
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            FluTextBox {
                id: resetDuration
                cleanEnabled: true
                width: cameraList.width
                placeholderText: qsTr("Reset duration (s)")
            }

            FluAutoSuggestBox {
                id: serialNumber
                width: promotionList.width
                placeholderText: qsTr("Serial number")
                items: generateSerialNumbers(10, 30)
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
                    contentHeight: deviceDetails.height
                    ScrollBar.vertical: FluScrollBar {
                    }
                    FluText {
                        id: deviceDetails
                        width: scrollview.width
                        wrapMode: Text.WrapAnywhere
                        padding: 10
                    }
                }
            }
        }
        // Buttons
        Row {
            spacing: 25
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            X_Button {
                width: 125
                height: 30
                radius: 5
                text: qsTr("Configure")
                enabled: serialNumber.text.length > 0
                onClicked: {
                    showLoading()
                    let cameraIpAddress = cameraList.currentText;
                    let lynxSerialNumber = serialNumber.text;
                    let promotionIpAddress = promotionList.currentText;
                    let resetDurationValue = parseFloat(resetDuration.text);
                    ConfigureController.run(cameraIpAddress, lynxSerialNumber, promotionIpAddress, resetDurationValue);
                }
            }
            X_Button {
                width: 125
                height: 30
                radius: 5
                text: qsTr("Discover")
                onClicked: {
                    ConfigureController.discover();
                }
            }
            X_Button {
                width: 125
                height: 30
                radius: 5
                text: qsTr("Device details")
                enabled: ConfigureController.cameraList.length !== 0 && ConfigureController.promotionList.length !== 0
                onClicked: {
                    deviceDetails.text = ConfigureController.deviceDetails(cameraList.currentText);
                    expander.expand = true;
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
