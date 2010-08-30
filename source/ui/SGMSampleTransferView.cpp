#include "SGMSampleTransferView.h"

SGMSampleTransferView::SGMSampleTransferView(QWidget *parent) :
	QWidget(parent)
{
	mainLayout_ = new QStackedLayout();

	QPushButton* tmpButton;
	tmpButton = new QPushButton("Transfer sample out of loadlock");
	transferButtons_.append(tmpButton);
	tmpButton = new QPushButton("Transfer sample into loadlock");
	transferButtons_.append(tmpButton);
	tmpButton = new QPushButton("Transfer sample out of chamber");
	transferButtons_.append(tmpButton);
	tmpButton = new QPushButton("Transfer sample into chamber");
	transferButtons_.append(tmpButton);

	transferBox_ = new SGMSampleTransferProceduresView("Transfer Procedures", transferButtons_);
	SGMSampleTransferPaneView* tmpPane;
	tmpPane = new SGMSampleTransferPaneView(SGMBeamline::sgm()->transferLoadlockOutActions(), "Transfer Out of Loadlock");
	transferPanes_.append(tmpPane);
	tmpPane = new SGMSampleTransferPaneView(SGMBeamline::sgm()->transferLoadlockInActions(), "Transfer Into Loadlock");
	transferPanes_.append(tmpPane);
	tmpPane = new SGMSampleTransferPaneView(SGMBeamline::sgm()->transferChamberOutActions(), "Transfer Out of Chamber");
	transferPanes_.append(tmpPane);
	tmpPane = new SGMSampleTransferPaneView(SGMBeamline::sgm()->transferChamberInActions(), "Transfer Into Chamber");
	transferPanes_.append(tmpPane);

	mainLayout_->addWidget(transferBox_);
	for(int x = 0; x < transferPanes_.count(); x++){
		mainLayout_->addWidget(transferPanes_.at(x));
		connect(transferPanes_.at(x), SIGNAL(completed()), this, SLOT(drawMain()));
		switch(x){
		case 0:
			connect(transferButtons_.at(x), SIGNAL(clicked()), this, SLOT(drawIndex1()));
			break;
		case 1:
			connect(transferButtons_.at(x), SIGNAL(clicked()), this, SLOT(drawIndex2()));
			break;
		case 2:
			connect(transferButtons_.at(x), SIGNAL(clicked()), this, SLOT(drawIndex3()));
			break;
		case 3:
			connect(transferButtons_.at(x), SIGNAL(clicked()), this, SLOT(drawIndex4()));
			break;
		}
	}
	setLayout(mainLayout_);
}

void SGMSampleTransferView::drawMain(){
	mainLayout_->setCurrentIndex(0);
}

void SGMSampleTransferView::drawIndex1(){
	mainLayout_->setCurrentIndex(1);
	transferPanes_.at(0)->startPane();
}

void SGMSampleTransferView::drawIndex2(){
	mainLayout_->setCurrentIndex(2);
	transferPanes_.at(1)->startPane();
}

void SGMSampleTransferView::drawIndex3(){
	mainLayout_->setCurrentIndex(3);
	transferPanes_.at(2)->startPane();
}

void SGMSampleTransferView::drawIndex4(){
	mainLayout_->setCurrentIndex(4);
	transferPanes_.at(3)->startPane();
}

//void SGMSampleTransferView::drawLoadlockOut(){
//	mainLayout_->setCurrentIndex(1);
//	loadlockOut_->startPane();
//}

//void SGMSampleTransferView::drawLoadlockIn(){
//	mainLayout_->setCurrentIndex(2);
//	loadlockIn_->startPane();
//}


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

SGMSampleTransferPaneView::SGMSampleTransferPaneView(QList<AM1BeamlineActionItem*> items, const QString &title, QWidget *parent) :
		QGroupBox(title, parent)
{
	vl_ = new QVBoxLayout();
	mainLayout_ = new QGridLayout();
	QList<AM1BeamlineActionItem*> transferActions = items;
	connect(transferActions.at(transferActions.count()-1), SIGNAL(succeeded()), this, SLOT(prepareCompletion()));
	AMBeamlineActionItemView *tmpView;
	int maxMessage = 0;
	int maxLight = 0;
	int maxProceed = 0;
	firstItem_ = transferActions.at(0);
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
		connect(this, SIGNAL(completed()), itemViews_.at(x), SLOT(initializeView()));
	}
	for(int x = 0; x < itemViews_.count(); x++){
		itemViews_.at(x)->fixMessageSize(maxMessage);
		itemViews_.at(x)->fixLightSize(maxLight);
		itemViews_.at(x)->fixProceedSize(maxProceed);
	}
	completeLabel_ = new QLabel("");
	initialize();
	completeButton_ = new QPushButton("Return to Main Transfer Menu");
	connect(completeButton_, SIGNAL(clicked()), this, SIGNAL(completed()));
	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(completeLabel_);
	hl->addWidget(completeButton_);
	vl_->addLayout(hl);
	mainLayout_->addLayout(vl_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	setLayout(mainLayout_);
}

void SGMSampleTransferPaneView::initialize(){
	completeLabel_->setText("Procedure Incomplete");
	for(int x = 0; x < itemViews_.count(); x++)
		itemViews_.at(x)->initializeView();
}

void SGMSampleTransferPaneView::prepareCompletion(){
	completeLabel_->setText("Procedure Finished");
}
