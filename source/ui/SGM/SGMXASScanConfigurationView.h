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


#ifndef ACQMAN_SGMXASScanConfigurationViewer_H
#define ACQMAN_SGMXASScanConfigurationViewer_H

#include <QtGui>

class QPushButton;
class QVBoxLayout;
class QGridLayout;
class QSpacerItem;

#include "ui/beamline/AMControlSetView.h"
#include "ui/beamline/AMOldDetectorSetView.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"
#include "ui/acquaman/AMScanConfigurationView.h"
#include "ui/beamline/AMControlOptimizationView.h"
#include "ui/AMTopFrame.h"

#include "ui/beamline/AMOldDetectorView.h"

class SGMFluxResolutionPickerView;

class SGMXASScanConfigurationView : public AMScanConfigurationView{
Q_OBJECT
public:
	SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, QWidget *parent = 0);
	~SGMXASScanConfigurationView();

	const AMScanConfiguration* configuration() const;

public slots:
	void setDisabled(bool disabled);

protected slots:
	void onDetectorConfigurationsChanged();
	void onSGMBeamlineCriticalControlsConnectedChanged();

	void onScanNameEditChanged(const QString &scanName);

protected:
	SGMXASScanConfiguration *cfg_;

	AMTopFrame *topFrame_;
	AMRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	SGMFluxResolutionPickerView *fluxResolutionView_;
	AMControlSetView *trackingView_;
	AMOldDetectorSetView *xasDetectorsView_;

	QLabel *warningsLabel_;

	QVBoxLayout *mainVL_;
	QGridLayout *bottomGL_;

	QLabel *scanNameLabel_;
	QLineEdit *scanNameEdit_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
