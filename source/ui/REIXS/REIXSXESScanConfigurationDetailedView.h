#ifndef REIXSXESSCANCONFIGURATIONDETAILEDVIEW_H
#define REIXSXESSCANCONFIGURATIONDETAILEDVIEW_H

#include "ui/REIXS/REIXSXESScanConfigurationView.h"
#include "ui/REIXS/REIXSXESMCPDetectorView.h"

class REIXSXESScanConfigurationDetailedView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	explicit REIXSXESScanConfigurationDetailedView(REIXSXESMCPDetector* detector, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const { return basicConfigurationView_->configuration(); }

signals:

public slots:

protected:
	REIXSXESMCPDetectorView* detectorView_;
	REIXSXESScanConfigurationView* basicConfigurationView_;


};

#endif // REIXSXESSCANCONFIGURATIONDETAILEDVIEW_H
