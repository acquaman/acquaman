#include "VESPERSXASScanConfigurationView.h"
#include "ui/AMTopFrame.h"

#include <QVBoxLayout>

VESPERSXASScanConfigurationView::VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	AMTopFrame *frame = new AMTopFrame("VESPERS XAS for teh win!");
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(frame);
	setLayout(layout);
}

const AMScanConfiguration *VESPERSXASScanConfigurationView::configuration() const
{
	return config_;
}
