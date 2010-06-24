#ifndef AMSCANCONFIGURATIONVIEW_H
#define AMSCANCONFIGURATIONVIEW_H

#include <QWidget>
#include "ui_AMScanConfigurationView.h"
#include "ui/SGMXASScanConfigurationViewer.h"

/*namespace Ui {
	class AMScanConfigurationView;
}

class AMScanConfigurationView : public QWidget {
	Q_OBJECT
public:
	AMScanConfigurationView(QWidget *parent = 0);
	~AMScanConfigurationView();

protected:
	void changeEvent(QEvent *e);

protected slots:
	void viewXASScan(){;}
	void viewXESScan(){;}

private:
	Ui::AMScanConfigurationView *ui;
};
*/

class AMScanConfigurationView : public QWidget, private Ui::AMScanConfigurationView {
	Q_OBJECT
public:
	AMScanConfigurationView(QWidget *parent = 0);
	~AMScanConfigurationView();

protected:
	void changeEvent(QEvent *e);

protected slots:
	void viewXASScan(){
		delete scansLabel;
		delete xasButton;
		delete xesButton;
		delete layout();
		sxscViewer = new SGMXASScanConfigurationViewer();
		vl_ = new QVBoxLayout();
		vl_->addWidget(sxscViewer);
		this->setLayout(vl_);
	}

	void viewXESScan(){;}

private:
	SGMXASScanConfigurationViewer *sxscViewer;
	QVBoxLayout *vl_;
};

#endif // AMSCANCONFIGURATIONVIEW_H
