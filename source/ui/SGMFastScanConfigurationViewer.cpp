#include "SGMFastScanConfigurationViewer.h"

SGMFastScanConfigurationViewer::SGMFastScanConfigurationViewer(SGMFastScanConfiguration *sfsc, QWidget *parent) :
		QWidget(parent)
{
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sfsc;

		availableElements_ = new QComboBox();
		availableElements_->addItems(sfsc->options());
		gl_.addWidget(availableElements_,	0, 0, 1, 1, Qt::AlignCenter);
		this->setLayout(&gl_);
		this->setMaximumSize(800, 800);
	}
}
