#include "AMScanViewPlotToolView.h"

AMScanViewPlotToolView::AMScanViewPlotToolView(QWidget *parent) :
	QWidget(parent)
{

}

AMScanViewPlotToolView::~AMScanViewPlotToolView()
{

}









AMDataPositionToolView::AMDataPositionToolView(AMDataPositionTool *tool, QWidget *parent, bool valueVisible, bool cursorVisible) :
	AMScanViewPlotToolView(parent)
{
	// Initialize member variables.

	tool_ = 0;

	// Create UI elements.

	valuePrompt_ = new QLabel("Value:");
	value_ = new QLabel();

	valueBox_ = new QGroupBox();
	valueBox_->setFlat(true);

	cursorColor_ = new AMColorPickerButton(QColor(Qt::black));
	cursorVisibility_ = new QCheckBox("Show cursor");

	cursorBox_ = new QGroupBox();
	cursorBox_->setFlat(true);

	// Create and set layouts.

	QHBoxLayout *valueBoxLayout = new QHBoxLayout();
	valueBoxLayout->setMargin(0);
	valueBoxLayout->addWidget(valuePrompt_);
	valueBoxLayout->addWidget(value_);

	valueBox_->setLayout(valueBoxLayout);

	QHBoxLayout *cursorBoxLayout = new QHBoxLayout();
	cursorBoxLayout->setMargin(0);
	cursorBoxLayout->addWidget(cursorColor_);
	cursorBoxLayout->addWidget(cursorVisibility_);

	cursorBox_->setLayout(cursorBoxLayout);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(valueBox_);
	layout->addWidget(cursorBox_);

	setLayout(layout);

	// Make connections.

	connect( cursorColor_, SIGNAL(colorChanged(QColor)), this, SLOT(onCursorColorChanged()) );
	connect( cursorVisibility_, SIGNAL(clicked()), this, SLOT(onCursorVisibilityClicked()) );

	// Current settings.

	setTool(tool);
	setValueWidgetVisibility(valueVisible);
	setCursorWidgetVisibility(cursorVisible);
}

AMDataPositionToolView::~AMDataPositionToolView()
{

}

void AMDataPositionToolView::setTool(AMDataPositionTool *newTool)
{
	if (tool_ != newTool) {

		if (tool_) {
			disconnect( tool_, 0, this, 0 );
		}

		tool_ = newTool;

		if (tool_) {
			connect( tool_, SIGNAL(positionChanged(QPointF)), this, SLOT(updateValues()) );
			connect( tool_, SIGNAL(cursorVisibilityChanged(bool)), this, SLOT(updateCursorVisibility()) );
			connect( tool_, SIGNAL(cursorColorChanged(QColor)), this, SLOT(updateCursorColor()) );
		}

		refresh();

		emit toolChanged(tool_);
	}
}

void AMDataPositionToolView::clear()
{
	value_->clear();
	cursorVisibility_->setChecked(false);
	cursorColor_->setColor(QColor());


}

void AMDataPositionToolView::update()
{
	updateValues();
	updateCursorVisibility();
	updateCursorColor();
	updateValueWidgetVisibility();
	updateCursorWidgetVisibility();
}

void AMDataPositionToolView::refresh()
{
	// Clear the view.

	clear();

	// Update with the most recent settings.

	update();
}

void AMDataPositionToolView::setValueWidgetVisibility(bool isVisible)
{
	if (valueWidgetShown_ != isVisible) {
		valueWidgetShown_ = isVisible;
		emit valueWidgetVisibilityChanged(valueWidgetShown_);

		updateValueWidgetVisibility();
	}
}

void AMDataPositionToolView::setCursorWidgetVisibility(bool isVisible)
{
	if (cursorBox_->isVisible() != isVisible) {
		cursorWidgetShown_ = isVisible;
		emit cursorWidgetVisibilityChanged(cursorWidgetShown_);

		updateCursorWidgetVisibility();
	}
}

void AMDataPositionToolView::updateValues()
{
	if (tool_)
		value_->setText( valuesToString(tool_->currentPosition(), tool_->units()) );
	else
		value_->clear();
}

void AMDataPositionToolView::updateCursorVisibility()
{
	if (tool_)
		cursorVisibility_->setChecked(tool_->cursorVisibility());
	else
		cursorVisibility_->setChecked(false);
}

void AMDataPositionToolView::updateCursorColor()
{
	if (tool_)
		cursorColor_->setColor(tool_->cursorColor());
	else
		cursorColor_->setColor(QColor());
}

void AMDataPositionToolView::updateValueWidgetVisibility()
{
	if (tool_ && valueWidgetShown_)
		valueBox_->show();
	else
		valueBox_->hide();
}

void AMDataPositionToolView::updateCursorWidgetVisibility()
{
	if (tool_ && cursorWidgetShown_)
		cursorBox_->show();
	else
		cursorBox_->hide();
}

void AMDataPositionToolView::onCursorColorChanged()
{
	AMDataPositionTool *tool = qobject_cast<AMDataPositionTool*>(tool_);

	if (tool) {
		tool->setCursorColor(cursorColor_->color());
	}
}

void AMDataPositionToolView::onCursorVisibilityClicked()
{
	AMDataPositionTool *tool = qobject_cast<AMDataPositionTool*>(tool_);

	if (tool) {
		tool->setCursorVisibility(cursorVisibility_->isChecked());
	}
}

QString AMDataPositionToolView::valuesToString(const QPointF &values, const QStringList &units)
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
