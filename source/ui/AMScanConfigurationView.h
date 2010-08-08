#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include "ui/SGMXASScanConfigurationViewer.h"
#include "ui/SGMXASScanConfigurationWizard.h"

class AMXASScanConfigurationHolder : public QWidget
{
	Q_OBJECT
public:
	AMXASScanConfigurationHolder(QWidget *parent = 0);
	~AMXASScanConfigurationHolder();

protected slots:
	void onSidebarLinkChanged(){
		if(!sxscViewer && isVisible() && SGMBeamline::sgm()->isConnected()){
			sxscViewer = new SGMXASScanConfigurationViewer();
			vl_ = new QVBoxLayout();
			vl_->addWidget(sxscViewer);
			this->setLayout(vl_);
		}
		if(!sxscWizard && isVisible() && SGMBeamline::sgm()->isConnected()){
			sxscWizard = new SGMXASScanConfigurationWizard();
			sxscWizard->show();
		}
	}

protected:
	SGMXASScanConfigurationViewer *sxscViewer;
	SGMXASScanConfigurationWizard *sxscWizard;
	QVBoxLayout *vl_;
};

#endif // AMSCANCONFIGURATIONVIEW_H
