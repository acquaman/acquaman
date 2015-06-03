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


#include "BioXASSidePersistentView.h"
#include "ui/BioXAS/BioXASSIS3820ScalerChannelsView.h"
#include "beamline/BioXAS/BioXASSideBeamline.h"
#include "ui/beamline/AMControlEditor.h"

#include <QComboBox>

BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
	QWidget(parent)
{
	// Energy control editor.

	energyControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->energyControl());
	energyControlEditor_->setTitle("Mono Energy");
	energyControlEditor_->setControlFormat('f', 2);

	// Region control editor.

	regionControlEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(BioXASSideBeamline::bioXAS()->mono()->regionControl());
	regionControlEditor_->setTitle("Mono Crystal Region");

	// Bragg control editor.

	braggControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->braggMotor());
	braggControlEditor_->setTitle("Mono Goniometer Angle");
	braggControlEditor_->setControlFormat('f', 2);

	// Scaler channel views.

	BioXASSIS3820ScalerChannelsView *channels = new BioXASSIS3820ScalerChannelsView(BioXASSideBeamline::bioXAS()->scaler());

	QVBoxLayout *channelsLayout = new QVBoxLayout();
	channelsLayout->addWidget(channels);

	channelViews_ = new QGroupBox();
	channelViews_->setTitle("Scaler channels");
	channelViews_->setLayout(channelsLayout);

	standardsComboBox_ = new QComboBox;

	foreach (CLSStandardsWheelElement *element, BioXASSideBeamline::bioXAS()->standardsWheel()->wheelElements())
		standardsComboBox_->addItem(element->name());

	connect(standardsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(onStandardsWheelIndexChanged(int)));
	connect(BioXASSideBeamline::bioXAS()->standardsWheel(), SIGNAL(nameChanged(int,QString)), this, SLOT(onStandardsWheelNameChanged(int,QString)));

	// Create and set main layout.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(energyControlEditor_);
	layout->addWidget(regionControlEditor_);
	layout->addWidget(braggControlEditor_);
	layout->addWidget(channelViews_);
	layout->addWidget(standardsComboBox_);
	layout->addStretch();

	setLayout(layout);
	setFixedWidth(400);

	// Initial settings.

	channelViews_->hide();

	// Make connections.

	connect( BioXASSideBeamline::bioXAS()->scaler(), SIGNAL(connectedChanged(bool)), this, SLOT(onScalerConnectedChanged()) );

	// Current settings.

	onScalerConnectedChanged();
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::onStandardsWheelIndexChanged(int index)
{
	BioXASSideBeamline::bioXAS()->standardsWheel()->moveToIndex(index);
}

void BioXASSidePersistentView::onStandardsWheelNameChanged(int index, const QString &newName)
{
	standardsComboBox_->setItemText(index, newName);
}

void BioXASSidePersistentView::onScalerConnectedChanged()
{
	CLSSIS3820Scaler *scaler = BioXASSideBeamline::bioXAS()->scaler();

	if (scaler && scaler->isConnected()) {
		channelViews_->show();
	} else {
		channelViews_->hide();
	}
}
