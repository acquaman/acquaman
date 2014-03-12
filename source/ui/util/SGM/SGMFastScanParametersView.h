/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").
Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SGMFASTSCANPARAMETERSVIEW_H
#define SGMFASTSCANPARAMETERSVIEW_H

#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"

#define SGMFASTSCANPARAMETERSVIEW_FAILED_TO_LOAD_SAVE_DATABASE 370270
#define SGMFASTSCANPARAMETERSVIEW_FAILED_NAMING_CHECK 370271
#define SGMFASTSCANPARAMETERSVIEW_FAILED_TO_SAVE_NEW_PARAMETERS 370272

class QHBoxLayout;

class SGMFastScanParametersView : public QWidget
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersView();
	SGMFastScanParametersView(SGMFastScanParameters *fastScanParameters, bool disableCopy = false, QWidget *parent = 0);

	bool hasUnsavedChanges() const;

signals:
	void unsavedChanges(bool hasUnsavedChanges);

protected slots:
	void onEditButtonClicked();
	void onCopyButtonClicked();

	void onSaveButtonClicked();
	void onCancelButtonClicked();

	void onAnySettingChanged();

protected:
	SGMFastScanParameters *fastScanParameters_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;
	SGMFastScanSettings fastScanSettingsCopy_;

	SGMEnergyPositionView *startPositionView_;
	SGMEnergyPositionView *middlePositionView_;
	SGMEnergyPositionView *endPositionView_;
	SGMFastScanSettingsView *fastScanSettingsView_;

	QPushButton *copyButton_;
	QPushButton *editButton_;

	QPushButton *saveButton_;
	QPushButton *cancelButton_;

	QHBoxLayout *buttonsHL1_;
	QHBoxLayout *buttonsHL2_;
	QVBoxLayout *masterVL_;

	bool disableCopy_;
	bool hasUnsavedChanges_;
};

class QLineEdit;

class SGMFastScanParametersDatabaseSaveView : public QWidget
{
Q_OBJECT
public:
 	virtual ~SGMFastScanParametersDatabaseSaveView();
	SGMFastScanParametersDatabaseSaveView(SGMFastScanParameters *fastScanParameters, QWidget *parent = 0);

protected slots:
	void onUnsavedChanges(bool hasUnsavedChanges);
	void onDatabasesComboBoxIndexChanged(int index);
	void onNewNameLineEditTextEdited(const QString &text);

	void onSaveButtonClicked();

protected:
	void checkNewNameIsValid();

protected:
	SGMFastScanParameters *fastScanParameters_;
	SGMFastScanParametersView *parametersView_;
	QComboBox *databasesComboBox_;
	QComboBox *elementsComboBox_;
	QLineEdit *newNameLineEdit_;

	QPushButton *saveButton_;
	QPushButton *cancelButton_;

	QList<QStringList> takenNames_;
};

#endif // SGMFASTSCANPARAMETERSVIEW_H
