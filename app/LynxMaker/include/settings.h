#pragma once

#include <QSettings>

class ApplicationSettings : public QSettings
{
	Q_OBJECT
public:
	explicit ApplicationSettings(const QString& fileName, const QSettings::Format& format = QSettings::Format::IniFormat, QObject* parent = nullptr);

	Q_INVOKABLE void setValue(QString key, const QVariant& value);
	Q_INVOKABLE QVariant value(QString key, const QVariant& defaultValue) const;
	Q_INVOKABLE QVariant value(QString key) const;

	Q_INVOKABLE QString language() const;
	Q_INVOKABLE QStringList availableLanguages() const;
	Q_INVOKABLE void setLanguage(const QString& language);

	Q_INVOKABLE bool animationEnabled() const;
	Q_INVOKABLE void setAnimationEnabled(bool animationEnabled);

	Q_INVOKABLE QString accentColor() const;
	Q_INVOKABLE void setAccentColor(QString accentColor);

	Q_INVOKABLE bool useSystemAppBar() const;
	Q_INVOKABLE void setUseSystemAppBar(bool useSystemAppBar);

	Q_INVOKABLE bool fitsAppBarWindows() const;
	Q_INVOKABLE void setFitsAppBarWindows(bool fitsAppBarWindows);

	Q_INVOKABLE int displayMode() const;
	Q_INVOKABLE void setDisplayMode(int displayMode);

	Q_INVOKABLE int darkMode() const;
	Q_INVOKABLE void setDarkMode(int darkModel);
};