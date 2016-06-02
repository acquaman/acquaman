#include "PGMBPMStatusView.h"
#include "beamline/PGM/PGMBeamline.h"
#include "beamline/PGM/PGMBPMControl.h"

#include <QLabel>
#include <QVBoxLayout>

PGMBPMStatusView::PGMBPMStatusView(QWidget *parent) :
    QWidget(parent)
{

    // Set widgets and layout for PGMBpmControls
	QLayout *bpmLayout = setupBPMDisplay();

    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addLayout(bpmLayout);

    setLayout(mainLayout);

	// Set connections for PGMBpmContols
	setupBPMConnections();
}

QLayout * PGMBPMStatusView::setupBPMDisplay(){

    bpm10IDvalueX_ = new QLabel("   um");
    bpm10IDvalueX_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm10IDvalueY_ = new QLabel("   um");
    bpm10IDvalueY_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID1valueX_ = new QLabel("   um");
    bpm11ID1valueX_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID1valueY_ = new QLabel("   um");
    bpm11ID1valueY_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID2valueX_ = new QLabel("   um");
    bpm11ID2valueX_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

    bpm11ID2valueY_ = new QLabel("   um");
    bpm11ID2valueY_->setFont(QFont("Lucida Grande", 10, QFont::Bold));

	QLabel *bpm10IDLabel = new QLabel("10ID DnStr");
	bpm10IDLabel->setAlignment(Qt::AlignLeft);
	QLabel *bpm11ID1Label = new QLabel("11ID #1");
	bpm11ID1Label->setAlignment(Qt::AlignLeft);
	QLabel *bpm11ID2Label = new QLabel("11ID #2");
	bpm11ID2Label->setAlignment(Qt::AlignLeft);

	QLabel *bpmXValueLabel = new QLabel("X: ");
	bpmXValueLabel->setAlignment(Qt::AlignCenter);
	QLabel *bpmYValueLabel = new QLabel("Y: ");
	bpmYValueLabel->setAlignment(Qt::AlignCenter);

	QGridLayout *bpmLayout = new QGridLayout();
	bpmLayout->addWidget(bpm10IDLabel,   0, 1, 1, 1);
	bpmLayout->addWidget(bpm11ID1Label,  0, 2, 1, 1);
	bpmLayout->addWidget(bpm11ID2Label,  0, 3, 1, 1);
	bpmLayout->addWidget(bpmXValueLabel, 1, 0, 1, 1);
	bpmLayout->addWidget(bpmYValueLabel, 2, 0, 1, 1);
	bpmLayout->addWidget(bpm10IDvalueX_,  1, 1, 1, 1);
	bpmLayout->addWidget(bpm10IDvalueY_,  2, 1, 1, 1);
	bpmLayout->addWidget(bpm11ID1valueX_, 1, 2, 1, 1);
	bpmLayout->addWidget(bpm11ID1valueY_, 2, 2, 1, 1);
	bpmLayout->addWidget(bpm11ID2valueX_, 1, 3, 1, 1);
	bpmLayout->addWidget(bpm11ID2valueY_, 2, 3, 1, 1);

	bpmLayout->setColumnMinimumWidth(1, 100);
	bpmLayout->setColumnMinimumWidth(2, 100);
	bpmLayout->setColumnMinimumWidth(3, 100);

	return bpmLayout;
}

void PGMBPMStatusView::setupBPMConnections(){
    // BPM 10ID-x connections
    connect(PGMBeamline::pgm()->bpm10IDxControl(), SIGNAL(onBPMValueChanged(QString)), bpm10IDvalueX_, SLOT(setText(QString)));
	connect(PGMBeamline::pgm()->bpm10IDxControl(), SIGNAL(validityStateChanged(bool)), this, SLOT(onBPM10IDxStatusChanged(bool)));
	// BPM 10ID-y connections
    connect(PGMBeamline::pgm()->bpm10IDyControl(), SIGNAL(onBPMValueChanged(QString)), bpm10IDvalueY_, SLOT(setText(QString)));
	connect(PGMBeamline::pgm()->bpm10IDyControl(), SIGNAL(validityStateChanged(bool)), this, SLOT(onBPM10IDyStatusChanged(bool)));
	// BPM 11ID-1-x connections
    connect(PGMBeamline::pgm()->bpm11ID1xControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID1valueX_, SLOT(setText(QString)));
	connect(PGMBeamline::pgm()->bpm11ID1xControl(), SIGNAL(validityStateChanged(bool)), this, SLOT(onBPM11ID1xStatusChanged(bool)));
	// BPM 11ID-1-y connections
    connect(PGMBeamline::pgm()->bpm11ID1yControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID1valueY_, SLOT(setText(QString)));
	connect(PGMBeamline::pgm()->bpm11ID1yControl(), SIGNAL(validityStateChanged(bool)), this, SLOT(onBPM11ID1yStatusChanged(bool)));
	// BPM 11ID-2-x connections
    connect(PGMBeamline::pgm()->bpm11ID2xControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID2valueX_, SLOT(setText(QString)));
	connect(PGMBeamline::pgm()->bpm11ID2xControl(), SIGNAL(validityStateChanged(bool)), this, SLOT(onBPM11ID2xStatusChanged(bool)));
	// BPM 11ID-2-y connections
    connect(PGMBeamline::pgm()->bpm11ID2yControl(), SIGNAL(onBPMValueChanged(QString)), bpm11ID2valueY_, SLOT(setText(QString)));
	connect(PGMBeamline::pgm()->bpm11ID2yControl(), SIGNAL(validityStateChanged(bool)), this, SLOT(onBPM11ID2yStatusChanged(bool)));

	if (PGMBeamline::pgm()->bpm10IDxControl()->isConnected()) {
		bpm10IDvalueX_->setText(PGMBeamline::pgm()->bpm10IDxControl()->valueStr());
		onBPM10IDxStatusChanged(PGMBeamline::pgm()->bpm10IDxControl()->isValid());
	}
	if (PGMBeamline::pgm()->bpm10IDyControl()->isConnected()) {
		bpm10IDvalueY_->setText(PGMBeamline::pgm()->bpm10IDyControl()->valueStr());
		onBPM10IDyStatusChanged(PGMBeamline::pgm()->bpm10IDyControl()->isValid());
	}
	if (PGMBeamline::pgm()->bpm11ID1xControl()->isConnected()) {
		bpm11ID1valueX_->setText(PGMBeamline::pgm()->bpm11ID1xControl()->valueStr());
		onBPM11ID1xStatusChanged(PGMBeamline::pgm()->bpm11ID1xControl()->isValid());
	}
	if (PGMBeamline::pgm()->bpm11ID1yControl()->isConnected()) {
		bpm11ID1valueY_->setText(PGMBeamline::pgm()->bpm11ID1yControl()->valueStr());
		onBPM11ID1yStatusChanged(PGMBeamline::pgm()->bpm11ID1yControl()->isValid());
	}
	if (PGMBeamline::pgm()->bpm11ID2xControl()->isConnected()) {
		bpm11ID2valueX_->setText(PGMBeamline::pgm()->bpm11ID2xControl()->valueStr());
		onBPM11ID2xStatusChanged(PGMBeamline::pgm()->bpm11ID2xControl()->isValid());
	}
	if (PGMBeamline::pgm()->bpm11ID2yControl()->isConnected()) {
		bpm11ID2valueY_->setText(PGMBeamline::pgm()->bpm11ID2yControl()->valueStr());
		onBPM11ID2yStatusChanged(PGMBeamline::pgm()->bpm11ID2yControl()->isValid());
	}
}


void PGMBPMStatusView::onBPM10IDxStatusChanged(bool state)
{
	updateBPMStatusLabelStatusStyle(bpm10IDvalueX_, state);
}

void PGMBPMStatusView::onBPM10IDyStatusChanged(bool state)
{
	updateBPMStatusLabelStatusStyle(bpm10IDvalueY_, state);
}


void PGMBPMStatusView::onBPM11ID1xStatusChanged(bool state)
{
	updateBPMStatusLabelStatusStyle(bpm11ID1valueX_, state);
}

void PGMBPMStatusView::onBPM11ID1yStatusChanged(bool state)
{
	updateBPMStatusLabelStatusStyle(bpm11ID1valueY_, state);
}

void PGMBPMStatusView::onBPM11ID2xStatusChanged(bool state)
{
	updateBPMStatusLabelStatusStyle(bpm11ID2valueX_, state);
}

void PGMBPMStatusView::onBPM11ID2yStatusChanged(bool state)
{
	updateBPMStatusLabelStatusStyle(bpm11ID2valueY_, state);
}

void PGMBPMStatusView::updateBPMStatusLabelStatusStyle(QLabel *bpmLabel, bool state)
{
	if(state){
		 bpmLabel->setStyleSheet( "color : black"  );
	} else {
		 bpmLabel->setStyleSheet( "color : red"  );
	}
}
