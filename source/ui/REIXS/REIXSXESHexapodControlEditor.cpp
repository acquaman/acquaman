/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "REIXSXESHexapodControlEditor.h"

#include "beamline/REIXS/REIXSBeamline.h"
#include <QBoxLayout>
#include <QLabel>
#include "ui/beamline/AMControlEditor.h"

REIXSXESHexapodControlEditor::REIXSXESHexapodControlEditor(REIXSHexapod* hexapod, QWidget *parent) :
	QGroupBox("Hexapod Control", parent)
{
	hexapod_ = hexapod;

	QVBoxLayout* vl = new QVBoxLayout();

	vl->addWidget(new QLabel("X, Y, Z"));
	vl->addWidget(new AMBasicControlEditor(hexapod->x()));
	vl->addWidget(new AMBasicControlEditor(hexapod->y()));
	vl->addWidget(new AMBasicControlEditor(hexapod->z()));

	vl->addWidget(new QLabel("U,V,W"));
	vl->addWidget(new AMBasicControlEditor(hexapod->u()));
	vl->addWidget(new AMBasicControlEditor(hexapod->v()));
	vl->addWidget(new AMBasicControlEditor(hexapod->w()));


	vl->addWidget(new QLabel("R,S,T"));
	vl->addWidget(new AMBasicControlEditor(hexapod->r()));
	vl->addWidget(new AMBasicControlEditor(hexapod->s()));
	vl->addWidget(new AMBasicControlEditor(hexapod->t()));

	setLayout(vl);

}
