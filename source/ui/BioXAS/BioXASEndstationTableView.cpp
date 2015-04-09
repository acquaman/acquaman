#include "BioXASEndstationTableView.h"

#include <QLabel>
#include <QGridLayout>

#include "ui/beamline/AMExtendedControlEditor.h"


BioXASEndstationTableView::BioXASEndstationTableView(BioXASEndstationTable *endstationTable, QWidget *parent) :
    QWidget(parent)
{
	endstationTable_ = endstationTable;

	heightControlEditor_ = new AMExtendedControlEditor(endstationTable_->heightPVController());
	pitchControlEditor_ = new AMExtendedControlEditor(endstationTable_->pitchPVController());
	lateralControlEditor_ = new AMExtendedControlEditor(endstationTable_->lateralPVController());
	yawControlEditor_ = new AMExtendedControlEditor(endstationTable_->yawPVController());
	if (endstationTable->rollPVController())
		rollControlEditor_ = new AMExtendedControlEditor(endstationTable_->rollPVController());

	QGridLayout *tableControlLayout = new QGridLayout;
	tableControlLayout->addWidget(new QLabel("Height"),   0, 0, 1, 1);
	tableControlLayout->addWidget(heightControlEditor_,   0, 1, 1, 1);
	tableControlLayout->addWidget(new QLabel("Pitch"),    1, 0, 1, 1);
	tableControlLayout->addWidget(pitchControlEditor_,    1, 1, 1, 1);
	tableControlLayout->addWidget(new QLabel("Lateral"),  2, 0, 1, 1);
	tableControlLayout->addWidget(lateralControlEditor_,  2, 1, 1, 1);
	tableControlLayout->addWidget(new QLabel("Yaw"),      3, 0, 1, 1);
	tableControlLayout->addWidget(yawControlEditor_,      3, 1, 1, 1);
	if (endstationTable_->rollPVController()) {
		tableControlLayout->addWidget(new QLabel("Roll"),  4, 0, 1, 1);
		tableControlLayout->addWidget(rollControlEditor_,  4, 1, 1, 1);
	}

	setLayout(tableControlLayout);
}
