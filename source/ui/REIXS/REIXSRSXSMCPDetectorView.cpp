/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "REIXSRSXSMCPDetectorView.h"
#include "dataman/datasource/REIXS/REIXSRSXSMCPDataSource.h"
#include "beamline/AMPVControl.h"

#include "QBoxLayout"

#include <cmath>
#include "dataman/datasource/AMDataSourceImageData.h"

#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotRectangle.h"

#include "ui/dataman/AMImagePropertyEditor.h"

REIXSRSXSMCPDetectorView::~REIXSRSXSMCPDetectorView(){}

REIXSRSXSMCPDetectorView::REIXSRSXSMCPDetectorView(QWidget *parent) :
	QWidget(parent)
{
	imageControl_ = new AMReadOnlyPVControl("Memory Buffer", "PDTR0000-01:zippedBuffer");
	dataSource_ = new REIXSRSXSMCPDataSource("MCP Image" , imageControl_);

	// create UI elements
	imageView_ = new MPlotWidget();
	imagePlot_ = new MPlot();
	imageView_->setPlot(imagePlot_);
	image_ = new MPlotImageBasic();

	colorMapEditor_ = 0;

	topLeftX1_ = new AMSinglePVControl("Top Left X 1", "PDTR0000-01:setLeftTopX");
	topLeftY1_ = new AMSinglePVControl("Top Left Y 1", "PDTR0000-01:setLeftTopY");
	bottomRightX1_ = new AMSinglePVControl("Bottom Right X 1", "PDTR0000-01:setRightBottomX");
	bottomRightY1_ = new AMSinglePVControl("Bottom Right Y 1", "PDTR0000-01:setRightBottomY");



	//Rectangle Displays currnet region 1 rectangle
	QColor white(Qt::white);
	white.setAlphaF(0.4);
	rangeRectangleY1_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangleY1_->setSelectable(false);
	rangeRectangleY1_->setIgnoreWhenAutoScaling(true);
	rangeRectangleY1_->setZValue(3000);
	imagePlot_->addItem(rangeRectangleY1_);

	getTopLeftX1_ = new AMReadOnlyPVControl("Top Left X 1 readback", "PDTR0000-01:getLeftTopX");
	getTopLeftY1_ = new AMReadOnlyPVControl("Top Left Y 1 readback", "PDTR0000-01:getLeftTopY");
	getBottomRightX1_ = new AMReadOnlyPVControl("Bottom Right X 1 readback", "PDTR0000-01:getRightBottomX");
	getBottomRightY1_ = new AMReadOnlyPVControl("Bottom Right Y 1 readback", "PDTR0000-01:getRightBottomY");


	// configure UI elements

	image_->setDescription("RSXS Detector Image");
	AMDataSourceImageData *model = new AMDataSourceImageData;
	model->setDataSource(dataSource_);
	image_->setModel(model, true);
	image_->setDescription("RSXS Detector Image");

	imagePlot_->axisScaleLeft()->setAutoScaleEnabled();
	imagePlot_->axisScaleBottom()->setAutoScaleEnabled();
	imagePlot_->axisScaleLeft()->setPadding(1);
	imagePlot_->axisScaleBottom()->setPadding(1);

//	imagePlot_->addTool(new MPlotDragZoomerTool);
//	imagePlot_->addTool(new MPlotWheelZoomerTool);

	adjustColorMapButton_ = new QToolButton();
	adjustColorMapButton_->setText("Adjust colors...");

	MPlotDataPositionTool *positionTool = new MPlotDataPositionTool(true);
	imagePlot_->addTool(positionTool);
	positionTool->setDataPositionIndicator(imagePlot_->axisScaleBottom(), imagePlot_->axisScaleLeft());

	//connect(imagePlot_->signalSource(), SIGNAL(dataPositionChanged(QPointF)), this, SLOT(onDataPositionChanged(QPointF)));
	connect(imagePlot_->signalSource(), SIGNAL(selectedDataRectChanged(QRectF)), this, SLOT(onSelectedDataRectChanged(QRectF)));

	connect(adjustColorMapButton_, SIGNAL(clicked()), this, SLOT(onAdjustColorMapButtonClicked()));

	connect(getTopLeftX1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getTopLeftY1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getBottomRightX1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getBottomRightY1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));



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

	MPlotColorMap colorMap(MPlotColorMap::Jet);
	colorMap.setContrast(2.1);
	colorMap.setBrightness(0.08);
	colorMap.setGamma(1.0);
	image_->setColorMap(colorMap);
	image_->setZValue(1);
	imagePlot_->addItem(image_);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(imageView_);

	layout->addWidget(adjustColorMapButton_);


	setLayout(layout);


}

void REIXSRSXSMCPDetectorView::onDataPositionChanged(QPointF point)
{
	//qDebug() << point;
}

void REIXSRSXSMCPDetectorView::onSelectedDataRectChanged(QRectF rect)
{
	if(rect.size() != QSize(0,0))
	{
		int topLeftX1 = qBound(0, qRound(rect.topLeft().x()),127);
		int topLeftY1 = qBound(0, qRound(rect.topLeft().y()),127);
		int bottomRightX1 = qBound(0, qRound(rect.bottomRight().x()),127);
		int bottomRightY1 = qBound(0, qRound(rect.bottomRight().y()),127);

		topLeftX1_->move(topLeftX1);
		topLeftY1_->move(topLeftY1);
		bottomRightX1_->move(bottomRightX1);
		bottomRightY1_ ->move(bottomRightY1);
	}
}


void REIXSRSXSMCPDetectorView::onAdjustColorMapButtonClicked()
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

void REIXSRSXSMCPDetectorView::onColorMapChanged(const MPlotColorMap &map)
{
	image_->setColorMap(map);
}


void REIXSRSXSMCPDetectorView::drawRegions()
{

	rangeRectangleY1_->setXAxisTarget(imagePlot_->axisScaleBottom());	// note: does nothing if already correct
	rangeRectangleY1_->setYAxisTarget(imagePlot_->axisScaleLeft());	// note: does nothing if already correct


	rangeRectangleY1_->setRect(QRectF(QPoint(getTopLeftX1_->value(),getBottomRightY1_->value()),  QPoint(getBottomRightX1_->value(),  getTopLeftY1_->value())));
	rangeRectangleY1_->setVisible(true);
}



// REIXSRSXSMCPDetectorView::~REIXSRSXSMCPDetectorView(){}
////REIXSRSXSMCPDetectorView::REIXSRSXSMCPDetectorView(REIXSRSXSMCPDetectorPre2013* detector, QWidget *parent) :
//REIXSRSXSMCPDetectorView::REIXSRSXSMCPDetectorView(REIXSRSXSMCPDetector* detector, QWidget *parent) :
//	QWidget(parent)
//{
//	detector_ = detector;

//	// create UI elementstrue
//	imageView_ = new MPlotWidget();
//	imagePlot_ = new MPlot();
//	imageView_->setPlot(imagePlot_);
//	image_ = new MPlotImageBasic();

//	clearButton_ = new QPushButton("Clear All Counts");

//	imageSelector_ = new QComboBox();
//	averagingPeriodControl_ = new AMControlEditor(detector_->averagingPeriodControl());

//	persistDurationControl_ = new AMControlEditor(detector_->persistDurationControl());

//	countsPerSecondIndicator_ = new QLabel();
//	countsPerSecondIndicator_->setFixedWidth(70);
//	countsPerSecondIndicator_->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//	countsPerSecondBar_ = new QProgressBar();

//	colorMapEditor_ = 0;

//	///////////////////////

//	// configure UI elements

//	image_->setDescription("XES Detector Image");

//	imagePlot_->axisScaleLeft()->setAutoScaleEnabled();
//	imagePlot_->axisScaleBottom()->setAutoScaleEnabled();
//	imagePlot_->axisScaleLeft()->setPadding(1);
//	imagePlot_->axisScaleBottom()->setPadding(1);

//	imagePlot_->addTool(new MPlotDragZoomerTool);
//	imagePlot_->addTool(new MPlotWheelZoomerTool);

//	imagePlot_->setMarginBottom(10);
//	imagePlot_->setMarginLeft(10);
//	imagePlot_->setMarginRight(5);
//	imagePlot_->setMarginTop(5);


//	imagePlot_->plotArea()->setBrush(QBrush(QColor(Qt::white)));
//	imagePlot_->axisRight()->setTicks(4);
//	imagePlot_->axisBottom()->setTicks(4);
//	imagePlot_->axisLeft()->showGrid(false);
//	imagePlot_->axisBottom()->showAxisName(false);
//	imagePlot_->axisLeft()->showAxisName(false);

//	MPlotColorMap colorMap(MPlotColorMap::Bone);
//	colorMap.setContrast(2.1);
//	colorMap.setBrightness(0.08);
//	colorMap.setGamma(1.0);
//	image_->setColorMap(colorMap);
//	imagePlot_->addItem(image_);

//	imageSelector_->addItem("Realtime Image");
//	imageSelector_->addItem("Accumulated Image");
//	imageSelector_->addItem("None");

//	countsPerSecondBar_->setOrientation(Qt::Vertical);
//	countsPerSecondBar_->setRange(0, 600);
//	countsPerSecondBar_->setValue(0);
//	countsPerSecondBar_->setFormat(QString());

//	adjustColorMapButton_ = new QToolButton();
//	adjustColorMapButton_->setText("Adjust colors...");

//	///////////////////////

//	// create layout
//	QVBoxLayout* vl = new QVBoxLayout();
//	QHBoxLayout* hl1 = new QHBoxLayout();
//	QHBoxLayout* hl2 = new QHBoxLayout();
//	QHBoxLayout* hl3 = new QHBoxLayout();
//	QVBoxLayout* vl1 = new QVBoxLayout();

//	hl1->addWidget(imageView_);
//	vl1->addWidget(countsPerSecondBar_);
//	vl1->addWidget(countsPerSecondIndicator_);
//	hl1->addLayout(vl1);

//	hl2->addWidget(adjustColorMapButton_);
//	hl2->addStretch();
//	hl2->addWidget(imageSelector_);
//	hl2->addWidget(clearButton_);

//	hl3->addWidget(new QLabel("Persist:"));
//	hl3->addWidget(persistDurationControl_);
//	hl3->addWidget(new QLabel("Averaging Period:"));
//	hl3->addWidget(averagingPeriodControl_);

//	vl->addLayout(hl1);
//	vl->addLayout(hl2);
//	vl->addLayout(hl3);

//	setLayout(vl);

//	//////////////////////

//	// hookup signals:
//	connect(clearButton_, SIGNAL(clicked()), detector_, SLOT(clear()));
//	connect(imageSelector_, SIGNAL(currentIndexChanged(int)), this, SLOT(onImageSelectorChanged(int)));
//	connect(detector_, SIGNAL(countsPerSecondChanged(double)), this, SLOT(onCountsPerSecondChanged(double)));

//	connect(adjustColorMapButton_, SIGNAL(clicked()), this, SLOT(onAdjustColorMapButtonClicked()));


//	//////////////////////////

//	// connect the real-time data source to the plot image.
//	onCountsPerSecondChanged(0);
//	onImageSelectorChanged(0);

//}

//void REIXSRSXSMCPDetectorView::onCountsPerSecondChanged(double countsPerSecond) {


//	countsPerSecondIndicator_->setText(QString("%1").arg(countsPerSecond, 0, 'e', 1));

//	// log(0) is undefined...
//	if(countsPerSecond == 0)
//		countsPerSecond = 1;

//	countsPerSecondBar_->setValue(int(log10(countsPerSecond)*100));	// integer scale goes up to 600.  Highest count rate we'll see is 1e6.
//}


//void REIXSRSXSMCPDetectorView::onImageSelectorChanged(int index) {

//	if(index == 0) {
//		AMDataSourceImageData *model = new AMDataSourceImageData;
//		model->setDataSource(detector_->instantaneousImage());
//		image_->setModel(model, true);
//		image_->setDescription("Real-time XES Detector Image");
//	}
//	else if(index == 1) {
//		AMDataSourceImageData *model = new AMDataSourceImageData;
//		model->setDataSource(detector_->image());
//		image_->setModel(model, true);
//		image_->setDescription("Accumulated XES Detector Image");
//	}
//	else {
//		image_->setModel(0, true);
//		image_->setDescription(QString());
//	}

//}

//void REIXSRSXSMCPDetectorView::onAdjustColorMapButtonClicked()
//{
//	if(!colorMapEditor_) {
//		colorMapEditor_ = new QDialog(adjustColorMapButton_);
//		colorMapEditor_->setAttribute(Qt::WA_TranslucentBackground);
//		QVBoxLayout* vl = new QVBoxLayout();
//		vl->setContentsMargins(0,0,0,0);
//		AMImagePropertyEditor* ipe = new AMImagePropertyEditor(image_->colorMap());
//		vl->addWidget(ipe);
//		colorMapEditor_->setLayout(vl);

//		connect(ipe, SIGNAL(colorMapChanged(MPlotColorMap)), this, SLOT(onColorMapChanged(MPlotColorMap)));
//	}

//	colorMapEditor_->show();
//}

//void REIXSRSXSMCPDetectorView::onColorMapChanged(const MPlotColorMap &map)
//{
//	image_->setColorMap(map);
//}

