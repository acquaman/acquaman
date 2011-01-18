#include "SGMFastScanConfigurationViewer.h"

SGMFastScanConfigurationViewer::SGMFastScanConfigurationViewer(SGMFastScanConfiguration *sfsc, QWidget *parent) :
		AMScanConfigurationViewer(sfsc, parent)
{
	cfg_ = NULL;
	if(SGMBeamline::sgm()->isConnected()){
		cfg_ = sfsc;

		availableElements_ = new QComboBox();
		availableElements_->addItems(sfsc->options());

		startScanButton_ = new QPushButton();
		startScanButton_->setText("Start Scan");
		addToQueueButton_ = new QPushButton();
		addToQueueButton_->setText("Add Scan to Queue");
		queueDirectorButton_ = new QPushButton();
		queueDirectorButton_->setText("Queue Director");
		int buttonWidth = std::max(startScanButton_->sizeHint().width(), addToQueueButton_->sizeHint().width());
		buttonWidth = std::max(buttonWidth, queueDirectorButton_->sizeHint().width());
		int buttonHeight = std::max(startScanButton_->sizeHint().height(), addToQueueButton_->sizeHint().height());
		buttonHeight = std::max(buttonHeight, queueDirectorButton_->sizeHint().height());
		qDebug() << "height: " << buttonHeight;
		startScanButton_->setMinimumSize(buttonWidth, (int)(1.75*buttonHeight));
		addToQueueButton_->setMinimumSize(buttonWidth, buttonHeight);
		queueDirectorButton_->setMinimumSize(buttonWidth, buttonHeight);
		connect(startScanButton_, SIGNAL(clicked()), this, SLOT(onStartScanClicked()));
		connect(addToQueueButton_, SIGNAL(clicked()), this, SLOT(onAddToQueueRequested()));
		connect(queueDirectorButton_, SIGNAL(clicked()), this, SIGNAL(queueDirectorRequested()));

		gl_.addWidget(availableElements_,	0, 0, 1, 1, Qt::AlignCenter);
		gl_.addWidget(startScanButton_,		5, 3, 1, 2, Qt::AlignRight);
		gl_.addWidget(addToQueueButton_,	6, 3, 1, 2, Qt::AlignRight);
		gl_.addWidget(queueDirectorButton_,	7, 3, 1, 2, Qt::AlignRight);
		this->setLayout(&gl_);
		this->setMaximumSize(800, 800);
	}
}

void SGMFastScanConfigurationViewer::onLockdowScanning(bool isLocked, QString reason){
	if(isLocked){
		startScanButton_->setEnabled(false);
		startScanButton_->setText("Start Scan\n"+reason);
	}
	else{
		startScanButton_->setEnabled(true);
		startScanButton_->setText("Start Scan");
	}
}
