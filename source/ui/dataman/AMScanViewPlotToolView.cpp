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

	xPositionBox_ = new QDoubleSpinBox();
	xPositionBox_->setMinimum(-1000000);
	xPositionBox_->setMaximum(1000000);
	xPositionBox_->setValue(0);

	yPositionBox_ = new QDoubleSpinBox();
	yPositionBox_->setMinimum(-1000000);
	yPositionBox_->setMaximum(1000000);
	yPositionBox_->setValue(0);

	markerComboBox_ = new AMPlotMarkerComboBox(QList<MPlotMarkerShape::Shape>() << MPlotMarkerShape::VerticalBeam << MPlotMarkerShape::HorizontalBeam << MPlotMarkerShape::Cross);

	colorButton_ = new AMColorPickerButton(QColor());

	visibilityCheckBox_ = new QCheckBox("Show cursor");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->addWidget(positionPrompt);
	layout->addWidget(positionLabel_);
	layout->addWidget(xPositionBox_);
	layout->addWidget(yPositionBox_);
	layout->addWidget(markerComboBox_);
	layout->addWidget(colorButton_);
	layout->addWidget(visibilityCheckBox_);

	setLayout(layout);

	// Initial settings.

	xPositionBox_->hide();
	yPositionBox_->hide();
	markerComboBox_->hide();
	colorButton_->hide();
	visibilityCheckBox_->setChecked(false);

	// Make connections.

	connect( xPositionBox_, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()) );
	connect( yPositionBox_, SIGNAL(valueChanged(double)), this, SLOT(onPositionChanged()) );
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
			connect( tool_, SIGNAL(cursorPositionChanged(QPointF)), this, SLOT(updatePositionBoxes()) );
			connect( tool_, SIGNAL(unitsChanged(QStringList)), this, SLOT(updatePositionLabel()) );
			connect( tool_, SIGNAL(unitsChanged(QStringList)), this, SLOT(updatePositionBoxes()) );
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
	xPositionBox_->blockSignals(true);
	yPositionBox_->blockSignals(true);
	markerComboBox_->blockSignals(true);
	colorButton_->blockSignals(true);
	visibilityCheckBox_->blockSignals(true);

	positionLabel_->clear();
	xPositionBox_->setValue(0);
	yPositionBox_->setValue(0);
	markerComboBox_->setSelectedMarkerShape(MPlotMarkerShape::None);
	colorButton_->setColor(QColor());
	visibilityCheckBox_->setChecked(false);

	xPositionBox_->blockSignals(false);
	yPositionBox_->blockSignals(false);
	markerComboBox_->blockSignals(false);
	colorButton_->blockSignals(false);
	visibilityCheckBox_->blockSignals(false);
}

void AMDataPositionCursorToolView::update()
{
	updatePositionLabel();
	updatePositionBoxes();
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
		tool_->setDataPosition(QPointF(xPositionBox_->value(), yPositionBox_->value()));
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
	QString toDisplay = QString("");

	if (tool_)
		toDisplay = positionToString(tool_->cursorPosition(), tool_->units());

	positionLabel_->setText(toDisplay);
}

void AMDataPositionCursorToolView::updatePositionBoxes()
{
	if (tool_) {
		QPointF cursorPosition = tool_->cursorPosition();
		QStringList unitsList = tool_->units();

		// Update the x position box.

		if (xPositionBox_->value() != cursorPosition.x()) {
			xPositionBox_->setValue(cursorPosition.x());

			if (unitsList.count() > 0)
				xPositionBox_->setSuffix(QString(" %1").arg(unitsList.at(0)));
			else
				xPositionBox_->setSuffix("");
		}

		// Update the y position box.

		if (yPositionBox_->value() != cursorPosition.y()) {
			yPositionBox_->setValue(cursorPosition.y());

			if (unitsList.count() > 1)
				yPositionBox_->setSuffix(QString(" %1").arg(unitsList.at(1)));
			else
				yPositionBox_->setSuffix("");
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
			xPositionBox_->show();
			yPositionBox_->show();
			markerComboBox_->show();
			colorButton_->show();

		} else {
			positionLabel_->show();
			xPositionBox_->hide();
			yPositionBox_->hide();
			markerComboBox_->hide();
			colorButton_->hide();
		}

		visibilityCheckBox_->setChecked(cursorVisible);
	}
}

QString AMDataPositionCursorToolView::positionToString(double value, const QString &units) const
{
	QString result = QString("%1").arg(value);

	if (!units.isEmpty() && units != " ")
		result.append(QString(" %1").arg(units));

	return result;
}

QString AMDataPositionCursorToolView::positionToString(const QPointF &values, const QStringList &units) const
{
	QString result;

	QString xString = positionToString(values.x(), units.size() > 0 ? units.at(0) : "");
	QString yString = positionToString(values.y(), units.size() > 1 ? units.at(1) : "");

	result = QString("(%1, %2)").arg(xString).arg(yString);

	return result;
}
