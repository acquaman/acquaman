#ifndef SGMXASSCANCONFIGURATION2013VIEW_H
#define SGMXASSCANCONFIGURATION2013VIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"

#include "ui/AMTopFrame.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"
#include "beamline/AMDetectorSelector.h"
#include "ui/beamline/AMDetectorSelectorView.h"

class SGMXASScanConfiguration2013View : public AMScanConfigurationView
{
Q_OBJECT

public:
	SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *configuration, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const;

	void setDetectorSelector(AMDetectorSelector *xasDetectorSelector);

signals:
	void scanControllerCreated(AMScanController *scanController);

protected slots:
	void onCreateNewScanActionButtonClicked();

protected:
	SGMXASScanConfiguration2013 *configuration_;
	AMDetectorSelector *xasDetectorSelector_;

	AMTopFrame *topFrame_;
	AMRegionsView *regionsView_;
	AMRegionsLineView *regionsLineView_;
	AMDetectorSelectorView *xasDetectorSelectorView_;

	QVBoxLayout *mainVL_;
	//QGridLayout *bottomGL_;
	QHBoxLayout *topHL_;
	QHBoxLayout *bottomHL_;

	QPushButton *createNewScanActionButton_;
};

#endif // SGMXASSCANCONFIGURATION2013VIEW_H
