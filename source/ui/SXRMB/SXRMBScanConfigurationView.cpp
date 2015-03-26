#include "SXRMBScanConfigurationView.h"

#include "application/SXRMB/SXRMB.h"

SXRMBScanConfigurationView::~SXRMBScanConfigurationView()
{

}

SXRMBScanConfigurationView::SXRMBScanConfigurationView(QWidget *parent)
	:AMScanConfigurationView(parent)
{

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
