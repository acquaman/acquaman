#include "SGMSSAWidget.h"

SGMSSAWidget::SGMSSAWidget(QWidget *parent) :
	QWidget(parent)
{
	// Initialize AMBeamline::bl() as an SGMBeamline::sgm() instance. FIRST!
	SGMBeamline::sgm();

	sampleManipulatorView_ = new SGMSampleManipulatorView(true);

	QVBoxLayout *vl = new QVBoxLayout();
	vl->addWidget(sampleManipulatorView_);

	setLayout(vl);
}
