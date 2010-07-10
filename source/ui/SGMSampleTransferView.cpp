#include "SGMSampleTransferView.h"

SGMSampleTransferView::SGMSampleTransferView(QWidget *parent) :
	QWidget(parent)
{
	vl_ = new QVBoxLayout();
	mainLayout_ = new QGridLayout();
	QList<AMBeamlineActionItem*> transferActions = SGMBeamline::sgm()->transferActions();
	AMBeamlineActionItemView *tmpView;
	for(int x = 0; x < transferActions.count(); x++){
		tmpView = new AMBeamlineActionItemView(transferActions.at(x), this);
		tmpView->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		vl_->addWidget(tmpView);
	}
	mainLayout_->addLayout(vl_, 0, 0, 1, 1, Qt::AlignLeft|Qt::AlignTop);
	/*
	action1Label_ = new QLabel("Close the valve between the endstation and the loadlock", this);
	action2Label_ = new QLabel("Turn off the CCG", this);
	action3Label_ = new QLabel("Close the roughing pump valve", this);
	action4Label_ = new QLabel("Turn off the turbo pump power", this);
	action1Proceed_ = new QPushButton("Proceed", this);
//	action1Proceed_->setEnabled(false);
	action2Proceed_ = new QPushButton("Proceed", this);
	action2Proceed_->setEnabled(false);
	action3Proceed_ = new QPushButton("Proceed", this);
	action3Proceed_->setEnabled(false);
	action4Proceed_ = new QPushButton("Proceed", this);
	action4Proceed_->setEnabled(false);
	action1Light_ = new QPushButton("", this);
	action2Light_ = new QPushButton("", this);
	action3Light_ = new QPushButton("", this);
	action4Light_ = new QPushButton("", this);
	action1Light_->setObjectName("action1Light_");
	action2Light_->setObjectName("action2Light_");
	action3Light_->setObjectName("action3Light_");
	action4Light_->setObjectName("action4Light_");
	QString lightStopStyle = "QPushButton { background: red; border: 1px solid red; }";
	//setStyleSheet("QPushButton#action1Light_, QPushButton#action2Light_, QPushButton#action3Light_, QPushButton#action4Light_"+lightStopStyle);
	action1Light_->setStyleSheet(lightStopStyle);
	action2Light_->setStyleSheet(lightStopStyle);
	action3Light_->setStyleSheet(lightStopStyle);
	action4Light_->setStyleSheet(lightStopStyle);
	QList<AMBeamlineActionItem*> transferActions = SGMBeamline::sgm()->transferActions();
	connect(action1Proceed_, SIGNAL(clicked()), transferActions.at(0), SIGNAL(succeeded()));

	connect(transferActions.at(0), SIGNAL(succeeded()), this, SLOT(setAction1Success()));
	connect(transferActions.at(1), SIGNAL(succeeded()), this, SLOT(setAction2Success()));
	connect(transferActions.at(2), SIGNAL(succeeded()), this, SLOT(setAction3Success()));
	connect(transferActions.at(3), SIGNAL(succeeded()), this, SLOT(setAction4Success()));
	connect(transferActions.at(1), SIGNAL(ready(bool)), action2Proceed_, SLOT(setEnabled(bool)));
	connect(transferActions.at(2), SIGNAL(ready(bool)), action3Proceed_, SLOT(setEnabled(bool)));
	connect(transferActions.at(3), SIGNAL(ready(bool)), action4Proceed_, SLOT(setEnabled(bool)));
	mainLayout_->addWidget(action1Label_, 0, 0, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action2Label_, 1, 0, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action3Label_, 2, 0, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action4Label_, 3, 0, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action1Light_, 0, 1, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action2Light_, 1, 1, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action3Light_, 2, 1, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action4Light_, 3, 1, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action1Proceed_, 0, 2, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action2Proceed_, 1, 2, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action3Proceed_, 2, 2, 1, 1, Qt::AlignHCenter);
	mainLayout_->addWidget(action4Proceed_, 3, 2, 1, 1, Qt::AlignHCenter);
	*/
	setLayout(mainLayout_);
}

void SGMSampleTransferView::setAction1Success(){
//	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
//	action1Light_->setStyleSheet(lightGoStyle);
}

void SGMSampleTransferView::setAction2Success(){
//	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
//	action2Light_->setStyleSheet(lightGoStyle);
}

void SGMSampleTransferView::setAction3Success(){
//	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
//	action3Light_->setStyleSheet(lightGoStyle);
}

void SGMSampleTransferView::setAction4Success(){
//	QString lightGoStyle = "QPushButton { background: green; border: 1px solid green; }";
//	action4Light_->setStyleSheet(lightGoStyle);
}
