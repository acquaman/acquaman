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


#include "REIXSBeamOnOffActionEditor.h"

#include <QRadioButton>
#include <QHBoxLayout>
#include <QLabel>

REIXSBeamOnOffActionEditor::REIXSBeamOnOffActionEditor(REIXSBeamOnOffActionInfo *info, QWidget *parent) :
	QFrame(parent)
{
	info_ = info;
	setFrameStyle(QFrame::StyledPanel);

	QRadioButton* beamOnButton = new QRadioButton("On");
	QRadioButton* beamOffButton = new QRadioButton("Off");

	QHBoxLayout* hl = new QHBoxLayout(this);
	hl->addWidget(new QLabel("Beam: "));
	hl->addWidget(beamOnButton);
	hl->addWidget(beamOffButton);
	hl->addStretch();

	beamOnButton->setChecked(info_->beamOn());
	beamOffButton->setChecked(!info_->beamOn());

	connect(beamOnButton, SIGNAL(toggled(bool)), this, SLOT(onBeamOnRadioButtonToggled(bool)));
}

#include <QDebug>
void REIXSBeamOnOffActionEditor::onBeamOnRadioButtonToggled(bool beamOn)
{
	qDebug() << "Beam on toggled:" << beamOn;
	info_->setBeamOn(beamOn);
}
