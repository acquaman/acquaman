#ifndef CLSJJSLITSCANCONFIGURATIONVIEW_H
#define CLSJJSLITSCANCONFIGURATIONVIEW_H

#include "acquaman/CLS/CLSJJSlitScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"

class CLSJJSlitScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitScanConfigurationView(CLSJJSlitScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitScanConfigurationView();

	/// Returns the configuration being viewed.
	virtual CLSJJSlitScanConfiguration* configuration() const { return configuration_; }

signals:

public slots:

protected:

};

#endif // CLSJJSLITSCANCONFIGURATIONVIEW_H
