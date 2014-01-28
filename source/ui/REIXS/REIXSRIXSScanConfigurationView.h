#ifndef REIXSRIXSSCANCONFIGURATIONVIEW_H
#define REIXSRIXSSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

class REIXSRIXSScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT
public:
	explicit REIXSRIXSScanConfigurationView(REIXSXESScanConfiguration* config = 0, QWidget *parent = 0);
	virtual ~REIXSRIXSScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const { return config_; }

protected:

	REIXSXESScanConfiguration* config_;

signals:

public slots:

};

#endif // REIXSRIXSSCANCONFIGURATIONVIEW_H
