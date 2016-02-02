
#include "SXRMBScanConfigurationView.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>

#include "application/SXRMB/SXRMB.h"
#include "beamline/SXRMB/SXRMBBeamline.h"

SXRMBScanConfigurationView::~SXRMBScanConfigurationView()
{

}

SXRMBScanConfigurationView::SXRMBScanConfigurationView(QWidget *parent)
	:AMScanConfigurationView(parent)
{

}

void SXRMBScanConfigurationView::updateFluorescenceDetectorComboBox(SXRMB::FluorescenceDetectors detector)
{
	fluorescenceDetectorComboBox_->setCurrentIndex(int(detector));
}

QComboBox *SXRMBScanConfigurationView::createFluorescenceComboBox()
{
	QComboBox *newComboBox = new QComboBox;
	newComboBox->insertItem(0, "Transmission");
	newComboBox->insertItem(1, "Bruker");
	newComboBox->insertItem(2, "4E Vortex");
//	newComboBox->insertItem(3, "Bruker & 4E");

	return newComboBox;
}

QGroupBox *SXRMBScanConfigurationView::createAndLayoutDetectorSettings(SXRMBScanConfiguration * configuration)
{
	fluorescenceDetectorComboBox_ = createFluorescenceComboBox();

	QHBoxLayout *brukerDetectorHLayout = new QHBoxLayout;
	brukerDetectorHLayout->addWidget(new QLabel("Choose Detector"));
	brukerDetectorHLayout->addWidget(fluorescenceDetectorComboBox_);

	powerOnHVControlCheckBox_ = new QCheckBox("Power on TEY HV Control automatically");
	updatePowerOnHVControlCheckBoxText();

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout;
	detectorBoxLayout->addLayout(brukerDetectorHLayout);
	detectorBoxLayout->addWidget(powerOnHVControlCheckBox_);

	QGroupBox * detectorSettingGroupBox = new QGroupBox("Detector Setting");
	detectorSettingGroupBox->setLayout(detectorBoxLayout);

	connect(powerOnHVControlCheckBox_, SIGNAL(clicked(bool)), this, SLOT(onPowerOnTEYHVControlEnabled(bool)));
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceDetectorChanged(int)));
	connect(configuration->dbObject(), SIGNAL(fluorescenceDetectorChanged(SXRMB::FluorescenceDetectors)), this, SLOT(updateFluorescenceDetectorComboBox(SXRMB::FluorescenceDetectors)));

	updateFluorescenceDetectorComboBox(configuration->fluorescenceDetector());

	return detectorSettingGroupBox;
}

void SXRMBScanConfigurationView::updatePowerOnHVControlCheckBoxText()
{
	QString text = "Power on TEY HV Control automatically";

	SXRMB::Endstation curEndstation = SXRMBBeamline::sxrmb()->currentEndstation();
	if (curEndstation == SXRMB::AmbiantWithGasChamber || curEndstation == SXRMB::AmbiantWithoutGasChamber) {
		text = "Power on IC0/IC1 HV Control automatically";
	}

	powerOnHVControlCheckBox_->setText(text);
}

