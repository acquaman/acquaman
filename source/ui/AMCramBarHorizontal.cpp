#include "AMCramBarHorizontal.h"

AMCramBarHorizontal::AMCramBarHorizontal(QWidget *parent) :
    QWidget(parent)
{
	outerLayout_ = new QHBoxLayout();
	outerLayout_->setSpacing(0);
	outerLayout_->setContentsMargins(0,0,0,0);

	scrollLeftButton_ = new QToolButton();
	scrollLeftButton_->setArrowType(Qt::LeftArrow);
	scrollLeftButton_->setAutoRepeat(true);
	scrollRightButton_ = new QToolButton();
	scrollRightButton_->setArrowType(Qt::RightArrow);
	scrollRightButton_->setAutoRepeat(true);

	outerLayout_->addWidget(scrollLeftButton_);

	scrollArea_ = new AMScrollArea();
	scrollWidget_ = new AMSizeSignallingWidget();
	scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	innerLayout_ = new QHBoxLayout();
	innerLayout_->setSpacing(0);
	innerLayout_->setContentsMargins(0,0,0,0);
	innerLayout_->setSizeConstraint(QLayout::SetMinAndMaxSize);
	//unnecessary: innerLayout_->setSizeConstraint(QLayout::SetFixedSize);
	scrollWidget_->setLayout(innerLayout_);
	//unnecessary: scrollWidget_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	scrollArea_->setFrameStyle(QFrame::NoFrame);
	scrollArea_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);
	outerLayout_->addWidget(scrollArea_);
	outerLayout_->addWidget(scrollRightButton_);

	scrollArea_->setWidget(scrollWidget_);

	setLayout(outerLayout_);

	connect(scrollWidget_, SIGNAL(resized(QSize)), this, SLOT(onScrollWidgetResized(QSize)));
	connect(scrollArea_, SIGNAL(resized(QSize)), this, SLOT(onScrollAreaResized(QSize)));

	scrollLeftButton_->hide();
	scrollRightButton_->hide();

	connect(scrollLeftButton_, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));
	connect(scrollRightButton_, SIGNAL(clicked()), this, SLOT(onScrollButtonClicked()));
}

#include <QScrollBar>

void AMCramBarHorizontal::onScrollButtonClicked() {
	if(sender() == scrollLeftButton_)
		scrollArea_->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);

	if(sender() == scrollRightButton_)
		scrollArea_->horizontalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
}


/// called when the scroll area itself is resized:
void AMCramBarHorizontal::onScrollAreaResized(const QSize&) {
	if(scrollArea_->width() >= scrollWidget_->width()) {
		scrollLeftButton_->hide();
		scrollRightButton_->hide();
	}
	else {
		scrollLeftButton_->show();
		scrollRightButton_->show();
	}
}
