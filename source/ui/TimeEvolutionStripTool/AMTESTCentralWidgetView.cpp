#include "AMTESTCentralWidgetView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "appController/AMDSClientAppController.h"
#include "Connection/AMDSServer.h"

#include "MPlot/MPlotSeries.h"
#include "MPlot/MPlotImage.h"
#include "MPlot/MPlotTools.h"

#include "ui/TimeEvolutionStripTool/AMTESTDataModelListView.h"
#include "ui/TimeEvolutionStripTool/AMTESTServerConnectionButton.h"
#include "util/TimeEvolutionStripTool/AMTESTDataModel.h"
#include "util/TimeEvolutionStripTool/AMTESTServerConnection.h"
#include "util/TimeEvolutionStripTool/AMTESTStripTool.h"

AMTESTCentralWidgetView::AMTESTCentralWidgetView(QWidget *parent)
	: QWidget(parent)
{
	AMTESTServerConnectionButton *scalerButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("Scaler"));
	AMTESTServerConnectionButton *ampteksButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("Ampteks"));
	AMTESTServerConnectionButton *pvsButton = new AMTESTServerConnectionButton(AMTESTStripTool::stripTool()->serverConnectionByName("PVs"));
//	pvsButton->setDisabled(true);

	QHBoxLayout *connectionButtonsLayout = new QHBoxLayout;
	connectionButtonsLayout->addWidget(scalerButton);
	connectionButtonsLayout->addWidget(ampteksButton);
	connectionButtonsLayout->addWidget(pvsButton);

	setupPlot();

	AMTESTStripTool *stripTool = AMTESTStripTool::stripTool();
	dataModelListView_ = new AMTESTDataModelListView;
	connect(stripTool, SIGNAL(dataModelsCreated(AMTESTServerConnection*)), dataModelListView_, SLOT(addNewDataModels(AMTESTServerConnection*)));
	connect(stripTool, SIGNAL(dataModelsDeleted(QStringList)), dataModelListView_, SLOT(removeDataModels(QStringList)));
	connect(stripTool, SIGNAL(dataModelsDeleted(QStringList)), this, SLOT(onDataModelsDeleted(QStringList)));
	connect(stripTool, SIGNAL(dataModelsCreated(AMTESTServerConnection*)), this, SLOT(updateWidgetAppearance()));
	connect(stripTool, SIGNAL(dataModelsDeleted(QStringList)), this, SLOT(updateWidgetAppearance()));
	connect(dataModelListView_, SIGNAL(dataModelToBeAdded(QString)), this, SLOT(onDataModelToBeAdded(QString)));
	connect(dataModelListView_, SIGNAL(dataModelToBeRemoved(QString)), this, SLOT(onDataModelToBeRemoved(QString)));

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

	imageDataModelName_ = "";

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
	startButton_->setEnabled(false);

	QList<AMTESTServerConnection *> serverConnections = AMTESTStripTool::stripTool()->serverConnections();

	foreach (AMTESTServerConnection *serverConnection, serverConnections)
		if (serverConnection->serverIsActive())
			serverConnection->startContinuousDataRequest(quint64(timeIntervalSpinBox_->value()*1000));
}

void AMTESTCentralWidgetView::stopAcquisition()
{
	startButton_->setEnabled(true);

	QList<AMTESTServerConnection *> serverConnections = AMTESTStripTool::stripTool()->serverConnections();

	foreach (AMTESTServerConnection *serverConnection, serverConnections)
		if (serverConnection->serverIsActive())
			serverConnection->stopContinuousDataRequest();

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

void AMTESTCentralWidgetView::onDataModelToBeAdded(const QString &name)
{
	AMTESTDataModel *dataModel = 0;
	QList<AMTESTServerConnection *> serverConnections = AMTESTStripTool::stripTool()->serverConnections();

	for (int i = 0, size = serverConnections.size(); i < size && dataModel == 0; i++)
		dataModel = serverConnections.at(i)->dataModelFromName(name);

	if (dataModel->isSeriesDataModel()){

		MPlotSeriesBasic *series = new MPlotSeriesBasic;
		series->setModel(dataModel->seriesDataModel());
		series->setDescription(dataModel->name());
		series->setMarker(MPlotMarkerShape::None);
		series->setLinePen(QPen(nextColor()));
		plot_->addItem(series);
	}

	else if (dataModel->isImageDataModel()){

		if (!imageDataModelName_.isEmpty())
			onDataModelToBeRemoved(imageDataModelName_);

		imageDataModelName_ = name;
		MPlotImageBasic *image = new MPlotImageBasic;
		image->setModel(dataModel->imageDataModel());
		image->setDescription(dataModel->name());
		image->setColorMap(MPlotColorMap::Jet);
		image->setZValue(-5);
		plot_->addItem(image);
	}
}

void AMTESTCentralWidgetView::onDataModelToBeRemoved(const QString &name)
{
	foreach (MPlotItem *item, plot_->plotItems())
		if (item->description() == name){

			plot_->removeItem(item);
			delete item;
		}
}

void AMTESTCentralWidgetView::onDataModelsDeleted(const QStringList &dataModelNames)
{
	foreach (QString name, dataModelNames)
		onDataModelToBeRemoved(name);
}

void AMTESTCentralWidgetView::setupPlot()
{
	plotWidget_ = new MPlotWidget;
	plotWidget_->setMinimumSize(400, 300);
	plotWidget_->enableAntiAliasing(true);

	plot_ = new MPlot;
	plotWidget_->setPlot(plot_);

	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Time");

	MPlotDragZoomerTool *dragZoomTool = new MPlotDragZoomerTool;
	plot_->addTool(dragZoomTool);
}

QColor AMTESTCentralWidgetView::nextColor() const
{
	static int i = 0;

	switch(i++ % 11) {
	case 0: return QColor(255, 0, 128);
	case 1: return QColor(0, 128, 255);
	case 2: return QColor(128, 255, 0);
	case 3: return QColor(255, 128, 0);
	case 4: return QColor(128, 0, 255);
	case 5: return QColor(0, 0, 255);
	case 6: return QColor(0, 128, 0);
	case 7: return QColor(255, 0, 0);
	case 8: return QColor(0, 64, 128);
	case 9: return QColor(128, 64, 0);
	case 10: default: return QColor(128, 0, 64);
	}
}

