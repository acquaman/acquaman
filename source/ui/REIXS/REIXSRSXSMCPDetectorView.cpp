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
#include <QToolBar>
#include "dataman/datasource/AMDataSourceImageData.h"

#include "MPlot/MPlotTools.h"
#include "MPlot/MPlotRectangle.h"

#include "ui/dataman/AMImagePropertyEditor.h"

REIXSRSXSMCPDetectorView::~REIXSRSXSMCPDetectorView(){}

REIXSRSXSMCPDetectorView::REIXSRSXSMCPDetectorView(QWidget *parent) :
	QWidget(parent)
{
	imageControl_ = new AMReadOnlyPVControl("Memory Buffer", "PDTR0000-01:zippedBuffer", this);
	dataSource_ = new REIXSRSXSMCPDataSource("MCP Image" , imageControl_);
	waysToGetBuffer_ = new AMSinglePVControl("Way to get buffer","PDTR0000-01:waysToGetBuffer", this);
	downloadMemory_ = new AMSinglePVControl("Download Memory", "PDTR0000-01:downloadMemory", this);
	startAccumulation_ = new AMSinglePVControl("Start Accumulation", "PDTR0000-01:startAccumulation", this);


	// create UI elements
	imageView_ = new MPlotWidget();
	imagePlot_ = new MPlot();
	imageView_->setPlot(imagePlot_);
	image_ = new MPlotImageBasic();

	colorMapEditor_ = 0;

	topLeftX1_ = new AMSinglePVControl("Top Left X 1", "PDTR0000-01:setLeftTopX1", this);
	topLeftY1_ = new AMSinglePVControl("Top Left Y 1", "PDTR0000-01:setLeftTopY1", this);
	bottomRightX1_ = new AMSinglePVControl("Bottom Right X 1", "PDTR0000-01:setRightBottomX1", this);
	bottomRightY1_ = new AMSinglePVControl("Bottom Right Y 1", "PDTR0000-01:setRightBottomY1", this);
	ROI1Counts_ = new AMReadOnlyPVControl("ROI 1 Counts", "PDTR0000-01:getRoi1Counts", this);


	topLeftX2_ = new AMSinglePVControl("Top Left X 2", "PDTR0000-01:setLeftTopX2", this);
	topLeftY2_ = new AMSinglePVControl("Top Left Y 2", "PDTR0000-01:setLeftTopY2", this);
	bottomRightX2_ = new AMSinglePVControl("Bottom Right X 2", "PDTR0000-01:setRightBottomX2", this);
	bottomRightY2_ = new AMSinglePVControl("Bottom Right Y 2", "PDTR0000-01:setRightBottomY2", this);
	ROI2Counts_ = new AMReadOnlyPVControl("ROI 2 Counts", "PDTR0000-01:getRoi2Counts", this);

	totalCounts_ = new AMReadOnlyPVControl("Total Counts", "PDTR0000-01:getTotalCounts", this);




	//Rectangle Displays currnet region 1 rectangle
	QColor white(Qt::white);
	white.setAlphaF(0.4);
	rangeRectangle1_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(white));
	rangeRectangle1_->setSelectable(false);
	rangeRectangle1_->setIgnoreWhenAutoScaling(true);
	rangeRectangle1_->setZValue(3000);
	imagePlot_->addItem(rangeRectangle1_);

	//Rectangle Displays currnet region 2 rectangle
	QColor black(Qt::black);
	black.setAlphaF(0.4);
	rangeRectangle2_ = new MPlotRectangle(QRectF(0,0,10,10), Qt::NoPen, QBrush(black));
	rangeRectangle2_->setSelectable(false);
	rangeRectangle2_->setIgnoreWhenAutoScaling(true);
	rangeRectangle2_->setZValue(3000);
	imagePlot_->addItem(rangeRectangle2_);



	getTopLeftX1_ = new AMReadOnlyPVControl("Top Left X 1 readback", "PDTR0000-01:getLeftTopX1", this);
	getTopLeftY1_ = new AMReadOnlyPVControl("Top Left Y 1 readback", "PDTR0000-01:getLeftTopY1", this);
	getBottomRightX1_ = new AMReadOnlyPVControl("Bottom Right X 1 readback", "PDTR0000-01:getRightBottomX1", this);
	getBottomRightY1_ = new AMReadOnlyPVControl("Bottom Right Y 1 readback", "PDTR0000-01:getRightBottomY1", this);

	getTopLeftX2_ = new AMReadOnlyPVControl("Top Left X 2 readback", "PDTR0000-01:getLeftTopX2", this);
	getTopLeftY2_ = new AMReadOnlyPVControl("Top Left Y 2 readback", "PDTR0000-01:getLeftTopY2", this);
	getBottomRightX2_ = new AMReadOnlyPVControl("Bottom Right X 2 readback", "PDTR0000-01:getRightBottomX2", this);
	getBottomRightY2_ = new AMReadOnlyPVControl("Bottom Right Y 2 readback", "PDTR0000-01:getRightBottomY2", this);



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

	adjustColorMapButton_ = new QPushButton("Adjust colors...");
	updateButton_ = new QPushButton("Update Image");
	acquireButton_ = new QPushButton("Acquire Image");

	ROI1CountsLabel_ = new QLabel(QString(" ROI #1 Counts: %1 ").arg(ROI1Counts_->value()));
	ROI1CountsLabel_->setStyleSheet("QLabel { background-color : white; color : black; }");

	ROI2CountsLabel_ = new QLabel(QString(" ROI #2 Counts: %1 ").arg(ROI2Counts_->value()));
	ROI2CountsLabel_->setStyleSheet("QLabel { background-color : black; color : white; }");

	totalCountsLabel_ = new QLabel(QString(" Total Counts: %1 ").arg(totalCounts_->value()));


	QVBoxLayout *ROIRadioButtons = new QVBoxLayout;
	ROI1Select_ = new QRadioButton("Set ROI #&1");
	ROI2Select_ = new QRadioButton("Set ROI #&2");
	ROI1Select_->setChecked(true);
	ROIRadioButtons->addWidget(ROI1Select_);
	ROIRadioButtons->addWidget(ROI2Select_);

	positionTool_ = new MPlotDataPositionTool(true);
	imagePlot_->addTool(positionTool_);
	positionTool_->setDataPositionIndicator(imagePlot_->axisScaleBottom(), imagePlot_->axisScaleLeft());

	connect(imagePlot_->signalSource(), SIGNAL(selectedDataRectChanged(QRectF)), this, SLOT(onSelectedDataRectChanged(QRectF)));

	connect(adjustColorMapButton_, SIGNAL(clicked()), this, SLOT(onAdjustColorMapButtonClicked()));
	connect(updateButton_, SIGNAL(clicked()), this, SLOT(onUpdateButtonClicked()));
	connect(acquireButton_, SIGNAL(clicked()), this, SLOT(onAcquireButtonClicked()));



	connect(getTopLeftX1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getTopLeftY1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getBottomRightX1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getBottomRightY1_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getTopLeftX2_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getTopLeftY2_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getBottomRightX2_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));
	connect(getBottomRightY2_, SIGNAL(valueChanged(double)), this, SLOT(drawRegions()));


	connect(ROI1Counts_, SIGNAL(valueChanged(double)), SLOT(updateCounts()));
	connect(ROI2Counts_, SIGNAL(valueChanged(double)), SLOT(updateCounts()));
	connect(totalCounts_, SIGNAL(valueChanged(double)), SLOT(updateCounts()));





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
	QHBoxLayout* bottomLayout = new QHBoxLayout();

	QVBoxLayout *ROICountslayout = new QVBoxLayout;
	ROICountslayout->addWidget(ROI1CountsLabel_);
	ROICountslayout->addWidget(ROI2CountsLabel_);

	layout->addWidget(imageView_);

	bottomLayout->addWidget(adjustColorMapButton_);
	bottomLayout->addStretch();
	bottomLayout->addLayout(ROICountslayout);
	bottomLayout->addLayout(ROIRadioButtons);
	bottomLayout->addWidget(totalCountsLabel_);
	bottomLayout->addStretch();
	bottomLayout->addWidget(acquireButton_);
	bottomLayout->addWidget(updateButton_);

	layout->addLayout(bottomLayout);




	setLayout(layout);


}

void REIXSRSXSMCPDetectorView::onSelectedDataRectChanged(QRectF rect)
{
	if(rect.size() != QSize(0,0))
	{


		int topLeftX = qBound(0, qRound(rect.topLeft().x()),127);
		int topLeftY = qBound(0, qRound(rect.topLeft().y()),127);
		int bottomRightX = qBound(0, qRound(rect.bottomRight().x()),127);
		int bottomRightY = qBound(0, qRound(rect.bottomRight().y()),127);

		if(ROI1Select_->isChecked())
		{
		topLeftX1_->move(topLeftX);
		bottomRightX1_->move(bottomRightX);
		topLeftY1_->move(bottomRightY);
		bottomRightY1_ ->move(topLeftY);
		}
		else if(ROI2Select_->isChecked())
		{
		topLeftX2_->move(topLeftX);
		bottomRightX2_->move(bottomRightX);
		topLeftY2_->move(bottomRightY);
		bottomRightY2_ ->move(topLeftY);
		}

	}
}

void REIXSRSXSMCPDetectorView::onAcquireButtonClicked()
{
	startAccumulation_->move(1);
}

void REIXSRSXSMCPDetectorView::onUpdateButtonClicked()
{

	waysToGetBuffer_->move(1);
	downloadMemory_->move(1);
}

void REIXSRSXSMCPDetectorView::updateCounts()
{
	ROI1CountsLabel_->setText(QString(" ROI #1 Counts: %1 ").arg(ROI1Counts_->value()));
	ROI2CountsLabel_->setText(QString(" ROI #2 Counts: %1 ").arg(ROI2Counts_->value()));
	totalCountsLabel_->setText(QString(" Total Counts: %1 ").arg(totalCounts_->value()));

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

	rangeRectangle1_->setXAxisTarget(imagePlot_->axisScaleBottom());	// note: does nothing if already correct
	rangeRectangle1_->setYAxisTarget(imagePlot_->axisScaleLeft());	// note: does nothing if already correct

	rangeRectangle1_->setRect(QRectF(QPoint(getTopLeftX1_->value(),getTopLeftY1_->value()),  QPoint(getBottomRightX1_->value(),  getBottomRightY1_->value())));

	rangeRectangle1_->setVisible(true);



	rangeRectangle2_->setXAxisTarget(imagePlot_->axisScaleBottom());	// note: does nothing if already correct
	rangeRectangle2_->setYAxisTarget(imagePlot_->axisScaleLeft());	// note: does nothing if already correct

	rangeRectangle2_->setRect(QRectF(QPoint(getTopLeftX2_->value(),getTopLeftY2_->value()),  QPoint(getBottomRightX2_->value(),  getBottomRightY2_->value())));

	rangeRectangle2_->setVisible(true);



}



