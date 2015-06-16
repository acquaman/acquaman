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

void AMScanViewToolBar::onToolBoxItemActivated(const QString &text)
{
	Q_UNUSED(text)
}
