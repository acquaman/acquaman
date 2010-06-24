#include "AMScanConfigurationView.h"
#include "ui_AMScanConfigurationView.h"

AMScanConfigurationView::AMScanConfigurationView(QWidget *parent) :
	QWidget(parent)
{
	setupUi(this);
	connect(xasButton, SIGNAL(clicked()), this, SLOT(viewXASScan()));
	connect(xesButton, SIGNAL(clicked()), this, SLOT(viewXESScan()));
}

AMScanConfigurationView::~AMScanConfigurationView(){
}

void AMScanConfigurationView::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		retranslateUi(this);
		break;
	default:
		break;
	}
}
