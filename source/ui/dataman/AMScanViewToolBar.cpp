#include "AMScanViewToolBar.h"
#include "MPlot/MPlot.h"
#include "MPlot/MPlotAbstractTool.h"

AMScanViewToolBar::AMScanViewToolBar(MPlot *plot, QWidget *parent) :
    QWidget(parent)
{
	// Set member variables.

	plot_ = 0;

	// Create ui elements.

	QLabel *toolBoxPrompt = new QLabel("Tool:");

	toolBox_ = new QComboBox();

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(toolBoxPrompt);
	layout->addWidget(toolBox_);
	layout->addStretch();

	setLayout(layout);

	// Initial settings.

	toolBox_->setEnabled(false);

	// Make connections.

	connect( toolBox_, SIGNAL(activated(QString)), this, SLOT(onToolBoxItemActivated(QString)) );

	// Current settings.

	setPlot(plot);
}

AMScanViewToolBar::~AMScanViewToolBar()
{

}

void AMScanViewToolBar::setPlot(MPlot *newPlot)
{
	if (plot_ != newPlot) {

		if (plot_) {

			// Clear UI.

			toolBox_->clear();

			// Get new list of tools.

//			QList<MPlotAbstractTool*> tools = plot_->tools();

//			foreach (MPlotAbstractTool* tool, tools) {
//			}
		}

		plot_ = newPlot;

		if (plot_) {

		}

		emit plotChanged(plot_);
	}
}

void AMScanViewToolBar::setToolNames(const QStringList &names)
{
	toolBox_->clear();
	toolBox_->addItems(names);
}

void AMScanViewToolBar::onToolBoxItemActivated(const QString &text)
{
	Q_UNUSED(text)
}










AMScanViewToolBarDataPositionView::AMScanViewToolBarDataPositionView(bool enableValue, QWidget *parent) :
	QWidget(parent)
{
	// Initialize member variables.

	valueEnabled_ = false;

	// Create UI elements.

	QLabel *positionPrompt = new QLabel("Position:");
	position_ = new QLabel();
	positionDisplay_ = new QGroupBox();

	QLabel *valuePrompt = new QLabel("Value:");
	value_ = new QLabel();
	valueDisplay_ = new QGroupBox();

	// Create and set layouts.

	QHBoxLayout *positionDisplayLayout = new QHBoxLayout();
	positionDisplayLayout->addWidget(positionPrompt);
	positionDisplayLayout->addWidget(position_);

	positionDisplay_->setLayout(positionDisplayLayout);

	QHBoxLayout *valueDisplayLayout = new QHBoxLayout();
	valueDisplayLayout->addWidget(valuePrompt);
	valueDisplayLayout->addWidget(value_);

	valueDisplay_->setLayout(valueDisplayLayout);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(positionDisplay_);
	layout->addWidget(valueDisplay_);

	setLayout(layout);

	// Initial settings.

	valueDisplay_->hide();

	// Current settings.

	setValueEnabled(enableValue);
}

AMScanViewToolBarDataPositionView::~AMScanViewToolBarDataPositionView()
{

}

void AMScanViewToolBarDataPositionView::setValueEnabled(bool isEnabled)
{
	if (valueEnabled_ != isEnabled) {
		valueEnabled_ = isEnabled;
		valueDisplay_->setVisible(valueEnabled_);
		emit valueEnabledChanged(valueEnabled_);
	}
}

void AMScanViewToolBarDataPositionView::setPosition(const QString &newPosition)
{
	position_->setText(newPosition);
}

void AMScanViewToolBarDataPositionView::setValue(const QString &newValue)
{
	value_->setText(newValue);

	setValueEnabled(newValue.isEmpty());
}
