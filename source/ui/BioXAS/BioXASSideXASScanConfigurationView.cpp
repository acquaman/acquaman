#include "BioXASSideXASScanConfigurationView.h"

#include "ui/dataman/AMStepScanAxisView.h"

BioXASSideXASScanConfigurationView::BioXASSideXASScanConfigurationView(BioXASSideXASScanConfiguration *configuration, QWidget *parent)
    : AMScanConfigurationView(parent)
{
    configuration_ = configuration;

    // Regions setup
    AMStepScanAxisView *regionsView = new AMStepScanAxisView("Energy Region Configuration", configuration_);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(regionsView);

    setLayout(layout);
}

BioXASSideXASScanConfigurationView::~BioXASSideXASScanConfigurationView()
{

}
