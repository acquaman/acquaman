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


#ifndef SGMENERGYPOSITIONVIEW_H
#define SGMENERGYPOSITIONVIEW_H

#include <QGroupBox>
#include <QDialog>
#include "util/SGM/SGMElementInfo.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class QPushButton;
class QLabel;
class QCheckBox;
class QLineEdit;
class QDialogButtonBox;
class QVBoxLayout;
class QHBoxLayout;

class SGMEnergyPositionView : public QGroupBox
{
Q_OBJECT

public:
	enum EnergyPositionViewMode{
		ViewModeAll,
		ViewModeStartOrEnd,
		ViewModeMiddle
	};

 	virtual ~SGMEnergyPositionView();
	SGMEnergyPositionView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode = SGMEnergyPositionView::ViewModeAll, QWidget *parent = 0);

public slots:
	void setEnergyPosition(SGMEnergyPosition *energyPosition);

protected slots:
	void onEnergyEditingFinished();
	void onMonoEditingFinished();
	void onUndulatorEditingFinished();
	void onExitSlitEditingFinished();

	void onAlternateViewModeClicked();
	void onViewModeChanged();

protected:
	SGMEnergyPosition *energyPosition_;
	SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode_;
	SGMEnergyPositionView::EnergyPositionViewMode currentViewMode_;

	QDoubleSpinBox *energySpinBox_;
	QSpinBox *monoEncoderTargetSpinBox_;
	QSpinBox *undulatorStepSetpointSpinBox_;
	QDoubleSpinBox *exitSlitDistanceSpinBox_;
	QComboBox *sgmGratingComboBox_;

	QLabel *energyLabel_;
	QLabel *monoEncoderLabel_;
	QLabel *undulatorStartStepLabel_;
	QLabel *exitSlitDistanceLabel_;
	QLabel *sgmGratingLabel_;
	QPushButton *alternateViewModeButton_;

	QVBoxLayout *vl2_;
	QHBoxLayout *hl_;
};

class SGMEnergyPositionWBeamlineView : public SGMEnergyPositionView
{
Q_OBJECT
public:
 	virtual ~SGMEnergyPositionWBeamlineView();
	SGMEnergyPositionWBeamlineView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode = SGMEnergyPositionView::ViewModeAll, QWidget *parent = 0);

protected slots:
	void onSetFromBeamlineButtonClicked();

protected:
	QPushButton *setFromBeamlineButton_;
};

class SGMEnergyPositionDisassociateFromDbDialog : public QDialog
{
Q_OBJECT
public:
 	virtual ~SGMEnergyPositionDisassociateFromDbDialog();
	SGMEnergyPositionDisassociateFromDbDialog(SGMEnergyPosition *energyPosition, QWidget *parent);

public slots:
	virtual void accept();

protected slots:
	void onNewNameLineEditTextEdited(const QString &text);

protected:
	SGMEnergyPosition *energyPosition_;

	QLineEdit *newNameLineEdit_;
	QDialogButtonBox *buttonBox_;

	QStringList takenNames_;
};

class SGMEnergyPositionWBeamlineAndDatabaseView : public SGMEnergyPositionWBeamlineView
{
Q_OBJECT
public:
 	virtual ~SGMEnergyPositionWBeamlineAndDatabaseView();
	SGMEnergyPositionWBeamlineAndDatabaseView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode = SGMEnergyPositionView::ViewModeAll, QWidget *parent = 0);

	QStringList alsoUsedByList() const;

protected slots:
	void onDisassociateButtonClicked();
	void onUnlockDisassociateCheckBoxToggled(bool toggled);

protected:
	void setUsedByLabelHelper();

protected:
	QLabel *databaseUsedByLabel_;
	QPushButton *disassociateButton_;
	QCheckBox *unlockDisassociateCheckBox_;

	QStringList alsoUsedByList_;
};

#endif // SGMENERGYPOSITIONVIEW_H
