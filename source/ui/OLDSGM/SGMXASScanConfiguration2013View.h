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


#ifndef SGMXASSCANCONFIGURATION2013VIEW_H
#define SGMXASSCANCONFIGURATION2013VIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"

#include "ui/AMTopFrame.h"
#include "beamline/AMDetectorSelector.h"
#include "ui/beamline/AMDetectorSelectorView.h"

#include "ui/beamline/AMControlSetView.h"
#include "ui/SGM/SGMFluxResolutionPickerView.h"
#include "ui/AMRegExpLineEdit.h"
class SGMXASScanConfiguration2013View : public AMScanConfigurationView
{
Q_OBJECT

public:
	virtual ~SGMXASScanConfiguration2013View();
	SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *configuration, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const;

	void setDetectorSelector(AMDetectorSelector *xasDetectorSelector);

signals:
	void scanControllerCreated(AMScanController *scanController);

protected slots:
	void onFluxResolutionGroupChanged();
	void onScanNameEditChanged(const QString &scanName);
	void onUndulatorTrackingChanged(bool isTracking);
	void onUndulatorTrackingButtonClicked();
	void onExitSlitTrackingChanged(bool isTracking);
	void onExitSlitTrackingButtonClicked();
	void onGratingTrackingChanged(bool isTracking);
	void onGratingTrackingButtonClicked();
	void onMatchingBeamlineSettingsChanged(bool matchedBeamline);
	void onGetBeamlineSettingsClicked();
protected:
	SGMXASScanConfiguration2013 *configuration_;

	////////////////////////////////////////
	// THINGS I SHOULDN'T HAVE ACCESS TO //
	//////////////////////////////////////
	AMDetectorSelector *xasDetectorSelector_;
	/*
	// Two lines removed as temporary fix (see Issue579)
	AMControlSet *trackingSet_;
	AMControlSetView *trackingSetView_;
	*/
	SGMFluxResolutionPickerView *fluxResolutionView_;

	AMTopFrame *topFrame_;
	AMDetectorSelectorView *xasDetectorSelectorView_;

	QVBoxLayout *mainVL_;
	QGridLayout *bottomGL_;

	QLabel *scanNameLabel_;
	AMRegExpLineEdit *scanNameEdit_;
	QPushButton* undulatorTrackingButton_;
	QPushButton* gratingTrackingButton_;
	QPushButton* exitSlitTrackingButton_;
	QLabel *matchesBeamlineWarning_;
	QPushButton* getBeamlineSettings_;

	void updateTrackingButtonStatus(QPushButton* button, bool isTracking);

	QHBoxLayout *topRow_;
	QHBoxLayout *bottomRow_;
	QHBoxLayout *warningRow_;
	QHBoxLayout *explanationRow_;
};

#endif // SGMXASSCANCONFIGURATION2013VIEW_H
