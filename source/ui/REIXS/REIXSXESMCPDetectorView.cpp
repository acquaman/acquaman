#include "REIXSXESMCPDetectorView.h"

#include "acquaman/REIXS/REIXSXESMCPDetector.h"	/// \todo Move detector into beamline

#include "QBoxLayout"

#include <cmath>
#include "dataman/AMDataSourceImageData.h"

#include "MPlot/MPlotTools.h"
#include "ui/AMImagePropertyEditor.h"

REIXSXESMCPDetectorView::REIXSXESMCPDetectorView(REIXSXESMCPDetector* detector, QWidget *parent) :
	QWidget(parent)
{
	detector_ = detector;

	// create UI elements
	imageView_ = new MPlotWidget();
	imagePlot_ = new MPlot();
	imageView_->setPlot(imagePlot_);
	image_ = new MPlotImageBasic();

	clearButton_ = new QPushButton("Clear All Counts");

	imageSelector_ = new QComboBox();
	averagingPeriodControl_ = new AMBasicControlEditor(detector_->averagingPeriodControl());

	persistDurationControl_ = new AMBasicControlEditor(detector_->persistDurationControl());

	orientationControl_ = new AMBasicControlEditor(detector_->orientationControl());

	countsPerSecondIndicator_ = new QLabel();
	countsPerSecondBar_ = new QProgressBar();

	colorMapEditor_ = 0;

	///////////////////////

	// configure UI elements

	image_->setDescription("XES Detector Image");

	imagePlot_->axisScaleLeft()->setAutoScaleEnabled();
	imagePlot_->axisScaleBottom()->setAutoScaleEnabled();
	imagePlot_->axisScaleLeft()->setPadding(1);
	imagePlot_->axisScaleBottom()->setPadding(1);

	imagePlot_->addTool(new MPlotDragZoomerTool);
	imagePlot_->addTool(new MPlotWheelZoomerTool);

	// imagePlot_->setXDataRange(0, 1023);
	// imagePlot_->setYDataRangeLeft(0, 63);
	imagePlot_->setMarginBottom(10);
	imagePlot_->setMarginLeft(10);
	imagePlot_->setMarginRight(5);
	imagePlot_->setMarginTop(5);


	imagePlot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
	imagePlot_->axisRight()->setTicks(4);
	imagePlot_->axisBottom()->setTicks(4);
	imagePlot_->axisLeft()->showGrid(false);
	imagePlot_->axisBottom()->showAxisName(false);
	imagePlot_->axisLeft()->showAxisName(false);

	MPlotColorMap colorMap(MPlotColorMap::Bone);
	colorMap.setContrast(2.1);
	colorMap.setBrightness(0.08);
	colorMap.setGamma(0.7);
	image_->setColorMap(colorMap);
	imagePlot_->addItem(image_);

	imageSelector_->addItem("Realtime Image");
	imageSelector_->addItem("Accumulated Image");
	imageSelector_->addItem("None");

	countsPerSecondBar_->setOrientation(Qt::Vertical);
	countsPerSecondBar_->setRange(0, 600);
	countsPerSecondBar_->setValue(0);

	adjustColorMapButton_ = new QToolButton();
	adjustColorMapButton_->setText("Adjust colors...");

	///////////////////////

	// create layout
	QVBoxLayout* vl = new QVBoxLayout();
	QHBoxLayout* hl1 = new QHBoxLayout();
	QHBoxLayout* hl2 = new QHBoxLayout();
	QHBoxLayout* hl3 = new QHBoxLayout();
	QVBoxLayout* vl1 = new QVBoxLayout();

	hl1->addWidget(imageView_);
	vl1->addWidget(countsPerSecondBar_);
	vl1->addWidget(countsPerSecondIndicator_);
	hl1->addLayout(vl1);

	hl2->addWidget(adjustColorMapButton_);
	hl2->addStretch();
	hl2->addWidget(imageSelector_);
	hl2->addWidget(clearButton_);

	hl3->addWidget(new QLabel("Orientation:"));
	hl3->addWidget(orientationControl_);
	hl3->addWidget(new QLabel("Persist:"));
	hl3->addWidget(persistDurationControl_);
	hl3->addWidget(new QLabel("Averaging Period:"));
	hl3->addWidget(averagingPeriodControl_);

	vl->addLayout(hl1);
	vl->addLayout(hl2);
	vl->addLayout(hl3);

	setLayout(vl);

	//////////////////////

	// hookup signals:
	connect(clearButton_, SIGNAL(clicked()), detector_, SLOT(clearImage()));
	connect(imageSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onImageSelectorChanged(int)));
	connect(detector_, SIGNAL(countsPerSecondChanged(double)), this, SLOT(onCountsPerSecondChanged(double)));

	connect(adjustColorMapButton_, SIGNAL(clicked()), this, SLOT(onAdjustColorMapButtonClicked()));


	//////////////////////////

	// connect the real-time data source to the plot image.
	onCountsPerSecondChanged(0);
	onImageSelectorChanged(0);

}

#include <QDebug>
void REIXSXESMCPDetectorView::onCountsPerSecondChanged(double countsPerSecond) {


	countsPerSecondIndicator_->setText(QString("%1").arg(countsPerSecond, 5, 'e', 1));

	// log(0) is undefined...
	if(countsPerSecond == 0)
		countsPerSecond = 1;

	countsPerSecondBar_->setValue(log10(countsPerSecond)*100);	// integer scale goes up to 600.  Highest count rate we'll see is 1e6.
}


void REIXSXESMCPDetectorView::onImageSelectorChanged(int index) {

	if(index == 0) {
		image_->setModel(new AMDataSourceImageData(detector_->instantaneousImage()), true);
		image_->setDescription("Real-time XES Detector Image");
	}
	else if(index == 1) {
		image_->setModel(new AMDataSourceImageData(detector_->image()), true);
		image_->setDescription("Accumulated XES Detector Image");
	}
	else {
		image_->setModel(0, true);
		image_->setDescription(QString());
	}

}

void REIXSXESMCPDetectorView::onAdjustColorMapButtonClicked()
{
	if(!colorMapEditor_) {
		colorMapEditor_ = new QDialog(adjustColorMapButton_);
		colorMapEditor_->setAttribute(Qt::WA_TranslucentBackground);
		QVBoxLayout* vl = new QVBoxLayout();
		vl->setContentsMargins(0,0,0,0);
		AMImagePropertyEditor* ipe = new AMImagePropertyEditor(image_->colorMap());
		vl->addWidget(ipe);
		colorMapEditor_->setLayout(vl);

		connect(ipe, SIGNAL(colorMapChanged(MPlotColorMap)), this, SLOT(onColorMapChanged(MPlotColorMap)));
	}

	colorMapEditor_->show();
}

void REIXSXESMCPDetectorView::onColorMapChanged(const MPlotColorMap &map)
{
	image_->setColorMap(map);
}

