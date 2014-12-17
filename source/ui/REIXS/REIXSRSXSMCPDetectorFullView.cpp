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

#include "REIXSRSXSMCPDetectorFullView.h"
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

REIXSRSXSMCPDetectorFullView::~REIXSRSXSMCPDetectorFullView(){}

REIXSRSXSMCPDetectorFullView::REIXSRSXSMCPDetectorFullView(QWidget *parent) :
	QWidget(parent)
{
	imageControl_ = new AMReadOnlyPVControl("Memory Buffer", "PDTR0000-01:memoryBuffer", this);
	dataSource_ = new REIXSRSXSMCPDataSource("MCP Image" , imageControl_,256,256);
	waysToGetBuffer_ = new AMSinglePVControl("Way to get buffer","PDTR0000-01:waysToGetBuffer", this);
	downloadMemory_ = new AMSinglePVControl("Download Memory", "PDTR0000-01:downloadMemory", this);
	startAccumulation_ = new AMSinglePVControl("Start Accumulation", "PDTR0000-01:startAccumulation", this);


	// create UI elements
	imageView_ = new MPlotWidget();
	imagePlot_ = new MPlot();
	imageView_->setPlot(imagePlot_);
	image_ = new MPlotImageBasic();

	colorMapEditor_ = 0;

	// configure UI elements

	AMDataSourceImageData *model = new AMDataSourceImageData;
	model->setDataSource(dataSource_);
	image_->setModel(model, true);
	image_->setDescription("RSXS Full Detector Image");

	imagePlot_->axisScaleLeft()->setAutoScaleEnabled();
	imagePlot_->axisScaleBottom()->setAutoScaleEnabled();
	imagePlot_->axisScaleLeft()->setPadding(1);
	imagePlot_->axisScaleBottom()->setPadding(1);

	adjustColorMapButton_ = new QPushButton("Adjust colors...");
	updateButton_ = new QPushButton("Update Image");
	acquireButton_ = new QPushButton("Acquire Image");


	connect(adjustColorMapButton_, SIGNAL(clicked()), this, SLOT(onAdjustColorMapButtonClicked()));
	connect(updateButton_, SIGNAL(clicked()), this, SLOT(onUpdateButtonClicked()));
	connect(acquireButton_, SIGNAL(clicked()), this, SLOT(onAcquireButtonClicked()));


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

	layout->addWidget(imageView_);

	bottomLayout->addWidget(adjustColorMapButton_);
	bottomLayout->addStretch();
	bottomLayout->addWidget(acquireButton_);
	bottomLayout->addWidget(updateButton_);

	layout->addLayout(bottomLayout);




	setLayout(layout);


}

void REIXSRSXSMCPDetectorFullView::onAcquireButtonClicked()
{
	startAccumulation_->move(1);
}

void REIXSRSXSMCPDetectorFullView::onUpdateButtonClicked()
{

	waysToGetBuffer_->move(0);
	downloadMemory_->move(1);
}

void REIXSRSXSMCPDetectorFullView::onAdjustColorMapButtonClicked()
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

void REIXSRSXSMCPDetectorFullView::onColorMapChanged(const MPlotColorMap &map)
{
	image_->setColorMap(map);
}

