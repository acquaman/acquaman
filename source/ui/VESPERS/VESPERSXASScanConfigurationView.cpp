#include "VESPERSXASScanConfigurationView.h"
#include "ui/AMTopFrame.h"

#include <QVBoxLayout>

VESPERSXASScanConfigurationView::VESPERSXASScanConfigurationView()
{
	AMTopFrame *frame = new AMTopFrame("VESPERS XAS for teh win!");
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(frame);
	setLayout(layout);
}

const AMScanConfiguration *VESPERSXASScanConfigurationView::configuration()
{
	return config_;
}
