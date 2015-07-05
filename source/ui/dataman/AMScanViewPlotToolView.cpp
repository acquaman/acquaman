#include "AMScanViewPlotToolView.h"

#include <QDebug>

AMDataPositionToolView::AMDataPositionToolView(MPlotDataPositionTool *tool, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	tool_ = 0;

	// Create UI elements.

	QLabel *valuePrompt = new QLabel("Value:");
	value_ = new QLabel();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(valuePrompt);
	layout->addWidget(value_);

	setLayout(layout);

	// Current settings.

	setTool(tool);
}

AMDataPositionToolView::~AMDataPositionToolView()
{

}

void AMDataPositionToolView::setTool(MPlotDataPositionTool *newTool)
{
	if (tool_ != newTool) {

		if (tool_) {
			disconnect( tool_, 0, this, 0 );
		}

		tool_ = newTool;

		if (tool_) {
			connect( tool_, SIGNAL(positionChanged(QPointF)), this, SLOT(onToolPositionChanged()) );
		}

		emit toolChanged(tool_);
	}

	refresh();
}

void AMDataPositionToolView::clear()
{
	value_->clear();
}

void AMDataPositionToolView::update()
{
	onToolPositionChanged();
}

void AMDataPositionToolView::refresh()
{
	clear();
	update();
}

void AMDataPositionToolView::onToolPositionChanged()
{
	qDebug() << "\nAttempting to update the data position view...";

	if (tool_) {
		qDebug() << "Updating the data position view...";
		value_->setText( positionToString(tool_->currentPosition(), QStringList()) );
		qDebug() << "View update complete.\n";
	}
}

QString AMDataPositionToolView::positionToString(const QPointF &values, const QStringList &units)
{
	QString text;

	QString x = QString::number(values.x(), 'f', 3);
	if (units.size() > 0)
		x += " " + units.at(0);

	QString y = QString::number(values.y(), 'f', 3);
	if (units.size() > 1)
		y += " " + units.at(1);

	text = "(" + x + ", " + y + ")";

	return text;
}









AMDataPositionCursorToolView::AMDataPositionCursorToolView(MPlotDataPositionCursorTool *tool, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	tool_ = 0;

	// Create UI elements.

	positionView_ = new AMDataPositionToolView(0);

	visibilityCheckBox_ = new QCheckBox("Show cursor");

	positionSpinBox_ = new QDoubleSpinBox();
	positionSpinBox_->setMinimum(-1000000);
	positionSpinBox_->setMaximum(1000000);
	positionSpinBox_->setValue(0);

	colorButton_ = new AMColorPickerButton(QColor());

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(positionView_);
	layout->addSpacing(5);
	layout->addWidget(visibilityCheckBox_);
	layout->addWidget(positionSpinBox_);
	layout->addWidget(colorButton_);

	setLayout(layout);

	// Make connections.

	connect( visibilityCheckBox_, SIGNAL(clicked()), this, SLOT(onVisibilityChanged()) );
	connect( positionSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()) );
	connect( colorButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged()) );

	// Current settings.

	setTool(tool);
}

AMDataPositionCursorToolView::~AMDataPositionCursorToolView()
{

}

void AMDataPositionCursorToolView::setTool(MPlotDataPositionCursorTool *newTool)
{
	if (tool_ != newTool) {

		if (tool_) {
			disconnect( tool_, 0, this, 0 );
		}

		tool_ = newTool;

		positionView_->setTool(tool_);

		if (tool_) {
			connect( tool_, SIGNAL(positionChanged(QPointF)), this, SLOT(onToolPositionChanged()) );
			connect( tool_, SIGNAL(cursorVisibilityChanged(bool)), this, SLOT(onToolVisibilityChanged()) );
			connect( tool_, SIGNAL(cursorColorChanged(QColor)), this, SLOT(onToolColorChanged()) );
		}

		emit toolChanged(tool_);
	}

	refresh();
}

void AMDataPositionCursorToolView::clear()
{
	visibilityCheckBox_->setChecked(false);
	positionSpinBox_->setValue(0);
	colorButton_->setColor(QColor());

	positionView_->clear();
}

void AMDataPositionCursorToolView::update()
{
	if (tool_) {
		onToolVisibilityChanged();
		onToolPositionChanged();
		onToolColorChanged();

		positionView_->update();
	}
}

void AMDataPositionCursorToolView::refresh()
{
	clear();
	update();
}

void AMDataPositionCursorToolView::onVisibilityChanged()
{
	if (tool_) {
		tool_->setCursorVisibility(visibilityCheckBox_->isChecked());
	}
}

void AMDataPositionCursorToolView::onPositionChanged()
{
	if (tool_) {
		qDebug() << "\nSpin box position changed.";

		QPointF oldPosition = tool_->currentPosition();
		QPointF newPosition = QPointF(positionSpinBox_->value(), oldPosition.y());

		qDebug() << "About to set the tool position to match spinbox position...";

		tool_->setDataPosition(newPosition);
	}
}

void AMDataPositionCursorToolView::onColorChanged()
{
	if (tool_) {
		tool_->setCursorColor(colorButton_->color());
	}
}

void AMDataPositionCursorToolView::onToolVisibilityChanged()
{
	if (tool_) {
		visibilityCheckBox_->setChecked(tool_->cursorVisible());

		if (tool_->cursorVisible()) {
			positionSpinBox_->show();
			colorButton_->show();
		} else {
			positionSpinBox_->hide();
			colorButton_->hide();
		}
	}
}

void AMDataPositionCursorToolView::onToolColorChanged()
{
	if (tool_) {
		colorButton_->setColor(tool_->cursorColor());
	}
}

void AMDataPositionCursorToolView::onToolPositionChanged()
{
	if (tool_) {
		QPointF cursorPosition = tool_->currentPosition();

		if (positionSpinBox_->value() != cursorPosition.x()) {
			qDebug() << "\nCursor tool position changed. Updating spinbox:" << cursorPosition.x();
			positionSpinBox_->setValue(cursorPosition.x());
		}
	}
}
