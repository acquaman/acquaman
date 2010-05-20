#include "AMScanView.h"
#include <QGraphicsWidget>



AMScanViewModeBar::AMScanViewModeBar(QWidget* parent)
	: QWidget(parent)
{
	QHBoxLayout* hl = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout(), *hl3 = new QHBoxLayout();
	hl2->setSpacing(0);
	hl3->setSpacing(0);

	tabButton_ = new QToolButton();
	tabButton_->setText("1");
	overplotButton_ = new QToolButton();
	overplotButton_->setText("OP");
	multiScansButton_ = new QToolButton();
	multiScansButton_->setText("M-S");
	multiChannelsButton_ = new QToolButton();
	multiChannelsButton_->setText("M-C");

	tabButton_->setCheckable(true);
	overplotButton_->setCheckable(true);
	multiScansButton_->setCheckable(true);
	multiChannelsButton_->setCheckable(true);

	QButtonGroup* bg = new QButtonGroup(this);
	bg->addButton(tabButton_);
	bg->addButton(overplotButton_);
	bg->addButton(multiScansButton_);
	bg->addButton(multiChannelsButton_);
	tabButton_->setChecked(true);

	hl2->addWidget(tabButton_);
	hl2->addWidget(overplotButton_);
	hl2->addWidget(multiScansButton_);
	hl2->addWidget(multiChannelsButton_);

	hl->addLayout(hl2);
	hl->addStretch(1);

	plusButton_ = new QToolButton();
	plusButton_->setText("+");
	hl3->addWidget(plusButton_);
	subtractButton_ = new QToolButton();
	subtractButton_->setText("-");
	hl3->addWidget(subtractButton_);

	hl->addLayout(hl3);
	hl->setMargin(6);
	hl->setSpacing(24);
	setLayout(hl);

	setObjectName("AMScanViewModeBar");
	setStyleSheet("QWidget#AMScanViewModeBar { "
		"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(81, 81, 81, 255), stop:0.494444 rgba(81, 81, 81, 255), stop:0.5 rgba(64, 64, 64, 255), stop:1 rgba(64, 64, 64, 255));"
		"border-bottom: 1px solid black;"
		"}");



}

AMScanView::AMScanView(QWidget *parent) :
    QWidget(parent)
{
	setupUI();
}


void AMScanView::setupUI() {
	QVBoxLayout* vl = new QVBoxLayout();

	AMScanViewMainWidget* mw = new AMScanViewMainWidget();

	vl->addWidget(mw);

	barLayout_ = new QVBoxLayout();
	vl->addLayout(barLayout_);

	modeBar_ = new AMScanViewModeBar();
	vl->addWidget(modeBar_);

	setLayout(vl);


	// setup grid layout within main plot area:

	glayout_ = new QGraphicsGridLayout();
	mw->graphicsWidget()->setLayout(glayout_);

	plots_ << new MPlotGW();
	plots_[0]->plot()->setScalePadding(5);
	plots_[0]->plot()->enableAutoScale(MPlotAxis::Left | MPlotAxis::Bottom);

	glayout_->addItem(plots_[0], 0, 0, Qt::AlignCenter);
}
