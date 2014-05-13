#include "AMDeadTimeButton.h"

#include <QPainter>
#include <QStyleOption>

AMDeadTimeButton::~AMDeadTimeButton(){}

AMDeadTimeButton::AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, QWidget *parent)
	: QToolButton(parent)
{
	inputCountSource_ = inputCountSource;
	outputCountSource_ = outputCountSource;
	goodReferencePoint_ = goodReferencePoint;
	badReferencecPoint_ = badReferencePoint;

	connect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDeadTimeUpdated()));
	connect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onDeadTimeUpdated()));
}

void AMDeadTimeButton::onDeadTimeUpdated()
{
	setToolTip(QString("%1%").arg(100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex()))), 0, 'f', 0));
	update();
}

void AMDeadTimeButton::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e)

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Main button box.  Uses the standand push button style for the basic aspects of the box.
	QStyleOptionButton option;
	option.initFrom(this);
	option.state = QStyle::State_Off;

	if (!isEnabled())
		option.palette = QPalette(Qt::black, QColor(170, 170, 170, 100), Qt::gray, Qt::gray, QColor(170, 170, 170), Qt::gray, Qt::gray, Qt::gray, QColor(170, 170, 170));

	else if (!isChecked()){

		double newValue = 100*(1 - double(outputCountSource_->value(AMnDIndex()))/double(inputCountSource_->value(AMnDIndex())));

		if (newValue < goodReferencePoint_)
			option.palette = QPalette(Qt::black, QColor(20, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::green, QColor(0, 200, 0));
		else if (newValue >= goodReferencePoint_ && newValue < badReferencecPoint_)
			option.palette = QPalette(Qt::black, QColor(220, 220, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::yellow, QColor(200, 200, 0));
		else
			option.palette = QPalette(Qt::black, QColor(220, 20, 20), Qt::gray, Qt::darkGray, QColor(170, 170, 170), Qt::black, Qt::red, Qt::red, QColor(200, 0, 0));
	}

	else
		option.palette = QPalette(Qt::black, QColor(225, 225, 225, 100), Qt::gray, Qt::gray, QColor(225, 225, 225), Qt::gray, Qt::gray, Qt::gray, QColor(225, 225, 225));

	style()->drawControl(QStyle::CE_PushButton, &option, &painter, this);
}

void AMDeadTimeButton::setGoodReferencePoint(double newReference)
{
	goodReferencePoint_ = newReference;
	update();
}

void AMDeadTimeButton::setBadReferencePoint(double newReference)
{
	badReferencecPoint_ = newReference;
	update();
}

void AMDeadTimeButton::setDeadTimeSources(AMDataSource *inputCountSource, AMDataSource *outputCountSource)
{
	disconnect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
	disconnect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
	inputCountSource_ = inputCountSource;
	outputCountSource_ = outputCountSource;
	connect(inputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
	connect(outputCountSource_->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(update()));
}
