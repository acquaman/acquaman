#include "AMScanViewPlotToolView.h"

AMDataPositionToolView::AMDataPositionToolView(MPlotDataPositionTool *tool, QWidget *parent, bool view2D) :
	QWidget(parent)
{
	// Initialize member variables.

	tool_ = 0;
	view2D_ = false;

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

	setView2D(view2D);
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
			connect( tool_, SIGNAL(positionChanged(QPointF)), this, SLOT(update()) );
			connect( tool_, SIGNAL(unitsChanged(QStringList)), this, SLOT(update()) );
		}

		emit toolChanged(tool_);
	}

	refresh();
}

void AMDataPositionToolView::setView2D(bool use2D)
{
	if (view2D_ != use2D) {
		view2D_ = use2D;
		emit view2DChanged(view2D_);

		refresh();
	}
}

void AMDataPositionToolView::clear()
{
	value_->clear();
}

void AMDataPositionToolView::update()
{
	if (tool_) {
		QPointF newPosition = tool_->currentPosition();
		QStringList newUnits = tool_->units();

		QString toDisplay;

		if (view2D_) {
			toDisplay = positionToString(newPosition, newUnits);

		} else {
			QString units;

			if (!newUnits.isEmpty())
				units = newUnits.first();

			toDisplay = positionToString(newPosition.x(), units);
		}

		value_->setText(toDisplay);
	}
}

void AMDataPositionToolView::refresh()
{
	clear();
	update();
}

QString AMDataPositionToolView::positionToString(double value, const QString &units)
{
	QString text = QString::number(value, 'f', 2);
	if (!units.isEmpty())
		text += " " + units;

	return text;
}

QString AMDataPositionToolView::positionToString(const QPointF &values, const QStringList &units)
{
	QString text;

	QString x = QString::number(values.x(), 'f', 2);
	if (units.size() > 0)
		x += " " + units.at(0);

	QString y = QString::number(values.y(), 'f', 2);
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

	QLabel *positionPrompt = new QLabel("Value: ");

	positionLabel_ = new QLabel();

	positionSpinBox_ = new QDoubleSpinBox();
	positionSpinBox_->setMinimum(-1000000);
	positionSpinBox_->setMaximum(1000000);
	positionSpinBox_->setValue(0);

	markerComboBox_ = new AMPlotMarkerComboBox(QList<MPlotMarkerShape::Shape>() << MPlotMarkerShape::VerticalBeam << MPlotMarkerShape::HorizontalBeam << MPlotMarkerShape::Cross);

	colorButton_ = new AMColorPickerButton(QColor());

	visibilityCheckBox_ = new QCheckBox("Show cursor");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(positionPrompt);
	layout->addWidget(positionLabel_);
	layout->addWidget(positionSpinBox_);
	layout->addWidget(markerComboBox_);
	layout->addWidget(colorButton_);
	layout->addWidget(visibilityCheckBox_);

	setLayout(layout);

	// Initial settings.

	positionSpinBox_->hide();
	markerComboBox_->hide();
	colorButton_->hide();
	visibilityCheckBox_->setChecked(false);

	// Make connections.

	connect( positionSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()) );
	connect( markerComboBox_, SIGNAL(currentMarkerShapeChanged(MPlotMarkerShape::Shape)), this, SLOT(onMarkerChanged()) );
	connect( colorButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged()) );
	connect( visibilityCheckBox_, SIGNAL(clicked()), this, SLOT(onVisibilityChanged()) );

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

		if (tool_) {
			connect( tool_, SIGNAL(cursorPositionChanged(QPointF)), this, SLOT(updatePositionLabel()) );
			connect( tool_, SIGNAL(cursorPositionChanged(QPointF)), this, SLOT(updatePositionSpinBox()) );
			connect( tool_, SIGNAL(unitsChanged(QStringList)), this, SLOT(updatePositionLabel()) );
			connect( tool_, SIGNAL(unitsChanged(QStringList)), this, SLOT(updatePositionSpinBox()) );
			connect( tool_, SIGNAL(cursorMarkerChanged(MPlotMarkerShape::Shape)), this, SLOT(updateMarkerComboBox()) );
			connect( tool_, SIGNAL(cursorColorChanged(QColor)), this, SLOT(updateColorButton()) );
			connect( tool_, SIGNAL(cursorVisibilityChanged(bool)), this, SLOT(updateVisibility()) );
		}

		emit toolChanged(tool_);
	}

	refresh();
}

void AMDataPositionCursorToolView::clear()
{
	positionSpinBox_->blockSignals(true);
	markerComboBox_->blockSignals(true);
	colorButton_->blockSignals(true);
	visibilityCheckBox_->blockSignals(true);

	positionLabel_->clear();
	markerComboBox_->setSelectedMarkerShape(MPlotMarkerShape::None);
	positionSpinBox_->setValue(0);
	colorButton_->setColor(QColor());
	visibilityCheckBox_->setChecked(false);

	positionSpinBox_->blockSignals(false);
	markerComboBox_->blockSignals(false);
	colorButton_->blockSignals(false);
	visibilityCheckBox_->blockSignals(false);
}

void AMDataPositionCursorToolView::update()
{
	updatePositionLabel();
	updatePositionSpinBox();
	updateMarkerComboBox();
	updateColorButton();
	updateVisibility();
}

void AMDataPositionCursorToolView::refresh()
{
	clear();
	update();
}

void AMDataPositionCursorToolView::onPositionChanged()
{
	if (tool_)
		tool_->setDataPositionX(positionSpinBox_->value());
}

void AMDataPositionCursorToolView::onMarkerChanged()
{
	if (tool_)
		tool_->setCursorMarker(markerComboBox_->selectedMarkerShape());
}

void AMDataPositionCursorToolView::onColorChanged()
{
	if (tool_) {
		tool_->setCursorColor(colorButton_->color());
	}
}

void AMDataPositionCursorToolView::onVisibilityChanged()
{
	if (tool_) {
		tool_->setCursorVisibility(visibilityCheckBox_->isChecked());
	}
}

void AMDataPositionCursorToolView::updatePositionLabel()
{
	QString toDisplay;

	if (tool_) {
		QPointF value;
		QString units;
		QStringList unitsList;

		value = tool_->cursorPosition();
		unitsList = tool_->units();

		if (!unitsList.isEmpty())
			units = unitsList.first();

		toDisplay = positionToString(value.x(), units);
	}

	positionLabel_->setText(toDisplay);
}

void AMDataPositionCursorToolView::updatePositionSpinBox()
{
	if (tool_) {
		QPointF cursorPosition = tool_->cursorPosition();
		QString units;
		QStringList unitsList;

		unitsList = tool_->units();

		if (!unitsList.isEmpty())
			units = unitsList.first();

		if (positionSpinBox_->value() != cursorPosition.x()) {
			positionSpinBox_->setValue(cursorPosition.x());
			positionSpinBox_->setSuffix(" " + units);
		}
	}
}

void AMDataPositionCursorToolView::updateMarkerComboBox()
{
	if (tool_) {
		markerComboBox_->setSelectedMarkerShape(tool_->cursorMarker());
	}
}

void AMDataPositionCursorToolView::updateColorButton()
{
	if (tool_) {
		colorButton_->setColor(tool_->cursorColor());
	}
}

void AMDataPositionCursorToolView::updateVisibility()
{
	if (tool_) {
		bool cursorVisible = tool_->cursorVisible();

		if (cursorVisible) {
			positionLabel_->hide();
			positionSpinBox_->show();
			markerComboBox_->show();
			colorButton_->show();

		} else {
			positionLabel_->show();
			positionSpinBox_->hide();
			markerComboBox_->hide();
			colorButton_->hide();
		}

		visibilityCheckBox_->setChecked(cursorVisible);
	}
}

QString AMDataPositionCursorToolView::positionToString(double value, const QString &units)
{
	QString text = QString::number(value, 'f', 2);
	if (!units.isEmpty())
		text += " " + units;

	return text;
}

QString AMDataPositionCursorToolView::positionToString(const QPointF &values, const QStringList &units)
{
	QString text;

	QString x = QString::number(values.x(), 'f', 2);
	if (units.size() > 0)
		x += " " + units.at(0);

	QString y = QString::number(values.y(), 'f', 2);
	if (units.size() > 1)
		y += " " + units.at(1);

	text = "(" + x + ", " + y + ")";

	return text;
}
