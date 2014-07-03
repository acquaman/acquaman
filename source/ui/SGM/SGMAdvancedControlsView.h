/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef SGMADVANCEDCONTROLSVIEW_H
#define SGMADVANCEDCONTROLSVIEW_H

#include <QWidget>

#include "beamline/SGM/SGMBeamline.h"

class QRadioButton;
class AMExtendedControlEditor;
class QButtonGroup;
class QVBoxLayout;

class SGMAdvancedControlsView : public QWidget
{
Q_OBJECT
public:
 	virtual ~SGMAdvancedControlsView();
	SGMAdvancedControlsView(QWidget *parent = 0);

protected slots:
	void onCurrentMirrorStripeChanged(SGMBeamlineInfo::sgmMirrorStripe newMirrorStripe);
	void onMirrorStripeButtonsClicked(int buttonIndex);

	void onCurrentEndstationChanged(SGMBeamlineInfo::sgmEndstation newEndstation);
	void onEndstationButtonsClicked(int buttonIndex);

protected:
	AMExtendedControlEditor *undulatorHarmonicEditor_;
	AMExtendedControlEditor *undulatorOffsetEditor_;
	AMExtendedControlEditor *mirrorStripeFeedbackEditor_;
	QButtonGroup *mirrorStripeSelectionGroup_;
	QRadioButton *selectCarbonButton_;
	QRadioButton *selectSiliconButton_;
	AMExtendedControlEditor *entranceSlitEditor_;
	AMExtendedControlEditor *exitSlitEditor_;
	QButtonGroup *endstationsAvailable_;
	QRadioButton *scientaButton_;
	QRadioButton *ssaButton_;

	QVBoxLayout *mainVL_;
};

#endif // SGMADVANCEDCONTROLSVIEW_H
