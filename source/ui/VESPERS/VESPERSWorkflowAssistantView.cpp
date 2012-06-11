/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSWorkflowAssistantView.h"

#include <QHBoxLayout>

VESPERSWorkflowAssistantView::VESPERSWorkflowAssistantView(VESPERSWorkflowAssistant *assistant, QWidget *parent)
	: QWidget(parent)
{
	assistant_ = assistant;
//	setVisible(false);
//	connect(assistant_, SIGNAL(workflowRunningChanged(bool)), this, SLOT(setVisible(bool)));

	progressBar_ = new QProgressBar;
	progressBar_->setFixedWidth(400);
	connect(assistant_, SIGNAL(progressChanged(double)), this, SLOT(onProgressChanged(double)));

	totalScans_ = new QSpinBox;
	totalScans_->setMinimum(1);
	totalScans_->setValue(1);
	connect(totalScans_, SIGNAL(valueChanged(int)), this, SLOT(onTotalScansChanged(int)));

	name_ = new QLabel;

	currentScan_ = new QLabel;
	connect(assistant_, SIGNAL(currentScanChanged(int)), this, SLOT(onCurrentScanChanged(int)));

	connect(assistant_, SIGNAL(configurationChanged()), this, SLOT(onScanConfigurationChanged()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addStretch();
	layout->addWidget(name_);
	layout->addSpacing(25);
	layout->addWidget(progressBar_);
	layout->addSpacing(25);
	layout->addWidget(currentScan_);
	layout->addWidget(totalScans_);
	layout->addStretch();

	setLayout(layout);
}

void VESPERSWorkflowAssistantView::onTotalScansChanged(int num)
{
	if (num != assistant_->totalScans() && num >= assistant_->currentScan())
		assistant_->setTotalScans(num);
}

void VESPERSWorkflowAssistantView::onProgressChanged(double progress)
{
	progressBar_->setValue(int(100*((progress+assistant_->currentScan()-1)/assistant_->totalScans())));
}

void VESPERSWorkflowAssistantView::onCurrentScanChanged(int index)
{
	currentScan_->setText(QString("%1 /").arg(index));
	progressBar_->setValue(int((index-1)*100/assistant_->totalScans()));
	totalScans_->setMinimum(index);
}

void VESPERSWorkflowAssistantView::onScanConfigurationChanged()
{
	name_->setText(assistant_->configName());
	progressBar_->setValue(0);
	totalScans_->setValue(assistant_->totalScans());
	totalScans_->setMinimum(1);
	currentScan_->setText(QString("%1 /").arg(assistant_->currentScan()));
}
