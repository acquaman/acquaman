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

QToolButton* AMScanViewPlotToolsButtonView::createToolButton(MPlotAbstractTool *tool)
{
	QToolButton *toolButton = 0;

	if (tool) {
		toolButton = new QToolButton();
		toolButton->setIcon(toolToIcon(tool));
		toolButton->setIconSize(QSize(16, 16));
		toolButton->setText(tool->name());
		toolButton->setStatusTip(tool->description());
		toolButton->setCheckable(true);
		toolButton->setAutoRaise(false);
	}

	return toolButton;
}

QIcon AMScanViewPlotToolsButtonView::toolToIcon(MPlotAbstractTool *tool)
{
	QIcon icon;

	if (tool) {
		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);
		if (positionTool) {
			icon = QIcon(":/pointer-black.png");
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
