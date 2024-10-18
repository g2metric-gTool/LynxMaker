pragma Singleton

import QtQuick
import QtQuick.Controls

QtObject {
    property int displayMode: SettingsHelper.displayMode()

    onDisplayModeChanged: {
        if(displayMode !== SettingsHelper.displayMode())
            SettingsHelper.setDisplayMode(displayMode)
    }
}
