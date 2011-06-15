#include "XRFDetectorView.h"
#include "DeadTimeButton.h"
#include "acquaman/VESPERS/VESPERSXRFScanController.h"
#include "MPlot/MPlotAxisScale.h"
#include "dataman/AMDataSourceSeriesData.h"
#include "util/AMPeriodicTable.h"
#include "util/VESPERS/GeneralUtilities.h"

#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QGroupBox>
#include <QComboBox>
#include <QToolButton>
#include <QTimer>

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

	minimumEnergy_ = 0;
	maximumEnergy_ = 1e6;

	detector_ = static_cast<XRFDetector *>(detector);
	connect(detector_, SIGNAL(detectorConnected(bool)), this, SLOT(onDetecterConnected(bool)));
	//connect(detector_, SIGNAL(detectorConnected(bool)), this, SLOT(setEnabled(bool)));
	//connect(detector_, SIGNAL(addedRegionOfInterest(AMROIInfo)), this, SLOT(addRegionOfInterestMarker(AMROIInfo)));
	//connect(detector_, SIGNAL(removedRegionOfInterest(AMROIInfo)), this, SLOT(removeRegionOfInterestMarker(AMROIInfo)));
	//connect(detector_, SIGNAL(externalRegionsOfInterestChanged()), this, SLOT(onExternalRegionsOfInterestChanged()));

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/OFF.png").pixmap(20));
	//connect(detector_, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));

	elapsedTime_ = new QLabel(tr(" s"));
	//connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onElapsedTimeUpdate(double)));

	deadTime_ = new QLabel(tr(" %"));
	//connect(detector_, SIGNAL(deadTimeChanged()), this, SLOT(onDeadTimeUpdate()));

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
		//connect(detector_->deadTimeControl()->at(i), SIGNAL(valueChanged(double)), temp, SLOT(setCurrent(double)));
		deadTimeLayout->addWidget(temp);
		deadTimeGroup_->addButton(temp, i);
	}

	if (detector_->elements() == 1)
		deadTimeGroup_->button(0)->setCheckable(false);
	//else
	//	connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

	updateRate_ = new QComboBox;
	updateRate_->addItem("Passive");
	updateRate_->addItem("1 sec");
	updateRate_->addItem("0.2 sec");
	connect(updateRate_, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxUpdate(int)));
	connect(detector_, SIGNAL(refreshRateChanged(double)), this, SLOT(onUpdateRateUpdate(double)));

	isWaterfall_ = false;

	setupPlot();

	//connect(detector_, SIGNAL(roiUpdate(AMROI*)), this, SLOT(roiWidthUpdate(AMROI*)));

	QFont font(this->font());
	font.setBold(true);

	QLabel *statusLabel = new QLabel(QString("Acquiring"));
	statusLabel->setFont(font);
	QLabel *elapsedTimeLabel = new QLabel(QString("Elapsed Time"));
	elapsedTimeLabel->setFont(font);
	QLabel *deadTimeLabel = new QLabel(QString("Dead Time"));
	deadTimeLabel->setFont(font);
	QLabel *updateRateLabel = new QLabel(QString("Update Rate"));
	updateRateLabel->setFont(font);
	QLabel *logLabel = new QLabel(QString("Scale Axis"));
	logLabel->setFont(font);
	QLabel *rawElementLabel = new QLabel(QString("Raw Elements"));
	rawElementLabel->setFont(font);

	logButton_ = new QPushButton("Log");
	logButton_->setCheckable(true);
	logButton_->setChecked(false);
	connect(logButton_, SIGNAL(toggled(bool)), this, SLOT(onLogEnabled(bool)));

	waterfallButton_ = new QPushButton("Raw Spectra");
	waterfallButton_->setCheckable(true);
	connect(waterfallButton_, SIGNAL(toggled(bool)), this, SLOT(onWaterfallToggled(bool)));

	waterfallSeparation_ = new QDoubleSpinBox;
	waterfallSeparation_->setPrefix(QString::fromUtf8("Δh = "));
	waterfallSeparation_->setMinimum(0.0);
	waterfallSeparation_->setMaximum(1.0);
	waterfallSeparation_->setSingleStep(0.01);
	waterfallSeparation_->setValue(0.2);
	waterfallSeparation_->setAlignment(Qt::AlignCenter);
	waterfallSeparation_->setEnabled(false);
	connect(waterfallSeparation_, SIGNAL(valueChanged(double)), this, SLOT(onWaterfallSeparationChanged(double)));
	connect(waterfallButton_, SIGNAL(toggled(bool)), waterfallSeparation_, SLOT(setEnabled(bool)));

	/// \todo Save button hack.  Once auto-export works, redo this if need be.
	QPushButton *saveSpectraButton = new QPushButton(QIcon(":/Save.png"), "Save Spectra");
	connect(saveSpectraButton, SIGNAL(clicked()), this, SLOT(saveSpectra()));

	QHBoxLayout *statusLayout = new QHBoxLayout;
	statusLayout->addWidget(statusLabel, 0, Qt::AlignLeft);
	statusLayout->addWidget(status_, 0, Qt::AlignCenter);

	QVBoxLayout *viewControlLayout = new QVBoxLayout;
	viewControlLayout->addLayout(statusLayout);
	viewControlLayout->addWidget(elapsedTimeLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(elapsedTime_, 0, Qt::AlignCenter);
	viewControlLayout->addWidget(deadTimeLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(deadTime_, 0, Qt::AlignCenter);
	viewControlLayout->addLayout(deadTimeLayout, Qt::AlignLeft);
	viewControlLayout->addWidget(updateRateLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(updateRate_, 0, Qt::AlignCenter);
	viewControlLayout->addWidget(logLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(logButton_, 0, Qt::AlignCenter);
	viewControlLayout->addWidget(rawElementLabel, 0, Qt::AlignLeft);
	viewControlLayout->addWidget(waterfallButton_, 0, Qt::AlignCenter);

	if (detector_->elements() != 1)
		viewControlLayout->addWidget(waterfallSeparation_, 0, Qt::AlignCenter);

	viewControlLayout->addStretch();
	viewControlLayout->addWidget(saveSpectraButton, 0, Qt::AlignCenter);

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

void XRFDetailedDetectorView::onDetecterConnected(bool connected)
{
	setEnabled(connected);

	if (connected){

		connect(detector_, SIGNAL(addedRegionOfInterest(AMROIInfo)), this, SLOT(addRegionOfInterestMarker(AMROIInfo)));
		connect(detector_, SIGNAL(removedRegionOfInterest(AMROIInfo)), this, SLOT(removeRegionOfInterestMarker(AMROIInfo)));
		connect(detector_, SIGNAL(externalRegionsOfInterestChanged()), this, SLOT(onExternalRegionsOfInterestChanged()));
		connect(detector_, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
		connect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onElapsedTimeUpdate(double)));
		connect(detector_, SIGNAL(deadTimeChanged()), this, SLOT(onDeadTimeUpdate()));

		DeadTimeButton *temp;
		for (int i = 0; i < detector_->elements(); i++){

			temp = qobject_cast<DeadTimeButton *>(deadTimeGroup_->button(i));
			connect(detector_->deadTimeControl()->at(i), SIGNAL(valueChanged(double)), temp, SLOT(setCurrent(double)));
			temp->setCurrent(detector_->deadTimeControl()->at(i)->value());
		}

		if (detector_->elements() > 1)
			connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

		connect(detector_, SIGNAL(refreshRateChanged(double)), this, SLOT(onUpdateRateUpdate(double)));
		connect(detector_, SIGNAL(roiUpdate(AMROI*)), this, SLOT(roiWidthUpdate(AMROI*)));

		onStatusChanged();
		onElapsedTimeUpdate(detector_->elapsedTime());
		onDeadTimeUpdate();
		onUpdateRateUpdate(detector_->refreshRate());
	}

	else {

		disconnect(detector_, SIGNAL(addedRegionOfInterest(AMROIInfo)), this, SLOT(addRegionOfInterestMarker(AMROIInfo)));
		disconnect(detector_, SIGNAL(removedRegionOfInterest(AMROIInfo)), this, SLOT(removeRegionOfInterestMarker(AMROIInfo)));
		disconnect(detector_, SIGNAL(externalRegionsOfInterestChanged()), this, SLOT(onExternalRegionsOfInterestChanged()));
		disconnect(detector_, SIGNAL(statusChanged()), this, SLOT(onStatusChanged()));
		disconnect(detector_, SIGNAL(elapsedTimeChanged(double)), this, SLOT(onElapsedTimeUpdate(double)));
		disconnect(detector_, SIGNAL(deadTimeChanged()), this, SLOT(onDeadTimeUpdate()));

		DeadTimeButton *temp;
		for (int i = 0; i < detector_->elements(); i++){

			temp = qobject_cast<DeadTimeButton *>(deadTimeGroup_->button(i));
			disconnect(detector_->deadTimeControl()->at(i), SIGNAL(valueChanged(double)), temp, SLOT(setCurrent(double)));
		}

		if (detector_->elements() > 1)
			connect(deadTimeGroup_, SIGNAL(buttonClicked(int)), this, SLOT(elementClicked(int)));

		disconnect(detector_, SIGNAL(refreshRateChanged(double)), this, SLOT(onUpdateRateUpdate(double)));
		disconnect(detector_, SIGNAL(roiUpdate(AMROI*)), this, SLOT(roiWidthUpdate(AMROI*)));
	}
}

void XRFDetailedDetectorView::roiWidthUpdate(AMROI *roi)
{
	// Find the marker associated with the ROI and then change it.
	ROIPlotMarker *temp;

	for (int i = 0; i < markers_.size(); i++){

		temp = markers_.at(i);

		if (GeneralUtilities::removeGreek(temp->description()).compare(roi->name()) == 0){

			temp->setLowEnd(roi->low()*roi->scale());
			temp->setHighEnd(roi->high()*roi->scale());
		}
	}
}

void XRFDetailedDetectorView::onLogEnabled(bool logged)
{
	logged ? logButton_->setText("Linear") : logButton_->setText("Log");
	plot_->axisScaleLeft()->setLogScaleEnabled(logged);
}

void XRFDetailedDetectorView::onWaterfallToggled(bool isWaterfall)
{
	isWaterfall ? waterfallButton_->setText("Sum Spectra") : waterfallButton_->setText("Raw Spectra");
	isWaterfall_ = isWaterfall;

	if (isWaterfall){

		plot_->removeItem(corrSum_);

		for (int i = 0; i < detector_->elements(); i++)
			plot_->insertItem(rawDataSeries_.at(i), i);

		plot_->setAxisScaleWaterfall(MPlot::Left, waterfallSeparation_->value()*getMaximumHeight(plot_->item(0)));
	}
	else{

		for (int i = 0; i < detector_->elements(); i++)
			plot_->removeItem(rawDataSeries_.at(i));

		plot_->insertItem(corrSum_, 0);
		plot_->setAxisScaleWaterfall(MPlot::Left, 0);
	}
}

void XRFDetailedDetectorView::onWaterfallSeparationChanged(double val)
{
	plot_->setAxisScaleWaterfall(MPlot::Left, val*getMaximumHeight(plot_->item(0)));
}

void XRFDetailedDetectorView::onComboBoxUpdate(int index)
{
	switch(index){

	case 0:
		detector_->setRefreshRate(XRFDetector::Passive);
		break;
	case 1:
		detector_->setRefreshRate(XRFDetector::Slow);
		break;
	case 2:
		detector_->setRefreshRate(XRFDetector::Fast);
		break;
	}
}

void XRFDetailedDetectorView::onUpdateRateUpdate(double val)
{
	// This is here to stop the VESPERS single element detector from hurting getting into an infinite signal/slot loop.  It only seems to be it.  If this problem goes away then this line can be removed.
	if (detector_->elements() == 1)
		disconnect(updateRate_, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxUpdate(int)));

	switch((int)val){

	case 0:
		updateRate_->setCurrentIndex(0);
		break;
	case 6:
		updateRate_->setCurrentIndex(1);
		break;
	case 8:
		updateRate_->setCurrentIndex(2);
		break;
	}

	// This is here to stop the VESPERS single element detector from hurting getting into an infinite signal/slot loop.  It only seems to be it.  If this problem goes away then this line can be removed.
	if (detector_->elements() == 1)
		connect(updateRate_, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxUpdate(int)));
}

void XRFDetailedDetectorView::setupPlot()
{
	// Create the plot window.
	view_ = new MPlotWidget;
	view_->enableAntiAliasing(true);

	// Create the plot and setup all the axes.
	plot_ = new MPlot;
	plot_->axisBottom()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisBottom()->setAxisName("Energy, eV");
	plot_->axisLeft()->setAxisNameFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setTickLabelFont(QFont("Helvetica", 6));
	plot_->axisLeft()->setAxisName("Counts");

	// Set the margins for the plot.
	plot_->setMarginLeft(10);
	plot_->setMarginBottom(15);
	plot_->setMarginRight(2);
	plot_->setMarginTop(2);

	MPlotSeriesBasic *series;

	for (int i = 0; i < detector_->elements(); i++){

		series = new MPlotSeriesBasic;
		series->setModel(new AMDataSourceSeriesData(detector_->spectrumDataSource(i)));
		series->setMarker(MPlotMarkerShape::None);
		series->setDescription(detector_->spectrumDataSource(i)->name());
		series->setLinePen(QPen(getColor(i+1)));
		rawDataSeries_ << series;
	}

	corrSum_ = new MPlotSeriesBasic;
	corrSum_->setModel(new AMDataSourceSeriesData(detector_->correctedSumDataSource()));
	corrSum_->setModel(new AMDataSourceSeriesData(detector_->spectrumDataSource(0)));
	corrSum_->setMarker(MPlotMarkerShape::None);
	corrSum_->setDescription(detector_->correctedSumDataSource()->name());
	corrSum_->setLinePen(QPen(getColor(0)));
	plot_->addItem(corrSum_);

	// Enable autoscaling of both axes.
	plot_->axisScaleLeft()->setAutoScaleEnabled();
	plot_->axisScaleBottom()->setAutoScaleEnabled();

	// Enable some convenient zoom tools.
	plot_->addTool(new MPlotDragZoomerTool());
	plot_->addTool(new MPlotWheelZoomerTool());
	view_->setPlot(plot_);
	view_->setMinimumHeight(450);

	// Set the number of ticks.  A balance between readability and being practical.
	plot_->axisBottom()->setTicks(3);
	plot_->axisTop()->setTicks(0);
	plot_->axisLeft()->setTicks(4);
	plot_->axisRight()->setTicks(0);

	// Set the autoscale constraints.
	plot_->axisScaleLeft()->setDataRangeConstraint(MPlotAxisRange(1, 1e20));
}

double XRFDetailedDetectorView::getMaximumHeight(MPlotItem *data)
{
	double max = 0;
	MPlotAbstractSeries *temp = qgraphicsitem_cast<MPlotAbstractSeries *>(data);

	if (!temp)
		return max;

	AMDataSourceSeriesData *model = (AMDataSourceSeriesData *)temp->model();

	for (int i = 0; i < model->count(); i++){

		if (max < model->y(i))
			max = model->y(i);
	}

	return max;
}

void XRFDetailedDetectorView::sortRegionsOfInterest()
{
	// Sort the real ROIs.
	detector_->sort();

	// Remove all the markers from the plot for now.
	for (int i = 0; i < markers_.size(); i++)
		plot_->removeItem(markers_.at(i));

	// Sort the list.
	double smallest = 0;

	for (int i = 0; i < markers_.size(); i++){

		smallest = i;

		for (int j = i; j < markers_.size(); j++)
			if (markers_.at(smallest)->center() > markers_.at(j)->center())
				smallest = j;

		if (i != smallest)
			markers_.swap(i, smallest);
	}

	// Put all the markers back in their new order.
	for (int i = 0; i < markers_.size(); i++){

		plot_->addItem(markers_.at(i));
		markers_.at(i)->setYAxisTarget(plot_->axisScale(MPlot::VerticalRelative));
	}
}

void XRFDetailedDetectorView::addRegionOfInterestMarker(AMROIInfo info)
{
	QString symbol = info.name().split(" ").first();
	QString line = GeneralUtilities::addGreek(info.name().split(" ").last());

	ROIPlotMarker *newMarker = new ROIPlotMarker(symbol+" "+line, info.energy(), info.low(), info.high());
	plot_->insertItem(newMarker);
	newMarker->setYAxisTarget(plot_->axisScale(MPlot::VerticalRelative));
	markers_ << newMarker;
}

void XRFDetailedDetectorView::removeRegionOfInterestMarker(AMROIInfo info)
{
	MPlotItem *removeMe = 0;
	ROIPlotMarker *temp;

	for (int i = 0; i < markers_.size(); i++){

		temp = markers_.at(i);

		if (info.name().compare(GeneralUtilities::removeGreek(temp->description())) == 0)
			removeMe = markers_.takeAt(i);
	}

	if (removeMe){

		plot_->removeItem(removeMe);
		delete removeMe;
	}
}

void XRFDetailedDetectorView::removeAllRegionsOfInterestMarkers()
{
	for (int i = 0; i < markers_.size(); i++){

		plot_->removeItem(markers_.at(i));
		delete markers_.at(i);
	}

	markers_.clear();
}

void XRFDetailedDetectorView::highlightMarkers(XRFElement *el)
{
	ROIPlotMarker *temp;

	for (int i = 0; i < markers_.size(); i++){

		temp = markers_.at(i);

		// Need to add the space because Potassium shows up in Ka1 (Symbol for Potassium is K).
		if (temp->description().contains(el->symbol()+" "))
			temp->setHighlighted(true);
		else
			temp->setHighlighted(false);
	}
}

void XRFDetailedDetectorView::showEmissionLines(XRFElement *el)
{
	// Remove all the old lines from the plot and clear the list.
	if (!lines_.isEmpty()){

		for (int i = 0; i < lines_.size(); i++){

			plot_->removeItem(lines_.at(i));
			delete lines_.at(i);
		}

		lines_.clear();
	}

	MPlotPoint *newLine;
	QString line;
	double lineEnergy;

	for (int i = 0; i < el->emissionLines().size(); i++){

		line = el->emissionLines().at(i).first;
		lineEnergy = el->lineEnergy(line);

		if ((lineEnergy <= maximumEnergy_ && lineEnergy >= minimumEnergy_)
			&& line.contains("1") != 0 && line.compare("-")){

			newLine = new MPlotPoint(QPointF(el->lineEnergy(line), 0));
			newLine->setMarker(MPlotMarkerShape::VerticalBeam, 1e6, QPen(getColor(line)), QBrush(getColor(line)));
			newLine->setDescription(line + ": " + QString::number(el->lineEnergy(line)) + " eV");
			if (isWaterfall_)
				plot_->insertItem(newLine, lines_.size()+detector_->elements());
			else
				plot_->insertItem(newLine, lines_.size()+1);
			lines_ << newLine;
		}
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
		return Qt::darkCyan;
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

#include <QFileDialog>
#include <QTextStream>

/// \todo get rid of this because it is a dirty hack.  Will become auto-export once it's available.
void XRFDetailedDetectorView::saveSpectra()
{
	QFileDialog saveDialog(this);
	saveDialog.setFileMode(QFileDialog::AnyFile);

	QString fileName = saveDialog.getSaveFileName(this,
													   tr("Save Spectra"),
													   "",
													   tr("Spectra Data (*.dat);;All Files (*)"));

	QFile file(fileName+".dat");

	if (!file.open(QFile::WriteOnly | QFile::Text)){
		QMessageBox::warning(this, tr("XRF Detector"),
							 tr("Cannot open file %1:\n%2")
							 .arg(file.fileName())
							 .arg(file.errorString()));
		return;
	}

	QTextStream out(&file);

	if (detector_->elements() == 1){

		out << "eV\tData\n";
		for (int i = 0; i < detector_->correctedSumDataSource()->size(0); i++)
			out << double(detector_->correctedSumDataSource()->axisValue(0, i)) << "\t" << int(detector_->correctedSumDataSource()->value(AMnDIndex(i))) << "\n";
	}
	else{

		out << "eV\traw1\traw2\traw3\traw4\tcorrected sum\n";
		for (int i = 0; i < detector_->correctedSumDataSource()->size(0); i++)
			out << double(detector_->spectrumDataSources().at(0)->axisValue(0, i)) << "\t" << int(detector_->spectrumDataSources().at(0)->value(AMnDIndex(i))) << "\t" << int(detector_->spectrumDataSources().at(1)->value(AMnDIndex(i))) << "\t" << int(detector_->spectrumDataSources().at(2)->value(AMnDIndex(i))) << "\t" << int(detector_->spectrumDataSources().at(3)->value(AMnDIndex(i))) << "\t" << int(detector_->correctedSumDataSource()->value(AMnDIndex(i))) << "\n";
	}

	file.close();
}

void XRFDetailedDetectorView::onExternalRegionsOfInterestChanged()
{
	removeAllRegionsOfInterestMarkers();
	for (int i = 0; i < detector_->roiInfoList()->count(); i++)
		addRegionOfInterestMarker(detector_->roiInfoList()->at(i));
}

// End detailed detector view
/////////////////////////////////////////////////////
