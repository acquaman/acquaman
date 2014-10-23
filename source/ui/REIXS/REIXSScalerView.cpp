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


#include "REIXSScalerView.h"

#include "beamline/REIXS/REIXSBeamline.h"

#include <QVBoxLayout>

REIXSScalerView::~REIXSScalerView(){}
REIXSScalerView::REIXSScalerView(QWidget *parent)
	: QWidget(parent)
{
	CLSSIS3820Scaler *scaler = REIXSBeamline::bl()->scaler();

	QVBoxLayout *layout = new QVBoxLayout;


	CLSSIS3820ScalerChannelView *view = new CLSSIS3820ScalerChannelView(scaler->channelAt(16));
	view->setEnableCheckBoxVisibility(false);
	view->setAmplifierViewFormat('g');
	view->setAmplifierViewPrecision(3);
	singleViews_ << view;
	view->setFixedHeight(55);
	connect(view, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onSR570ViewChanged(AMCurrentAmplifierView::ViewMode)));
	connect(view, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)));
	layout->addWidget(view);

	view = new CLSSIS3820ScalerChannelView(scaler->channelAt(18));
	view->setEnableCheckBoxVisibility(false);
	view->setAmplifierViewFormat('g');
	view->setAmplifierViewPrecision(3);
	singleViews_ << view;
	view->setFixedHeight(40);
	connect(view, SIGNAL(amplifierViewModeChanged(AMCurrentAmplifierView::ViewMode)), this, SLOT(onSR570ViewChanged(AMCurrentAmplifierView::ViewMode)));
	connect(view, SIGNAL(outputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)), this, SLOT(onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode)));
	layout->addWidget(view);


	setLayout(layout);
}

void REIXSScalerView::onSR570ViewChanged(AMCurrentAmplifierView::ViewMode mode)
{	
	foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

		channel->blockSignals(true);
		channel->setAmplifierViewMode(mode);
		channel->blockSignals(false);
	}
}

void REIXSScalerView::onOutputViewModeChanged(CLSSIS3820CompositeScalerChannelView::OutputViewMode mode)
{	
	foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

		channel->blockSignals(true);
		channel->setOutputViewMode((CLSSIS3820ScalerChannelView::OutputViewMode)mode);
		channel->blockSignals(false);
	}
}

void REIXSScalerView::onOutputViewModeChanged(CLSSIS3820ScalerChannelView::OutputViewMode mode)
{
	foreach (CLSSIS3820ScalerChannelView *channel, singleViews_){

		channel->blockSignals(true);
		channel->setOutputViewMode(mode);
		channel->blockSignals(false);
	}
}

