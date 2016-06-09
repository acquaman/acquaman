#include "PGMXASScanConfigurationView.h"

#include <QVBoxLayout>

PGMXASScanConfigurationView::PGMXASScanConfigurationView(PGMXASScanConfiguration *configuration, QWidget *parent)
	: AMScanConfigurationView(parent)
{
	configuration_ = configuration;

	scanName_ = new QLineEdit();
	scanName_->setText(configuration_->name());

	connect(scanName_, SIGNAL(editingFinished()), this, SLOT(onScanNameEdited()));
	connect(configuration_, SIGNAL(nameChanged(QString)), scanName_, SLOT(setText(QString)));

	regionsView_ = new AMStepScanAxisView("VLS-PGM Region Configuration", configuration_);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(regionsView_);
	mainLayout->addWidget(scanName_);

	setLayout(mainLayout);
}

void PGMXASScanConfigurationView::onScanNameEdited()
{
	configuration_->setName(scanName_->text());
	configuration_->setUserScanName(scanName_->text());
}

