#include "AMRegionScanConfigurationView.h"

#include "ui/AMTopFrame.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/acquaman/AMRegionsLineView.h"

#include <QBoxLayout>

AMRegionScanConfigurationView::AMRegionScanConfigurationView(AMRegionScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	// GUI setup
	//////////////////////////////

//	topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
//	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	regionsLineView_ = new AMRegionsLineView(configuration_->regions());
	regionsView_ = new AMRegionsView(configuration_->regions());

	QVBoxLayout* vl = new QVBoxLayout();
	vl->addWidget(regionsLineView_);
	vl->addWidget(regionsView_);
	setLayout(vl);
}

AMRegionScanConfigurationView::~AMRegionScanConfigurationView() {

}



