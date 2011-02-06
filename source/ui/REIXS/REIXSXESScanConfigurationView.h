#ifndef REIXSXESSCANCONFIGURATIONVIEW_H
#define REIXSXESSCANCONFIGURATIONVIEW_H

#include "ui/AMScanConfigurationView.h"
#include "acquaman/REIXS/REIXSXESScanConfiguration.h"

class REIXSXESScanConfigurationView : public AMScanConfigurationView
{
	Q_OBJECT
public:
	explicit REIXSXESScanConfigurationView(QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const { return configuration_; }

signals:

public slots:

protected:
	/// The internal configuration that we modify
	REIXSXESScanConfiguration* configuration_;

};

#endif // REIXSXESSCANCONFIGURATIONVIEW_H
