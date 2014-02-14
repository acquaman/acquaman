#include "IDEASXASScanConfigurationView.h"

#include <QBoxLayout>

#include "acquaman/AMScanController.h"
#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"
#include "ui/acquaman/AMRegionsView.h"
#include "ui/AMTopFrame.h"

IDEASXASScanConfigurationView::IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent) :
	AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	topFrame_ = new AMTopFrame("Configure an XAS Scan to Run Later");
	topFrame_->setIcon(QIcon(":/utilities-system-monitor.png"));

	regionsView_ = new AMRegionsView(configuration_->regions(), this);

	QVBoxLayout *mainVL = new QVBoxLayout();
	mainVL->addWidget(topFrame_);
	mainVL->addWidget(regionsView_);

	mainVL->setContentsMargins(0,0,0,0);
	mainVL->setSpacing(1);
	setLayout(mainVL);
}

IDEASXASScanConfigurationView::~IDEASXASScanConfigurationView()
{

}

const AMScanConfiguration* IDEASXASScanConfigurationView::configuration() const{
	return configuration_;
}
