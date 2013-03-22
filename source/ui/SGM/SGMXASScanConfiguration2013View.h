#ifndef SGMXASSCANCONFIGURATION2013VIEW_H
#define SGMXASSCANCONFIGURATION2013VIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"
#include "acquaman/SGM/SGMXASScanConfiguration2013.h"

class SGMXASScanConfiguration2013View : public AMScanConfigurationView
{
Q_OBJECT

public:
	SGMXASScanConfiguration2013View(SGMXASScanConfiguration2013 *configuration, QWidget *parent = 0);

	virtual const AMScanConfiguration* configuration() const;

protected:
	SGMXASScanConfiguration2013 *configuration_;
};

#endif // SGMXASSCANCONFIGURATION2013VIEW_H
