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


#include "REIXSXESHexapodControlEditor.h"

#include "beamline/REIXS/REIXSBeamline.h"
#include <QBoxLayout>
#include <QLabel>
#include "ui/beamline/AMControlEditor.h"

 REIXSXESHexapodControlEditor::~REIXSXESHexapodControlEditor(){}
REIXSXESHexapodControlEditor::REIXSXESHexapodControlEditor(REIXSHexapod* hexapod, QWidget *parent) :
	QGroupBox("Hexapod Control", parent)
{
	hexapod_ = hexapod;

	QVBoxLayout* vl = new QVBoxLayout();

	vl->addWidget(new QLabel("X, Y, Z"));
	vl->addWidget(new AMControlEditor(hexapod->x()));
	vl->addWidget(new AMControlEditor(hexapod->y()));
	vl->addWidget(new AMControlEditor(hexapod->z()));

	vl->addWidget(new QLabel("U,V,W"));
	vl->addWidget(new AMControlEditor(hexapod->u()));
	vl->addWidget(new AMControlEditor(hexapod->v()));
	vl->addWidget(new AMControlEditor(hexapod->w()));


	vl->addWidget(new QLabel("R,S,T"));
	vl->addWidget(new AMControlEditor(hexapod->r()));
	vl->addWidget(new AMControlEditor(hexapod->s()));
	vl->addWidget(new AMControlEditor(hexapod->t()));

	setLayout(vl);

}
