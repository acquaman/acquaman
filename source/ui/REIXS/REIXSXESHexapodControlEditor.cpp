#include "REIXSXESHexapodControlEditor.h"

#include "beamline/REIXS/REIXSBeamline.h"
#include <QBoxLayout>
#include "ui/AMControlEditor.h"

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
