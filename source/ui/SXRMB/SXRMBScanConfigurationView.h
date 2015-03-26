#ifndef SXRMBSCANCONFIGURATIONVIEW_H
#define SXRMBSCANCONFIGURATIONVIEW_H

#include <QComboBox>

#include "ui/acquaman/AMScanConfigurationView.h"

class SXRMBScanConfigurationView : public AMScanConfigurationView
{
public:
	/// Constructor takes an SXRMB scan configuration to view
	SXRMBScanConfigurationView(QWidget *parent = 0);
	/// Destructor
	virtual ~SXRMBScanConfigurationView();

protected slots:

protected:
	QComboBox *createFluorescenceComboBox();


};

#endif // SXRMBSCANCONFIGURATIONVIEW_H
