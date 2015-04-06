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

#include "beamline/BioXAS/BioXASSideBeamline.h"

#include <QComboBox>

BioXASSidePersistentView::BioXASSidePersistentView(QWidget *parent) :
	QWidget(parent)
{
	// Create UI elements.

	energyControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->energyControl());
	energyControlEditor_->setTitle("Energy");
	energyControlEditor_->setControlFormat('f', 2);

	regionControlEditor_ = new BioXASSSRLMonochromatorRegionControlEditor(BioXASSideBeamline::bioXAS()->mono()->regionControl());
	regionControlEditor_->setTitle("Region");

	braggControlEditor_ = new AMExtendedControlEditor(BioXASSideBeamline::bioXAS()->mono()->braggMotor());
	braggControlEditor_->setTitle("Bragg motor position");

	QComboBox *standardsComboBox = new QComboBox;

	foreach (CLSStandardsWheelElement *element, BioXASSideBeamline::bioXAS()->standardsWheel()->wheelElements())
		standardsComboBox->addItem(element->name());

	connect(standardsComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onStandardsWheelIndexChanged(int)));

	// Create and set layouts.

	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(energyControlEditor_);
	layout->addWidget(regionControlEditor_);
	layout->addWidget(braggControlEditor_);
	layout->addWidget(standardsComboBox);
	layout->addStretch();

	setLayout(layout);
	setFixedWidth(300);
}

BioXASSidePersistentView::~BioXASSidePersistentView()
{

}

void BioXASSidePersistentView::onStandardsWheelIndexChanged(int index)
{
	BioXASSideBeamline::bioXAS()->standardsWheel()->moveToIndex(index);
}
