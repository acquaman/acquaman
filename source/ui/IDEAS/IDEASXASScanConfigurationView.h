#ifndef IDEASXASSCANCONFIGURATIONVIEW_H
#define IDEASXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include <QPushButton>

class AMScanController;
class IDEASXASScanConfiguration;
class AMRegionsView;
class AMTopFrame;

class IDEASXASScanConfigurationView : public AMScanConfigurationView
{
Q_OBJECT

public:
	IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent = 0);
	virtual ~IDEASXASScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const;

protected:
	IDEASXASScanConfiguration *configuration_;

	AMTopFrame *topFrame_;
	AMRegionsView *regionsView_;
        QPushButton *autoRegionButton_;

protected slots:
        void onAutoRegionButtonClicked();
};

#endif // IDEASXASSCANCONFIGURATIONVIEW_H
