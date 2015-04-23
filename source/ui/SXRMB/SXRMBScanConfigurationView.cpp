#include "SXRMBScanConfigurationView.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>

#include "application/SXRMB/SXRMB.h"

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
	newComboBox->insertItem(0, "None");
	newComboBox->insertItem(1, "Bruker");
	newComboBox->insertItem(2, "4E Vortex");
	newComboBox->insertItem(3, "Bruker & 4E");

	return newComboBox;
}

QGroupBox *SXRMBScanConfigurationView::createAndLayoutDetectorSettings(SXRMBScanConfiguration * configuration)
{
	fluorescenceDetectorComboBox_ = createFluorescenceComboBox();

	QHBoxLayout *brukerDetectorHLayout = new QHBoxLayout;
	brukerDetectorHLayout->addWidget(new QLabel("Choose Bruker"));
	brukerDetectorHLayout->addWidget(fluorescenceDetectorComboBox_);

	powerOnTEYHVControlCheckBox_ = new QCheckBox("Power on TEY HV Control automatically");

	QVBoxLayout *detectorBoxLayout = new QVBoxLayout;
	detectorBoxLayout->addLayout(brukerDetectorHLayout);
	detectorBoxLayout->addWidget(powerOnTEYHVControlCheckBox_);

	QGroupBox * detectorSettingGroupBox = new QGroupBox("Detector Setting");
	detectorSettingGroupBox->setLayout(detectorBoxLayout);

	connect(powerOnTEYHVControlCheckBox_, SIGNAL(clicked(bool)), this, SLOT(onPowerOnTEYHVControlEnabled(bool)));
	connect(fluorescenceDetectorComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onFluorescenceDetectorChanged(int)));
	connect(configuration->dbObject(), SIGNAL(fluorescenceDetectorsChanged(SXRMB::FluorescenceDetectors)), this, SLOT(updateFluorescenceDetectorComboBox(SXRMB::FluorescenceDetectors)));

	// default using bruker
	updateFluorescenceDetectorComboBox(SXRMB::Bruker);

	return detectorSettingGroupBox;
}

