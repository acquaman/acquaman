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
#include "util/SGM/SGMElementInfo.h"

class QDoubleSpinBox;
class QSpinBox;
class QComboBox;
class QPushButton;
class QLabel;
class QVBoxLayout;

class SGMEnergyPositionView : public QGroupBox
{
Q_OBJECT

public:
	enum EnergyPositionViewMode{
		ViewModeAll,
		ViewModeStartOrEnd,
		ViewModeMiddle
	};

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
};

class SGMEnergyPositionWBeamlineView : public SGMEnergyPositionView
{
Q_OBJECT
public:
	SGMEnergyPositionWBeamlineView(SGMEnergyPosition *energyPosition, SGMEnergyPositionView::EnergyPositionViewMode alternateViewMode = SGMEnergyPositionView::ViewModeAll, QWidget *parent = 0);

protected slots:
	void onSetFromBeamlineButtonClicked();

protected:
	QPushButton *setFromBeamlineButton_;
};

#endif // SGMENERGYPOSITIONVIEW_H
