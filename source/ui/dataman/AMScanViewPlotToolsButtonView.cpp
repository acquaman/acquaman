#include "AMScanViewPlotToolsButtonView.h"
#include "MPlot/MPlotTools.h"

AMScanViewPlotToolsButtonView::AMScanViewPlotToolsButtonView(AMScanViewPlotTools *tools, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	tools_ = 0;

	buttons_ = new QButtonGroup(this);
	buttons_->setExclusive(false);

	// Create and set layouts.

	layout_ = new QHBoxLayout();
	layout_->setMargin(0);
	layout_->setSpacing(0);

	setLayout(layout_);

	// Make connections.

	connect( buttons_, SIGNAL(buttonClicked(int)), this, SLOT(onToolButtonClicked(int)) );

	// Current settings.

	setTools(tools);
}

AMScanViewPlotToolsButtonView::~AMScanViewPlotToolsButtonView()
{

}

void AMScanViewPlotToolsButtonView::setTools(AMScanViewPlotTools *newTools)
{
	if (tools_ != newTools) {

		if (tools_) {
			disconnect( tools_, 0, this, 0 );
		}

		tools_ = newTools;

		if (tools_) {
			connect( tools_, SIGNAL(toolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(refresh()) );
			connect( tools_, SIGNAL(selectedToolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(update()) );
		}

		refresh();

		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotToolsButtonView::clear()
{
	int buttonCount = buttons_->buttons().count();

	for (int buttonIndex = 0; buttonIndex < buttonCount; buttonIndex++) {
		QAbstractButton *button = buttons_->button(buttonIndex);
		layout_->removeWidget(button);
		buttons_->removeButton(button);
		button->deleteLater();
	}
}

void AMScanViewPlotToolsButtonView::update()
{
	if (tools_) {

		buttons_->blockSignals(true);

		int toolCount = tools_->tools().count();

		for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
			MPlotAbstractTool *tool = tools_->tools().at(toolIndex);
			QAbstractButton *button = buttons_->button(toolIndex);

			if (tool && button) {
				if (tools_->isSelectedTool(tool))
					button->setChecked(true);
				else
					button->setChecked(false);
			}
		}

		buttons_->blockSignals(false);
	}
}

void AMScanViewPlotToolsButtonView::refresh()
{
	// Clear the view.

	clear();

	// Construct new view, according to the present tool information.

	if (tools_) {

		int toolCount = tools_->tools().count();

		// Iterate through all available tools, creating a button for each.

		for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
			MPlotAbstractTool *tool = tools_->tools().at(toolIndex);

			if (tool) {
				QToolButton *button = createToolButton(tool);
				buttons_->addButton(button, toolIndex);
				layout_->addWidget(button);
			}
		}
	}

	// Update view.

	update();
}

void AMScanViewPlotToolsButtonView::onToolButtonClicked(int buttonIndex)
{
	if (tools_) {

		// Identify the button that was clicked.

		int buttonCount = buttons_->buttons().count();

		if (buttonIndex >= 0 && buttonIndex < buttonCount) {

			QAbstractButton *button = buttons_->button(buttonIndex);

			// Identify the tool associated with the clicked button, select it.

			MPlotAbstractTool *tool = buttonToTool(button);

			if (tool) {
				if (tools_->isSelectedTool(tool))
					tools_->removeSelectedTool(tool);
				else
					tools_->addSelectedTool(tool);
			}
		}
	}
}

QToolButton* AMScanViewPlotToolsButtonView::createToolButton(MPlotAbstractTool *tool)
{
	QToolButton *toolButton = 0;

	if (tool) {
		toolButton = new QToolButton();
		toolButton->setIcon(toolToIcon(tool));
		toolButton->setIconSize(QSize(16, 16));
		toolButton->setText(tool->name());
		toolButton->setStatusTip(tool->description());
		toolButton->setToolTip(tool->name());
		toolButton->setCheckable(true);
		toolButton->setAutoRaise(false);
	}

	return toolButton;
}

MPlotAbstractTool* AMScanViewPlotToolsButtonView::buttonToTool(QAbstractButton *button)
{
	MPlotAbstractTool *result = 0;

	if (tools_) {
		int buttonIndex = buttons_->id(button);
		result = tools_->tools().at(buttonIndex);
	}

	return result;
}

QAbstractButton* AMScanViewPlotToolsButtonView::toolToButton(MPlotAbstractTool *tool)
{
	QAbstractButton *result = 0;

	if (tools_) {
		int toolIndex = tools_->tools().indexOf(tool);
		result = buttons_->button(toolIndex);
	}

	return result;
}

QIcon AMScanViewPlotToolsButtonView::toolToIcon(MPlotAbstractTool *tool)
{
	QIcon icon;

	if (tool) {

		MPlotPlotSelectorTool *selectorTool = qobject_cast<MPlotPlotSelectorTool*>(tool);
		if (selectorTool) {
			icon = QIcon(":/pointer-black.png");
		}

		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);
		if (positionTool) {
			icon = QIcon(":/finger-point-click.png");
		}

		MPlotDragZoomerTool *dragZoomerTool = qobject_cast<MPlotDragZoomerTool*>(tool);
		if (dragZoomerTool)
			icon = QIcon(":/Cursor-Select.png");

		MPlotWheelZoomerTool *wheelZoomerTool = qobject_cast<MPlotWheelZoomerTool*>(tool);
		if (wheelZoomerTool)
			icon = QIcon(":/search-black.png");
	}

	return icon;
}
