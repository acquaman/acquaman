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

	xPositionSpinBox_ = new QDoubleSpinBox();

	yPositionSpinBox_ = new QDoubleSpinBox();

	markerComboBox_ = new AMPlotMarkerComboBox();

	colorButton_ = new AMColorPickerButton(QColor());

	cursorVisibilityCheckBox_ = new QCheckBox("Show cursor");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(positionPrompt);
	layout->addWidget(positionLabel_);
	layout->addWidget(xPositionSpinBox_);
	layout->addWidget(yPositionSpinBox_);
	layout->addWidget(markerComboBox_);
	layout->addWidget(colorButton_);
	layout->addWidget(cursorVisibilityCheckBox_);

	setLayout(layout);

	// Initial settings.

	refresh();

	// Make connections.

	connect( xPositionSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()) );
	connect( yPositionSpinBox_, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()) );
	connect( markerComboBox_, SIGNAL(currentMarkerShapeChanged(MPlotMarkerShape::Shape)), this, SLOT(onMarkerChanged()) );
	connect( colorButton_, SIGNAL(colorChanged(QColor)), this, SLOT(onColorChanged()) );
	connect( cursorVisibilityCheckBox_, SIGNAL(clicked()), this, SLOT(onCursorVisibilityChanged()) );

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
			connect( tool_, SIGNAL(cursorPositionChanged(QPointF)), this, SLOT(update()) );
			connect( tool_, SIGNAL(unitsChanged(QStringList)), this, SLOT(update()) );
			connect( tool_, SIGNAL(cursorMarkerChanged(MPlotMarkerShape::Shape)), this, SLOT(update()) );
			connect( tool_, SIGNAL(cursorColorChanged(QColor)), this, SLOT(update()) );
			connect( tool_, SIGNAL(cursorVisibilityChanged(bool)), this, SLOT(update()) );
		}

		emit toolChanged(tool_);
	}

	refresh();
}

void AMDataPositionCursorToolView::clear()
{
	xPositionSpinBox_->blockSignals(true);
	yPositionSpinBox_->blockSignals(true);
	markerComboBox_->blockSignals(true);
	colorButton_->blockSignals(true);
	cursorVisibilityCheckBox_->blockSignals(true);

	positionLabel_->clear();
	markerComboBox_->clear();
	xPositionSpinBox_->clear();
	yPositionSpinBox_->clear();
	colorButton_->setColor(QColor());
	cursorVisibilityCheckBox_->setChecked(false);

	xPositionSpinBox_->blockSignals(false);
	yPositionSpinBox_->blockSignals(false);
	markerComboBox_->blockSignals(false);
	colorButton_->blockSignals(false);
	cursorVisibilityCheckBox_->blockSignals(false);
}

void AMDataPositionCursorToolView::update()
{
	updatePositionLabel();
	updatePositionSpinBoxes();
	updateMarkerComboBox();
	updateColorButton();
	updateVisibilityCheckBox();
}

void AMDataPositionCursorToolView::refresh()
{
	clear();

	// Set up the position spinboxes.

	xPositionSpinBox_->setMinimum(-1000000);
	xPositionSpinBox_->setMaximum(1000000);
	xPositionSpinBox_->setValue(0);

	yPositionSpinBox_->setMinimum(-1000000);
	yPositionSpinBox_->setMaximum(1000000);
	yPositionSpinBox_->setValue(0);

	// Set the marker shape options for the marker combo box.

	markerComboBox_->addItems(QList<MPlotMarkerShape::Shape>() << MPlotMarkerShape::VerticalBeam << MPlotMarkerShape::HorizontalBeam << MPlotMarkerShape::Cross);

	update();
}

void AMDataPositionCursorToolView::onPositionChanged()
{
	QPointF newPosition = QPointF(xPositionSpinBox_->value(), yPositionSpinBox_->value());

	if (tool_)
		tool_->setCursorPosition(newPosition);
}

void AMDataPositionCursorToolView::onMarkerChanged()
{
	MPlotMarkerShape::Shape newMarker = markerComboBox_->selectedMarkerShape();

	if (tool_)
		tool_->setCursorMarker(newMarker);
}

void AMDataPositionCursorToolView::onColorChanged()
{
	QColor newColor = colorButton_->color();

	if (tool_)
		tool_->setCursorColor(newColor);
}

void AMDataPositionCursorToolView::onCursorVisibilityChanged()
{
	bool cursorVisible = cursorVisibilityCheckBox_->isChecked();

	if (tool_)
		tool_->setCursorVisibility(cursorVisible);
}

void AMDataPositionCursorToolView::updatePositionLabel()
{
	QString toDisplay;

	if (tool_) {

		// Update the text displayed.

		QPointF value = tool_->cursorPosition();
		QStringList unitsList = tool_->units();

		toDisplay = positionToString(value, unitsList);

		// Update label visibility.

		if (tool_->cursorVisible())
			positionLabel_->hide();
		else
			positionLabel_->show();
	}

	positionLabel_->setText(toDisplay);
}

void AMDataPositionCursorToolView::updatePositionSpinBoxes()
{
	if (tool_) {

		// Update the value of the position spinboxes.

		QPointF cursorPosition = tool_->cursorPosition();

		if (xPositionSpinBox_->value() != cursorPosition.x())
			xPositionSpinBox_->setValue(cursorPosition.x());

		if (yPositionSpinBox_->value() != cursorPosition.y())
			yPositionSpinBox_->setValue(cursorPosition.y());

		// Update the units displayed.

		QStringList unitsList = tool_->units();
		int unitsCount = unitsList.count();

		if (unitsCount > 0)
			xPositionSpinBox_->setSuffix(" " + unitsList.at(0));

		if (unitsCount > 1)
			yPositionSpinBox_->setSuffix(" " + unitsList.at(1));

		// Update the spinbox visibility.

		bool cursorVisible = tool_->cursorVisible();

		if (cursorVisible) {

			// If the cursor is visible, then the spinbox visibility corresponds to the
			// currently selected marker.

			MPlotMarkerShape::Shape marker = tool_->cursorMarker();

			if (marker == MPlotMarkerShape::VerticalBeam) {
				xPositionSpinBox_->show();
				yPositionSpinBox_->hide();

			} else if (marker == MPlotMarkerShape::HorizontalBeam) {
				xPositionSpinBox_->hide();
				yPositionSpinBox_->show();

			} else {
				xPositionSpinBox_->show();
				yPositionSpinBox_->show();
			}

		} else {

			// If the cursor is not visible, then both spinboxes should be hidden.

			xPositionSpinBox_->hide();
			yPositionSpinBox_->hide();
		}

	} else {

		// If there is no tool, then both spinboxes should be hidden.

		xPositionSpinBox_->hide();
		yPositionSpinBox_->hide();
	}
}

void AMDataPositionCursorToolView::updateMarkerComboBox()
{
	if (tool_) {

		// Update the marker shape.

		markerComboBox_->setSelectedMarkerShape(tool_->cursorMarker());

		// Update the combo box visibility.

		if (tool_->cursorVisible())
			markerComboBox_->show();
		else
			markerComboBox_->hide();

	} else {

		// If there is no tool, the marker combobox should be hidden.

		markerComboBox_->hide();
	}
}

void AMDataPositionCursorToolView::updateColorButton()
{
	if (tool_) {

		// Update the color.

		colorButton_->setColor(tool_->cursorColor());

		// Update the color button visibility.

		if (tool_->cursorVisible())
			colorButton_->show();
		else
			colorButton_->hide();

	} else {

		// If there is no tool, the color button should be hidden.

		colorButton_->hide();
	}
}

void AMDataPositionCursorToolView::updateVisibilityCheckBox()
{
	if (tool_)
		cursorVisibilityCheckBox_->setChecked(tool_->cursorVisible());
	else
		cursorVisibilityCheckBox_->setChecked(false);
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
