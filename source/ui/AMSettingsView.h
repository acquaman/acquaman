#ifndef AMSETTINGSVIEW_H
#define AMSETTINGSVIEW_H

#include <QWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>

class AMUserSettingsView : public QGroupBox
{
Q_OBJECT
public:
	AMUserSettingsView(QWidget *parent = 0);

protected:
	QLineEdit *userDataFolderLineEdit_;
	QLineEdit *userDatabaseFileNameLineEdit_;

	QFormLayout *fl_;
};

class AMSettingsView : public QGroupBox
{
Q_OBJECT

public:
	AMSettingsView(QWidget *parent = 0);

protected:
	QLineEdit *publicDataFolderLineEdit_;
	QLineEdit *publicDatabaseFileNameLineEdit_;
	QLineEdit *fileLoaderPluginsFolderLineEdit_;

	QFormLayout *fl_;
};

class AMSettingsMasterView : public QWidget
{
Q_OBJECT
public:
	AMSettingsMasterView(QWidget *parent = 0);

protected:
	AMUserSettingsView *userSettingsView_;
	AMSettingsView *settingsView_;

	QVBoxLayout *vl_;
};

#endif // AMSETTINGSVIEW_H
