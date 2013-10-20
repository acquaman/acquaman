#ifndef SGMXASSCANCONFIGURATION2013VIEW_H
#define SGMXASSCANCONFIGURATION2013VIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"

#include "ui/AMTopFrame.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"
#include "beamline/AMDetectorSelector.h"
#include "ui/beamline/AMDetectorSelectorView.h"

#include "ui/beamline/AMControlSetView.h"
#include "ui/SGM/SGMFluxResolutionPickerView.h"

class SGMXASScanConfiguration2013View : public AMScanConfigurationView
{
Q_OBJECT

public:
	SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *configuration, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const;

	void setDetectorSelector(AMDetectorSelector *xasDetectorSelector);
	void setTrackingSet(AMControlSet *trackingSet);

signals:
	void scanControllerCreated(AMScanController *scanController);

protected slots:
	void onTrackingGroupChanged();
	void onFluxResolutionGroupChanged();

	void onScanNameEditChanged(const QString &scanName);

protected:
	SGMXASScanConfiguration2013 *configuration_;

	////////////////////////////////////////
	// THINGS I SHOULDN'T HAVE ACCESS TO //
	//////////////////////////////////////
	AMDetectorSelector *xasDetectorSelector_;
	AMControlSet *trackingSet_;
	AMControlSetView *trackingSetView_;
	SGMFluxResolutionPickerView *fluxResolutionView_;

	AMTopFrame *topFrame_;
	AMRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	AMDetectorSelectorView *xasDetectorSelectorView_;

	QVBoxLayout *mainVL_;
	QGridLayout *bottomGL_;

	QLabel *scanNameLabel_;
	QLineEdit *scanNameEdit_;
};

#endif // SGMXASSCANCONFIGURATION2013VIEW_H
