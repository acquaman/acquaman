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


#ifndef SGMFASTSCANCONFIGURATION2013VIEW_H
#define SGMFASTSCANCONFIGURATION2013VIEW_H

class QGridLayout;
class QComboBox;
class QLabel;
class QFileDialog;
class QDoubleSpinBox;
class QSpinBox;
class QLineEdit;
class QFormLayout;

class AMTopFrame;
class SGMFastScanCreateSampleScanListView;

#include <algorithm>

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMFastScanConfiguration2013.h"
#include "ui/util/SGM/SGMEnergyPositionView.h"
#include "ui/util/SGM/SGMFastScanSettingsView.h"
#include "beamline/AMDetectorSelector.h"
#include "ui/beamline/AMDetectorSelectorView.h"
#include "ui/AMRegExpLineEdit.h"

class SGMFastScanConfiguration2013View : public AMScanConfigurationView
{
	Q_OBJECT
public:
 	virtual ~SGMFastScanConfiguration2013View();
	SGMFastScanConfiguration2013View(SGMFastScanConfiguration2013 *sfsc, QWidget *parent = 0);

	const AMScanConfiguration* configuration() const;

	void setDetectorSelector(AMDetectorSelector *fastDetectorSelector);

signals:

protected slots:
	void onSGMBeamlineCriticalControlsConnectedChanged();

	void onScanNameEditChanged(const QString &scanName);

	void onParametersStartPositionChanged();
	void onParametersMiddlePositionChanged();
	void onParametersEndPositionChanged();
	void onFastScanSettingsChanged();

	void onStartPositionCopyChanged();
	void onMiddlePositionCopyChanged();
	void onEndPositionCopyChanged();
	void onFastScanSettingsCopyChanged();

	void onSetupSampleButtonClicked();
	void onSetupSampleViewDestroyed();

protected:
	SGMFastScanConfiguration2013 *cfg_;

	SGMEnergyPosition startPositionCopy_;
	SGMEnergyPosition middlePositionCopy_;
	SGMEnergyPosition endPositionCopy_;
	SGMFastScanSettings fastScanSettingsCopy_;

	SGMEnergyPositionWBeamlineView *startPositionView_;
	SGMEnergyPositionWBeamlineView *middlePositionView_;
	SGMEnergyPositionWBeamlineView *endPositionView_;
	SGMFastScanSettingsView *fastScanSettingsView_;

	////////////////////////////////////////
	// THINGS I SHOULDN'T HAVE ACCESS TO //
	//////////////////////////////////////
	AMDetectorSelector *fastDetectorSelector_;

	AMDetectorSelectorView *fastDetectorSelectorView_;

	QVBoxLayout *settingsLayout_;
	AMTopFrame *topFrame_;

	QComboBox *presetsComboBox_;
	QLabel *warningsLabel_;
	QCheckBox *enableUpDownScanningCheckBox_;

	QPushButton *setupSampleButton_;
	SGMFastScanCreateSampleScanListView *setupSampleView_;

	QLabel *scanNameLabel_;
	AMRegExpLineEdit *scanNameEdit_;
};

#endif // SGMFASTSCANCONFIGURATION2013VIEW_H
