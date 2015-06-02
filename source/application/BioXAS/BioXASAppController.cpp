#include "BioXASAppController.h"

BioXASAppController::BioXASAppController(QObject *parent) :
    AMAppController(parent)
{

}

BioXASAppController::~BioXASAppController()
{

}

void BioXASAppController::shutdown()
{
	// Make sure we release/clean-up the beamline interface
	AMBeamline::releaseBl();
	AMAppController::shutdown();
}

QGroupBox* BioXASAppController::createSqueezeGroupBoxWithView(QWidget *view, const QString &title, const QString &iconName)
{
	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addStretch();
	horizontalLayout->addWidget(view);
	horizontalLayout->addStretch();

	QVBoxLayout *verticalLayout = new QVBoxLayout;
	verticalLayout->addWidget(new AMTopFrame(title, QIcon(iconName)));
	verticalLayout->addStretch();
	verticalLayout->addLayout(horizontalLayout);
	verticalLayout->addStretch();

	QGroupBox *groupBox = new QGroupBox();
	groupBox->setFlat(true);
	groupBox->setLayout(verticalLayout);

	return groupBox;
}

void BioXASAppController::registerClasses()
{
	AMDbObjectSupport::s()->registerClass<CLSSIS3820ScalerDarkCurrentMeasurementActionInfo>();
//	AMDbObjectSupport::s()->registerClass<AM1DDarkCurrentCorrectionAB>();
	AMDbObjectSupport::s()->registerClass<BioXASScanConfigurationDbObject>();
	AMDbObjectSupport::s()->registerClass<BioXASUserConfiguration>();
//	AMDbObjectSupport::s()->registerClass<BioXASXRFScanConfiguration>();
}
