#include "AMTESTCentralWidgetView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "appController/AMDSClientAppController.h"
#include "Connection/AMDSServer.h"

#include "ui/TimeEvolutionStripTool/AMTESTDataModelListView.h"
#include "ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"
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

	dataModelListView_ = new AMTESTDataModelListView;
	connect(AMTESTStripTool::stripTool(), SIGNAL(dataModelsCreated(AMTESTServerConnection*)), dataModelListView_, SLOT(addNewDataModels(AMTESTServerConnection*)));
	connect(AMTESTStripTool::stripTool(), SIGNAL(dataModelsDeleted(QStringList)), dataModelListView_, SLOT(removeDataModels(QStringList)));
	connect(AMTESTStripTool::stripTool(), SIGNAL(dataModelsCreated(AMTESTServerConnection*)), this, SLOT(updateWidgetAppearance()));
	connect(AMTESTStripTool::stripTool(), SIGNAL(dataModelsDeleted(QStringList)), this, SLOT(updateWidgetAppearance()));

	QHBoxLayout *horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(plotWidget_);
	horizontalLayout->addWidget(dataModelListView_);

	timeIntervalSpinBox_ = new QDoubleSpinBox;
	timeIntervalSpinBox_->setAlignment(Qt::AlignCenter);
	timeIntervalSpinBox_->setDecimals(3);
	timeIntervalSpinBox_->setSuffix("s");
	timeIntervalSpinBox_->setValue(1.0);
	startButton_ = new QPushButton(QIcon(":/22x22/media-playback-start.png"), "Start");
	startButton_->setEnabled(false);
	stopButton_ = new QPushButton(QIcon(":/22x22/media-playback-stop.png"), "Stop");
	stopButton_->setEnabled(false);

	connect(startButton_, SIGNAL(clicked(bool)), this, SLOT(startAcquisition()));
	connect(stopButton_, SIGNAL(clicked(bool)), this, SLOT(stopAcquisition()));

	QHBoxLayout *bottomBarLayout = new QHBoxLayout;
	bottomBarLayout->addWidget(timeIntervalSpinBox_);
	bottomBarLayout->addWidget(startButton_);
	bottomBarLayout->addWidget(stopButton_);

	QVBoxLayout *centralWidgetLayout = new QVBoxLayout;
	centralWidgetLayout->addLayout(connectionButtonsLayout);
	centralWidgetLayout->addLayout(horizontalLayout);
	centralWidgetLayout->addLayout(bottomBarLayout);

	setLayout(centralWidgetLayout);
}

AMTESTCentralWidgetView::~AMTESTCentralWidgetView()
{

}

void AMTESTCentralWidgetView::startAcquisition()
{
	dataModelListView_->setEnabled(false);

	QList<AMTESTServerConnection *> serverConnections = AMTESTStripTool::stripTool()->serverConnections();
	AMDSClientAppController *client = AMDSClientAppController::clientAppController();

	foreach (AMTESTServerConnection *serverConnection, serverConnections)
		if (serverConnection->serverIsActive())
			client->requestClientData(serverConnection->serverConfiguration().hostName(),
						  serverConnection->serverConfiguration().portNumber(),
						  client->getServerByServerIdentifier(serverConnection->serverConfiguration().serverIdentifier())->bufferNames(),
						  quint64(timeIntervalSpinBox_->value()*1000));
}

void AMTESTCentralWidgetView::stopAcquisition()
{
	dataModelListView_->setEnabled(true);
}

void AMTESTCentralWidgetView::updateWidgetAppearance()
{
	QList<AMTESTServerConnection *> serverConnections = AMTESTStripTool::stripTool()->serverConnections();
	bool hasConnections = false;

	foreach (AMTESTServerConnection *serverConnection, serverConnections)
		hasConnections |= serverConnection->serverIsActive();

	timeIntervalSpinBox_->setEnabled(hasConnections);
	startButton_->setEnabled(hasConnections);
	stopButton_->setEnabled(hasConnections);
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

