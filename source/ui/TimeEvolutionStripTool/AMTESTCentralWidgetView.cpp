#include "AMTESTCentralWidgetView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.h"
#include "util/TimeEvolutionStripTool/AMTESTStripTool.h"

AMTESTCentralWidgetView::AMTESTCentralWidgetView(QWidget *parent)
	: QWidget(parent)
{
	AMTESTServerConnectionButton *scalerButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("Scaler"));
	AMTESTServerConnectionButton *ampteksButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("Ampteks"));
	AMTESTServerConnectionButton *pvsButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("PVs"));
	pvsButton->setDisabled(true);

	QHBoxLayout *connectionButtonsLayout = new QHBoxLayout;
	connectionButtonsLayout->addWidget(scalerButton);
	connectionButtonsLayout->addWidget(ampteksButton);
	connectionButtonsLayout->addWidget(pvsButton);

	setupPlot();

	QVBoxLayout *centralWidgetLayout = new QVBoxLayout;
	centralWidgetLayout->addLayout(connectionButtonsLayout);
	centralWidgetLayout->addWidget(plotWidget_);

	setLayout(centralWidgetLayout);
}

AMTESTCentralWidgetView::~AMTESTCentralWidgetView()
{

}

void AMTESTCentralWidgetView::setupPlot()
{
	plotWidget_ = new MPlotWidget;
	plotWidget_->enableAntiAliasing(true);

	plot_ = new MPlot;
	plotWidget_->setPlot(plot_);

	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Time");
}

