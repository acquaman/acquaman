#include "VESPERSXASScanConfigurationView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/AMTopFrame.h"

#include <QVBoxLayout>

VESPERSXASScanConfigurationView::VESPERSXASScanConfigurationView(VESPERSXASScanConfiguration *config, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	config_ = config;
	AMTopFrame *frame = new AMTopFrame("VESPERS XAS for teh win!");

	regionsView_ = new AMXASRegionsView(config_->regions(), this);
	regionsView_->setBeamlineEnergy(VESPERSBeamline::vespers()->energyRelative());

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(frame);
	layout->addWidget(regionsView_);
	setLayout(layout);
}

const AMScanConfiguration *VESPERSXASScanConfigurationView::configuration() const
{
	return config_;
}
