#include <QFileInfo>
#include <QStandardPaths>

#include "settings.h"


ApplicationSettings::ApplicationSettings(const QString& fileName, const QSettings::Format& format, QObject* parent)
	: QSettings(fileName, format, parent)
{
}

Q_INVOKABLE void ApplicationSettings::setValue(QString key, const QVariant& value)
{
	return QSettings::setValue(key, value);
}

Q_INVOKABLE QVariant ApplicationSettings::value(QString key, const QVariant& defaultValue) const
{
	return QSettings::value(key, defaultValue);
}

Q_INVOKABLE QVariant ApplicationSettings::value(QString key) const
{
	return QSettings::value(key);
}

QString ApplicationSettings::language() const
{
	return value("general/system/language", QVariant("en_US")).toString();
}
QStringList ApplicationSettings::availableLanguages() const
{
	QStringList languages;
	languages << "en_US";
	languages << "fr_FR";
	languages << "zh_CN";
	return languages;
}
void ApplicationSettings::setLanguage(const QString& language)
{
	setValue("general/system/language", language);
}

bool ApplicationSettings::animationEnabled() const
{
	return value("general/ui/animationEnabled", QVariant(true)).toBool();
}
void ApplicationSettings::setAnimationEnabled(bool animationEnabled)
{
	setValue("general/ui/animationEnabled", animationEnabled);
}

QString ApplicationSettings::accentColor() const
{
	return value("general/system/accentColor", QString("#71D776")).toString();
}
void ApplicationSettings::setAccentColor(QString accentColor)
{
	setValue("general/system/accentColor", accentColor);
}

bool ApplicationSettings::useSystemAppBar() const
{
	return value("general/ui/useSystemAppBar", QVariant(false)).toBool();
}
void ApplicationSettings::setUseSystemAppBar(bool useSystemAppBar)
{
	setValue("general/ui/useSystemAppBar", useSystemAppBar);
}

bool ApplicationSettings::fitsAppBarWindows() const
{
	return value("general/ui/fitsAppBarWindows", QVariant(true)).toBool();
}
void ApplicationSettings::setFitsAppBarWindows(bool fitsAppBarWindows)
{
	setValue("general/ui/fitsAppBarWindows", fitsAppBarWindows);
}

int ApplicationSettings::displayMode() const
{
	return value("general/ui/displayMode", QVariant(2)).toInt();
}
void ApplicationSettings::setDisplayMode(int displayMode)
{
	setValue("general/ui/displayMode", displayMode);
}

int ApplicationSettings::darkMode() const
{
	return value("general/ui/darkMode", QVariant(0)).toInt();
}
void ApplicationSettings::setDarkMode(int darkModel)
{
	setValue("general/ui/darkMode", darkModel);
}
