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


#include "AMScanActionEditor.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStringBuilder>

#include "acquaman/AMScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"


AMScanActionEditor::AMScanActionEditor(AMScanActionInfo *info, QWidget *parent)
	: QFrame(parent)
{
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	configView_ = 0;

	scanName_ = new QLabel(QString("%1 : %2").arg(info_->configuration()->detailedDescription()).arg(info_->configuration()->name()));
	connect(info_, SIGNAL(infoChanged()), this, SLOT(onScanInfoChanged()));

	QPushButton *configViewButton = new QPushButton(QIcon(":/32x32/hammer-wrench.png"), "Show Configuration");
	connect(configViewButton, SIGNAL(clicked()), this, SLOT(onScanConfigurationViewRequested()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(scanName_);
	layout->addStretch();
	layout->addWidget(configViewButton);
	layout->setContentsMargins(0,0,0,0);

	setLayout(layout);
}

AMScanActionEditor::~AMScanActionEditor()
{
	if (configView_){

		configView_->deleteLater();
		configView_ = 0;
	}
}

void AMScanActionEditor::onScanConfigurationViewRequested()
{
	if (configView_ == 0)
		configView_ = info_->configuration()->createView();

	configView_->show();
	configView_->raise();
}

void AMScanActionEditor::onScanInfoChanged()
{
	scanName_->setText(QString("%1 : %2").arg(info_->configuration()->detailedDescription()).arg(info_->configuration()->name()));
}
