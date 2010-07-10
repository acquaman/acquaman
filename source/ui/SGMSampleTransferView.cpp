#include "SGMSampleTransferView.h"

SGMSampleTransferView::SGMSampleTransferView(QWidget *parent) :
	QWidget(parent)
{
	mainLayout_ = new QStackedLayout();

	loadlockOutButton_ = new QPushButton("Transfer sample out of loadlock");
	loadlockInButton_ = new QPushButton("Transfer sample into loadlock");
	QList<QPushButton*> buttons;
	buttons.append(loadlockOutButton_);
	buttons.append(loadlockInButton_);
	transferBox_ = new SGMSampleTransferProceduresView("Transfer Procedures", buttons);
	loadlockOut_ = new SGMSampleTransferPaneView(SGMBeamline::sgm()->transferOutActions(), "Transfer Out of Loadlock");
	loadlockIn_ = new SGMSampleTransferPaneView(SGMBeamline::sgm()->transferInActions(), "Transfer Into Loadlock");
	connect(loadlockOutButton_, SIGNAL(clicked()), this, SLOT(drawLoadlockOut()));
	connect(loadlockInButton_, SIGNAL(clicked()), this, SLOT(drawLoadlockIn()));
	connect(loadlockOut_, SIGNAL(completed()), this, SLOT(drawMain()));
	connect(loadlockIn_, SIGNAL(completed()), this, SLOT(drawMain()));
	mainLayout_->addWidget(transferBox_);
	mainLayout_->addWidget(loadlockOut_);
	mainLayout_->addWidget(loadlockIn_);
	setLayout(mainLayout_);
}

void SGMSampleTransferView::drawMain(){
	mainLayout_->setCurrentIndex(0);
}

void SGMSampleTransferView::drawLoadlockOut(){
	mainLayout_->setCurrentIndex(1);
}

void SGMSampleTransferView::drawLoadlockIn(){
	mainLayout_->setCurrentIndex(2);
}


SGMSampleTransferProceduresView::SGMSampleTransferProceduresView(const QString &title, QList<QPushButton*> procedureButtons, QWidget *parent) :
		QGroupBox(title, parent)
{
	vl_ = new QVBoxLayout();
	mainLayout_ = new QGridLayout();
	procedureButtons_ = procedureButtons;
	for(int x = 0; x < procedureButtons_.count(); x++ )
		vl_->addWidget(procedureButtons_.at(x));
	mainLayout_->addLayout(vl_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	setLayout(mainLayout_);
}

SGMSampleTransferPaneView::SGMSampleTransferPaneView(QList<AMBeamlineActionItem*> items, const QString &title, QWidget *parent) :
		QGroupBox(title, parent)
{
	vl_ = new QVBoxLayout();
	mainLayout_ = new QGridLayout();
	QList<AMBeamlineActionItem*> transferActions = items;
	connect(transferActions.at(transferActions.count()-1), SIGNAL(succeeded()), this, SIGNAL(completed()));
	AMBeamlineActionItemView *tmpView;
	int maxMessage = 0;
	int maxLight = 0;
	int maxProceed = 0;
	for(int x = 0; x < transferActions.count(); x++){
		tmpView = new AMBeamlineActionItemView(transferActions.at(x), this);
		tmpView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		itemViews_.append(tmpView);
		vl_->addWidget(tmpView);
		if(tmpView->messageHint() > maxMessage)
			maxMessage = tmpView->messageHint();
		if(tmpView->lightHint() > maxLight)
			maxLight = tmpView->lightHint();
		if(tmpView->proceedHint() > maxProceed)
			maxProceed = tmpView->proceedHint();
	}
	for(int x = 0; x < itemViews_.count(); x++){
		itemViews_.at(x)->fixMessageSize(maxMessage);
		itemViews_.at(x)->fixLightSize(maxLight);
		itemViews_.at(x)->fixProceedSize(maxProceed);
	}
	mainLayout_->addLayout(vl_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	setLayout(mainLayout_);
}
