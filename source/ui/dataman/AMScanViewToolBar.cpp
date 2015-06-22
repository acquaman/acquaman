#include "AMScanViewToolBar.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotAbstractTool.h"
#include "MPlot/MPlotTools.h"
#include "dataman/AMScanSetModel.h"

#include <QDebug>

AMScanViewToolBar::AMScanViewToolBar(AMScanViewPlotToolOptions *toolOptions, QWidget *parent) :
	QGroupBox(parent)
{
	// Initialize member variables.

	toolOptions_ = 0;
	toolButtons_ = new QButtonGroup(this);

	// Create and set layouts.

	toolButtonsLayout_ = new QHBoxLayout();
	toolButtonsLayout_->setSpacing(0);

	toolViewsLayout_ = new QHBoxLayout();

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setMargin(6);
	layout->addLayout(toolButtonsLayout_);
	layout->addLayout(toolViewsLayout_);
	layout->addStretch();

	setLayout(layout);

	// Make connections.

	connect( toolButtons_, SIGNAL(buttonClicked(int)), this, SLOT(onToolButtonClicked(int)) );

	// Current settings.

	setToolOptions(toolOptions);

	qDebug() << "\n";
}

AMScanViewToolBar::~AMScanViewToolBar()
{

}

void AMScanViewToolBar::setToolOptions(AMScanViewPlotToolOptions *newOptions)
{
	if (toolOptions_ != newOptions) {

		qDebug() << "Setting tool bar tool options...";

		if (toolOptions_)
			toolOptions_->disconnect();

		toolOptions_ = newOptions;

		if (toolOptions_) {
			connect( toolOptions_, SIGNAL(toolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(onToolOptionsToolsChanged()) );
			connect( toolOptions_, SIGNAL(selectedToolsChanged(QList<MPlotAbstractTool*>)), this, SLOT(onToolOptionsSelectedToolsChanged()) );
		}

		onToolOptionsToolsChanged();
		onToolOptionsSelectedToolsChanged();

		qDebug() << "Tool bar tool options set successfully.";

		emit toolOptionsChanged(toolOptions_);
	}
}

void AMScanViewToolBar::updateToolButtonsChecked()
{
	if (toolOptions_) {

		toolButtons_->blockSignals(true);

		qDebug() << "Updating tool buttons checked.";

		foreach (QAbstractButton *button, toolButtons_->buttons()) {
			if (button && toolOptions_->isSelectedTool(button->text()))
				button->setChecked(true);
			else
				button->setChecked(false);
		}

		toolButtons_->blockSignals(false);
	}
}

void AMScanViewToolBar::updateToolViewsVisibility()
{
	if (toolOptions_) {

		qDebug() << "Updating tool view visibility.";

		for (int i = 0; i < toolOptions_->tools().count(); i++) {
			MPlotAbstractTool *tool = toolOptions_->tools().at(i);
			QWidget *toolView = toolViews_.at(i);

			if (toolView) {
				if (toolOptions_->isSelectedTool(tool)) {
					qDebug() << "Showing tool view.";
					toolView->show();
				} else {
					qDebug() << "Hiding tool view.";
					toolView->hide();
				}
			}
		}
	}
}

void AMScanViewToolBar::onToolButtonClicked(int buttonIndex)
{
	QAbstractButton *button = toolButtons_->buttons().at(buttonIndex);

	if (button && toolOptions_) {

		qDebug() << "Button clicked.";

		// Identify the tool associated with the given button.

		MPlotAbstractTool *tool = buttonToTool(button);

		// If the button is checked, we are applying this tool; removing it otherwise.

		if (tool) {
			if (button->isChecked())
				toolOptions_->addSelectedTool(tool);
			else
				toolOptions_->removeSelectedTool(tool);
		}
	}
}

void AMScanViewToolBar::onToolOptionsToolsChanged()
{
	// Clear the view.

	foreach (QAbstractButton *button, toolButtons_->buttons()) {
		toolButtonsLayout_->removeWidget(button);
		toolButtons_->removeButton(button);
		button->deleteLater();
	}

	foreach (QWidget *toolView, toolViews_) {
		toolViewsLayout_->removeWidget(toolView);
		toolViews_.removeOne(toolView);
		toolView->deleteLater();
	}

	// If we have valid tool options, build the view up with the available tools.

	if (toolOptions_) {

		qDebug() << "Setting up toolbar view for new tools...";

		foreach (MPlotAbstractTool *tool, toolOptions_->tools()) {

			// Create buttons for each available tool and add to buttons group.

			QToolButton *button = createToolButton(tool);
			toolButtons_->addButton(button);
			toolButtonsLayout_->addWidget(button);

			// Create tool view for each tool and add to list of tool views.

			QWidget *toolView = createToolView(tool);
			toolViews_ << toolView;
			toolViewsLayout_->addWidget(toolView);
		}

		// Update tool buttons and views visibility to correspond to the tool's actual select state.

		updateToolButtonsChecked();
		updateToolViewsVisibility();

		qDebug() << "Toolbar view setup with new tools.";
	}
}

void AMScanViewToolBar::onToolOptionsSelectedToolsChanged()
{
	// Update the check state of each button and visibility of tool views by comparing against the selected tools.

	updateToolButtonsChecked();
	updateToolViewsVisibility();
}

QToolButton* AMScanViewToolBar::createToolButton(MPlotAbstractTool *tool)
{
	QToolButton *toolButton = 0;

	if (tool) {

		qDebug() << "Creating tool button.";

		toolButton = new QToolButton();
		toolButton->setIcon(toolToIcon(tool));
		toolButton->setIconSize(QSize(16, 16));
		toolButton->setText(tool->name());
		toolButton->setStatusTip(tool->description());
		toolButton->setCheckable(true);
		toolButton->setAutoRaise(true);
	}

	return toolButton;
}

QWidget* AMScanViewToolBar::createToolView(MPlotAbstractTool *tool)
{
	QWidget *view = 0;

	if (tool) {

		qDebug() << "Creating tool view.";

		MPlotDataPositionTool *positionTool = qobject_cast<MPlotDataPositionTool*>(tool);

		if (positionTool) {
			view = new AMScanViewToolBarDataPositionView(toolOptions_, this);
			qDebug() << "\tCreating data position tool view.";

		} else {
			view = new QWidget(this);
			qDebug() << "\tCreating other tool view.";
		}
	}

	return view;
}

QAbstractButton* AMScanViewToolBar::toolToButton(MPlotAbstractTool *tool)
{
	QAbstractButton *result = 0;

	if (toolOptions_ && tool) {
		int buttonIndex = 0;
		int buttonCount = toolButtons_->buttons().count();

		while (!result && buttonIndex < buttonCount) {
			QAbstractButton *button = toolButtons_->buttons().at(buttonIndex);

			if (button->text() == tool->name())
				result = button;
		}
	}

	return result;
}

MPlotAbstractTool* AMScanViewToolBar::buttonToTool(QAbstractButton *button)
{
	MPlotAbstractTool *result = 0;

	if (toolOptions_ && button) {
		int toolIndex = 0;
		int toolCount = toolOptions_->tools().count();

		while (!result && toolIndex < toolCount) {
			MPlotAbstractTool *tool = toolOptions_->tools().at(toolIndex);

			if (tool->name() == button->text())
				result = tool;

			toolIndex++;
		}
	}

	return result;
}

QIcon AMScanViewToolBar::toolToIcon(MPlotAbstractTool *tool)
{
	QIcon result;

	if (tool) {
		if (qobject_cast<MPlotDataPositionTool*>(tool))
			result = QIcon(":/pointer-black.png");
		else if (qobject_cast<MPlotDragZoomerTool*>(tool))
			result = QIcon(":/Cursor-Select.png");
		else if (qobject_cast<MPlotWheelZoomerTool*>(tool))
			result = QIcon(":/search-black.png");
		else
			result = QIcon(":dark-lightning.png");
	}

	return result;
}




AMScanViewPlotToolOptions::AMScanViewPlotToolOptions(QList<MPlotAbstractTool *> tools, bool exclusiveSelection, QObject *parent) :
	QObject(parent)
{
	// Initialize member variables.

	exclusiveSelection_ = false;

	// Current settings.

	setTools(tools);
	setExclusiveSelectionEnabled(exclusiveSelection);
}

AMScanViewPlotToolOptions::~AMScanViewPlotToolOptions()
{

}

bool AMScanViewPlotToolOptions::isSelectedTool(MPlotAbstractTool *tool)
{
	return selectedTools_.contains(tool);
}

bool AMScanViewPlotToolOptions::isSelectedTool(const QString &toolName)
{
	int toolIndex = 0;
	int toolCount = selectedTools_.count();
	bool toolFound = false;

	while (!toolFound && toolIndex < toolCount) {
		MPlotAbstractTool *tool = selectedTools_.at(toolIndex);

		if (tool->name() == toolName)
			toolFound = true;

		toolIndex++;
	}

	return toolFound;
}

void AMScanViewPlotToolOptions::setTools(QList<MPlotAbstractTool *> newTools)
{
	if (tools_ != newTools) {

		// Clear any previously selected tools.

		if (!tools_.isEmpty())
			selectedTools_.clear();

		tools_ = newTools;

		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotToolOptions::addTool(MPlotAbstractTool *newTool)
{
	if (!tools_.contains(newTool)) {
		tools_.append(newTool);
		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotToolOptions::removeTool(MPlotAbstractTool *tool)
{
	if (tools_.contains(tool)) {

		if (selectedTools_.contains(tool))
			removeSelectedTool(tool);

		tools_.removeOne(tool);

		emit toolsChanged(tools_);
	}
}

void AMScanViewPlotToolOptions::setSelectedTools(QList<MPlotAbstractTool *> newTools)
{
	if (selectedTools_ != newTools) {

		// Clear the list of selected tools.

		selectedTools_.clear();

		// Check that each of the new tools exists in the master tool options list before adding them to selection.

		foreach (MPlotAbstractTool *tool, newTools) {
			if (tools_.contains(tool))
				selectedTools_.append(tool);
		}

		dragZoomerToolCheck(selectedTools_);

		emit selectedToolsChanged(selectedTools_);
	}
}

void AMScanViewPlotToolOptions::addSelectedTool(MPlotAbstractTool *newTool)
{
	if (newTool && !selectedTools_.contains(newTool)) {

		if (exclusiveSelection_)
			selectedTools_.clear();

		if (!tools_.contains(newTool))
			addTool(newTool);

		selectedTools_.append(newTool);

		dragZoomerToolCheck(selectedTools_);

		emit selectedToolsChanged(selectedTools_);
	}
}

void AMScanViewPlotToolOptions::removeSelectedTool(MPlotAbstractTool *tool)
{
	if (tool && selectedTools_.contains(tool)) {
		selectedTools_.removeOne(tool);
		emit selectedToolsChanged(selectedTools_);
	}
}

void AMScanViewPlotToolOptions::setExclusiveSelectionEnabled(bool isEnabled)
{
	if (exclusiveSelection_ != isEnabled) {
		exclusiveSelection_ = isEnabled;

		if (exclusiveSelection_ && selectedTools_.count() > 1)
			setSelectedTools(QList<MPlotAbstractTool*>() << selectedTools_.first());

		emit exclusiveSelectionEnabledChanged(exclusiveSelection_);
	}
}

void AMScanViewPlotToolOptions::setValues(const QPointF &newPosition)
{	
	setValues(QList<double>() << newPosition.x() << newPosition.y());
}

void AMScanViewPlotToolOptions::setValues(QList<double> newValues)
{
	if (values_ != newValues) {
		values_ = newValues;
		emit valuesChanged(values_);
	}
}

void AMScanViewPlotToolOptions::setUnits(const QStringList &newUnits)
{
	if (units_ != newUnits) {
		units_ = newUnits;
		emit unitsChanged(units_);
	}
}

void AMScanViewPlotToolOptions::dragZoomerToolCheck(QList<MPlotAbstractTool*> toolList)
{
	// Make sure that any drag zoomer tools in the list appear first.
	// They should always be added to a plot first.

	int toolCount = toolList.size();
	int dragZoomerCount = 0;

	for (int toolIndex = 0; toolIndex < toolCount; toolIndex++) {
		MPlotDragZoomerTool *dragZoomer = qobject_cast<MPlotDragZoomerTool*>(toolList.at(toolIndex));

		if (dragZoomer) {
			if (toolIndex > dragZoomerCount)
				toolList.swap(dragZoomerCount, toolIndex);
			dragZoomerCount++;
		}
	}
}










AMScanViewToolBarDataPositionView::AMScanViewToolBarDataPositionView(AMScanViewPlotToolOptions *toolOptions, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	toolOptions_ = 0;

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

	setToolOptions(toolOptions);
}

AMScanViewToolBarDataPositionView::~AMScanViewToolBarDataPositionView()
{

}

void AMScanViewToolBarDataPositionView::setToolOptions(AMScanViewPlotToolOptions *newOptions)
{
	if (toolOptions_ != newOptions) {

		if (toolOptions_)
			toolOptions_->disconnect();

		toolOptions_ = newOptions;

		if (toolOptions_) {
			connect( toolOptions_, SIGNAL(valuesChanged(QList<double>)), this, SLOT(updateDisplay()) );
			connect( toolOptions_, SIGNAL(unitsChanged(QStringList)), this, SLOT(updateDisplay()) );
		}

		emit toolOptionsChanged(toolOptions_);
	}
}

void AMScanViewToolBarDataPositionView::updateDisplay()
{
	if (toolOptions_) {
		value_->setText(valuesToString(toolOptions_->values(), toolOptions_->units()));
	}
}

QString AMScanViewToolBarDataPositionView::valuesToString(QList<double> values, const QStringList &units)
{
	QString text = "";

	double valuesSize = values.size();
	double unitsSize = units.size();

	// An assumption is made here that someone might conceivably care about a value with no units,
	// but not units without a value.

	for (int i = 0; i < valuesSize; i++) {

		if (i == 0)
			text += "(";

		double value = values.at(i);
		QString unit = "";

		if (unitsSize >= i && !units.at(i).isEmpty())
			unit = " " + units.at(i);

		text += QString::number(value, 'f', 3) + unit;

		if (i < valuesSize - 1)
			text += ", ";
		else if (i == valuesSize - 1)
			text += ")";
	}

	return text;
}
