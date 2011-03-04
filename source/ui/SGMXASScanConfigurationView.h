/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
#include "ui_SGMXASScanConfigurationViewer.h"
#include "AMControlSetView.h"
#include "AMDetectorView.h"
#include "AMXASRegionsView.h"
#include "AMRegionsLineView.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSpacerItem>
#include "acquaman/SGM/SGMXASScanConfiguration.h"
#include "acquaman/SGM/SGMXASDacqScanController.h"
//#include "AMScanConfigurationViewer.h"
#include "AMScanConfigurationView.h"
#include "ui/AMControlOptimizationView.h"


class SGMXASScanConfigurationView : public AMScanConfigurationView, private Ui::SGMXASScanConfigurationViewer {
Q_OBJECT
public:
		SGMXASScanConfigurationView(SGMXASScanConfiguration *sxsc, AMDetectorInfoSet *cfgDetectorInfoSet, QWidget *parent = 0);
		~SGMXASScanConfigurationView();

		const AMScanConfiguration* configuration() const;

public slots:
	void onAddRegionClicked();

protected slots:
	void onRegionsChanged(){
		/* DAVID CHECK ME
		if(cfg_ && fluxResolutionView_){
			fluxResolutionView_->onRegionsUpdate( ((SGMXASScanConfiguration*)cfg_)->regions() );
		}
		*/
	}

	void onSGMBeamlineCriticalControlsConnectedChanged();

	/*
	void onLockdowScanning(bool isLocked, QString reason);
	*/

protected:
	SGMXASScanConfiguration *cfg_;

	AMXASRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	AMCompactControlOptimizationSetView *fluxResolutionView_;
	AMControlSetView *trackingView_;
	AMDetectorInfoSetView *detectorView_;
	AMDetectorInfoSet *cfgDetectorInfoSet_;

	QLabel *warningsLabel_;
	QVBoxLayout vl_;
	QGridLayout gl_;
};

#endif // SGMXASSCANCONFIGURATIONVIEWER_H
