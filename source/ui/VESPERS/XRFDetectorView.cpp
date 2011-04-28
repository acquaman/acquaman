#include "XRFDetectorView.h"
#include "DeadTimeButton.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "MPlot/MPlotSeries.h"
#include "dataman/AMDataSourceSeriesData.h"
#include "ui/VESPERS/ROIPlotMarker.h"

#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QGroupBox>
#include <QComboBox>
#include <QToolButton>

// Brief detector view
/////////////////////////////////////////////

XRFBriefDetectorView::XRFBriefDetectorView(XRFDetector *detector, bool configureOnly, QWidget *parent)
	: AMBriefDetectorView(configureOnly, parent)
{
	// Setting this to zero even if detector is valid.  This is because I'll do a cast to the right detector in setDetector.
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool XRFBriefDetectorView::setDetector(AMDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	detector_ = static_cast<XRFDetector *>(detector);

	QLabel *name = new QLabel(detector_->name());
	deadTime_ = new QLabel(QString::number(detector_->deadTime()) + " %");
	QToolButton *details = new QToolButton;
	details->setText(tr("Detailed"));

	connect(detector_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(detector_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));
	connect(detector_, SIGNAL(readingsChanged(AMControlInfoList)), this, SLOT(onDeadTimeUpdate()));
	connect(details, SIGNAL(clicked()), this, SIGNAL(detailedViewClicked()));

	QHBoxLayout *briefLayout = new QHBoxLayout;
	briefLayout->addWidget(name);
	briefLayout->addWidget(deadTime_);
	briefLayout->addWidget(details);

	setLayout(briefLayout);

	return true;
}

void XRFBriefDetectorView::onDeadTimeUpdate()
{
	deadTime_->setText(QString::number(detector_->deadTime()) + " %");
}

// End brief detector view
////////////////////////////////////////////////////

// Detailed detector view
///////////////////////////////////////////////////
XRFDetailedDetectorView::XRFDetailedDetectorView(XRFDetector *detector, bool configureOnly, QWidget *parent)
	: AMDetailedDetectorView(configureOnly, parent)
{
	// Setting this to zero even if detector is valid.  This is because I'll do a cast to the right detector in setDetector.
	detector_ = 0;
	setDetector(detector, configureOnly);
}

bool XRFDetailedDetectorView::setDetector(AMDetector *detector, bool configureOnly)
{
	//I don't have a configure only view for these.  It doesn't make quite as much sense for the stand alone spectra to have configure only views.
	Q_UNUSED(configureOnly)

	// If there is no valid detector pointer, then return false.
	if (!detector)
		return false;

	detector_ = static_cast<XRFDetector *>(detector);
	connect(detector_, SIGNAL(detectorConnected(bool)), this, SLOT(setEnabled(bool)));

	elapsedTime_ = new QLabel(tr(" s"));
	connect(detector_->elapsedTimeControl(), SIGNAL(valueChanged(double)), this, SLOT(onElapsedTimeUpdate(double)));

	deadTime_ = new QLabel(tr(" %"));
	connect(detector_->deadTimeControl(), SIGNAL(controlSetValuesChanged(AMControlInfoList)), this, SLOT(onDeadTimeUpdate()));

	// Using a button group so I know which element I need to disable.
	DeadTimeButton *temp;
	deadTimeGroup_ = new QButtonGroup(this);
	deadTimeGroup_->setExclusive(false);
	QHBoxLayout *deadTimeLayout = new QHBoxLayout;
	deadTimeLayout->setSpacing(0);

	for (int i = 0; i < detector_->elements(); i++){

		temp = new DeadTimeButton(15.0, 30.0);
		temp->setCheckable(true);
		temp->setFixedSize(20, 20);
		connect(detector_->deadTimeControl()->at(i), SIGNAL(valueChanged(double)), temp, SLOT(setCurrent(double)));
		deadTimeLayout->addWidget(temp);
		deadTimeGroup_->addButton(temp, i);
	}

	if (detector_->elements() == 1)
		deadTimeGroup_->button(0)->setCheckable(false);
	else
		connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

	updateRate_ = new QComboBox;
	updateRate_->addItem("Passive");
	updateRate_->addItem("1 sec");
	updateRate_->addItem("0.2 sec");
	connect(updateRate_, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxUpdate(int)));
	connect(detector_->refreshRateControl(), SIGNAL(valueChanged(double)), this, SLOT(onUpdateRateUpdate(double)));

	setupPlot();

	connect(detector_->spectraControl(), SIGNAL(controlSetValuesChanged()), this, SLOT(resizeRoiMarkers()));

	QFont font(this->font());
	font.setBold(true);

	QLabel *elapsedTimeLabel = new QLabel(QString("Elapsed Time"));
	elapsedTimeLabel->setFont(font);
	QLabel *deadTimeLabel = new QLabel(QString("Dead Time"));
	deadTimeLabel->setFont(font);
	QLabel *updateRateLabel = new QLabel(QString("Update Rate"));
	updateRateLabel->setFont(font);

	QLabel *legend = new QLabel(QString("Legend"));
	legend->setFont(font);
	QLabel *kLines = new QLabel(QString("K-lines"));
	kLines->setFont(font);
	kLines->setStyleSheet("QLabel { background-color: rgb(0,255,0) ; border-width: 2px ; border-style: solid }");
	QLabel *lLines = new QLabel(QString("L-lines"));
	lLines->setFont(font);
	lLines->setStyleSheet("QLabel { background-color: rgb(255,255,0) ; border-width: 2px ; border-style: solid }");
	QLabel *mLines = new QLabel(QString("M-lines"));
	mLines->setFont(font);
	mLines->setStyleSheet("QLabel { background-color: rgb(0,255,255) ; border-width: 2px ; border-style: solid }");

	QVBoxLayout *viewControlLayout = new QVBoxLayout;
	viewControlLayout->addWidget(elapsedTimeLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(elapsedTime_, 0, Qt::AlignCenter);
	viewControlLayout->addWidget(deadTimeLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(deadTime_, 0, Qt::AlignCenter);
	viewControlLayout->addLayout(deadTimeLayout, Qt::AlignLeft);
	viewControlLayout->addWidget(updateRateLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(updateRate_, 0, Qt::AlignCenter);
	viewControlLayout->addStretch();

	QGroupBox *controlBox = new QGroupBox;
	controlBox->setLayout(viewControlLayout);
	controlBox->setMinimumWidth(80);
	controlBox->setFlat(true);

	QHBoxLayout *viewLayout = new QHBoxLayout;
	viewLayout->addWidget(view_);
	viewLayout->addWidget(controlBox);

	QVBoxLayout *detectorLayout = new QVBoxLayout;
	detectorLayout->addLayout(viewLayout);

	setLayout(detectorLayout);

	return true;
}

void XRFDetailedDetectorView::onComboBoxUpdate(int index)
{
	switch(index){

	case 0:
		detector_->setRefreshRateControl(XRFDetectorInfo::Passive);
		break;
	case 1:
		detector_->setRefreshRateControl(XRFDetectorInfo::Slow);
		break;
	case 2:
		detector_->setRefreshRateControl(XRFDetectorInfo::Fast);
		break;
	}
}

void XRFDetailedDetectorView::onUpdateRateUpdate(double val)
{
	switch((int)val){

	case 0:
		updateRate_->setCurrentIndex(0);
		break;
	case 6:
		updateRate_->setCurrentIndex(1);
		break;
	case 8:
		updateRate_->setCurrentIndex(2);
	}
}

void XRFDetailedDetectorView::setupPlot()
{
	lines_ = new QList<MPlotPoint *>;

	// Create the plot window.
	view_ = new MPlotWidget;
	view_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	plot_ = new MPlot;
	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 11));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 11));
	plot_->axisBottom()->setAxisName("Energy, eV");
	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 11));
	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 11));
	plot_->axisLeft()->setAxisName("Counts");

	// Set the margins for the plot.
	plot_->setMarginLeft(10);
	plot_->setMarginBottom(15);
	plot_->setMarginRight(2);
	plot_->setMarginTop(2);

	MPlotSeriesBasic *series;

	for (int i = 0; i < detector_->elements(); i++){

		series = new MPlotSeriesBasic;
		series->setModel(new AMDataSourceSeriesData(detector_->dataSource(i)));
		series->setMarker(MPlotMarkerShape::None);
		series->setDescription(detector_->dataSource(i)->name());
		series->setLinePen(QPen(getColor(i)));
		plot_->addItem(series);
	}

	// Enable autoscaling of both axes.
	plot_->enableAutoScaleLeft(true);
	plot_->enableAutoScaleBottom(true);

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	view_->setPlot(plot_);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);
}

void XRFDetailedDetectorView::resizeRoiMarkers()
{
	if (markers_.isEmpty())
		return;

	ROIPlotMarker *temp;
	double height = getMaximumHeight(plot_->item(0));

	for (int i = 0; i < markers_.size(); i++){

		temp = (ROIPlotMarker *)(markers_.at(i));
		temp->setHeight(height);
	}
}

double XRFDetailedDetectorView::getMaximumHeight(MPlotItem *data)
{
	double max = 0;
	MPlotAbstractSeries *temp = qgraphicsitem_cast<MPlotAbstractSeries *>(data);

	if (!temp)
		return max;

	AMDataSourceSeriesData *modal = (AMDataSourceSeriesData *)temp->model();

	for (int i = 0; i < modal->count(); i++){

		if (max < modal->y(i))
			max = modal->y(i);
	}

	return max;
}

void XRFDetailedDetectorView::onAdditionOfRegionOfInterest(AMElement *el, QPair<QString, QString> line)
{
	AMROIInfo info(el->symbol()+" "+line.first, line.second.toDouble(), 0.025, detector_->scale());
	detector_->addRegionOfInterest(info);
	ROIPlotMarker *newMarker = new ROIPlotMarker(info.name(), info.energy(), info.energy()*(1-info.width()/2), info.energy()*(1+info.width()/2), plot_->axisLeft()->max());
	markers_ << newMarker;
	plot_->addItem(newMarker);
}

void XRFDetailedDetectorView::onRemovalOfRegionOfInterest(AMElement *el, QPair<QString, QString> line)
{
	detector_->removeRegionOfInterest(el->symbol()+" "+line.first);

	MPlotItem *removeMe = 0;
	ROIPlotMarker *temp;

	for (int i = 0; i < markers_.size(); i++){

		temp = (ROIPlotMarker *)(markers_.at(i));

		if (temp->description().compare(el->symbol()+" "+line.first) == 0)
			removeMe = markers_.at(i);
	}

	if (removeMe)
		plot_->removeItem(removeMe);
}

void XRFDetailedDetectorView::showEmissionLines(AMElement *el)
{
	QList<QPair<QString, QString> > toBeAdded;
	QString lineName;

	for (int i = 0; i < el->emissionLines().size(); i++){

		lineName = el->emissionLines().at(i).first;

		if (el->emissionLines().at(i).second.toDouble() <= detector_->maximumEnergy()*1000
			&& (lineName.compare(QString::fromUtf8("Kα2")) && lineName.compare(QString::fromUtf8("Lα2")) && lineName.compare(QString::fromUtf8("Lβ2"))
				&& lineName.compare("-")))
			toBeAdded << el->emissionLines().at(i);
	}

	if (!lines_->isEmpty()){

		for (int i = 0; i < lines_->size(); i++){

			plot_->removeItem(lines_->at(i));
		}

		lines_->clear();
	}

	MPlotPoint *point;
	QPair<QString, QString> current;

	for (int i = 0; i < toBeAdded.size(); i++){

		current = toBeAdded.at(i);
		point = new MPlotPoint(QPoint(current.second.toDouble(), 0));
		point->setMarker(MPlotMarkerShape::VerticalBeam, 1e5, getColor(current.first));
		point->setDescription(current.first + ": " + current.second + " eV");
		plot_->insertItem(point, i+detector_->elements());
		lines_->append(point);
	}
}

QColor XRFDetailedDetectorView::getColor(QString name)
{
	if (name.compare(QString::fromUtf8("Kα1")) == 0)
		return Qt::red;
	if (name.compare(QString::fromUtf8("Kβ1")) == 0)
		return Qt::magenta;
	if (name.compare(QString::fromUtf8("Lα1")) == 0)
		return Qt::darkBlue;
	if (name.compare(QString::fromUtf8("Lβ1")) == 0)
		return Qt::blue;
	if (name.compare(QString::fromUtf8("Lγ1")) == 0)
		return Qt::darkCyan;
	if (name.compare(QString::fromUtf8("Mα1")) == 0)
		return Qt::darkGreen;

	return Qt::black;
}

QColor XRFDetailedDetectorView::getColor(int index)
{
	int i = index%10;

	switch(i){
	case 0:
		return Qt::red;
	case 1:
		return Qt::blue;
	case 2:
		return Qt::green;
	case 3:
		return Qt::yellow;
	case 4:
		return Qt::black;
	case 5:
		return QColor(255, 127, 0);
	case 6:
		return Qt::magenta;
	case 7:
		return Qt::cyan;
	case 8:
		return Qt::darkRed;
	case 9:
		return Qt::darkBlue;
	}

	return QColor(100, 100, 100);
}

void XRFDetailedDetectorView::elementClicked(int elementId)
{
	// If the button is checked then it means the element should be disabled.
	if (deadTimeGroup_->button(elementId)->isChecked())
		detector_->disableElement(elementId);
	else
		detector_->enableElement(elementId);
}

void XRFDetailedDetectorView::onDeadTimeUpdate()
{
	deadTime_->setText(QString::number(detector_->deadTime(), 'f', 2) + " %");
}

void XRFDetailedDetectorView::onElapsedTimeUpdate(double time)
{
	elapsedTime_->setText(QString::number(time, 'f', 2) + " s");
}

// End detailed detector view
/////////////////////////////////////////////////////
