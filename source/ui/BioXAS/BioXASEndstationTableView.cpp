#include "BioXASEndstationTableView.h"

#include <QLabel>
#include <QGridLayout>

#include "ui/beamline/AMExtendedControlEditor.h"


BioXASEndstationTableView::BioXASEndstationTableView(BioXASEndstationTable *endstationTable, QWidget *parent) :
    QWidget(parent)
{
	endstationTable_ = endstationTable;

	heightControlEditor_ = createControlEditorView(endstationTable_->heightPVController());
	pitchControlEditor_ = createControlEditorView(endstationTable_->pitchPVController());
	if (endstationTable->rollPVController()) {
		rollControlEditor_ = createControlEditorView(endstationTable_->rollPVController());
		rollControlEditor_->hideBorder();
	}
	lateralControlEditor_ = createControlEditorView(endstationTable_->lateralPVController());
	yawControlEditor_ = createControlEditorView(endstationTable_->yawPVController());

	int curRow = 0;
	QGridLayout *tableControlLayout = new QGridLayout;
	tableControlLayout->addWidget(new QLabel("Height"),   curRow, 0, 1, 1);
	tableControlLayout->addWidget(heightControlEditor_,   curRow, 1, 1, 1);

	curRow++;
	tableControlLayout->addWidget(new QLabel("Pitch"),    curRow, 0, 1, 1);
	tableControlLayout->addWidget(pitchControlEditor_,    curRow, 1, 1, 1);

	if (endstationTable_->rollPVController()) {
		curRow++;
		tableControlLayout->addWidget(new QLabel("Roll"),  curRow, 0, 1, 1);
		tableControlLayout->addWidget(rollControlEditor_,  curRow, 1, 1, 1);
	}

	curRow++;
	tableControlLayout->addWidget(new QLabel("Lateral"),  curRow, 0, 1, 1);
	tableControlLayout->addWidget(lateralControlEditor_,  curRow, 1, 1, 1);

	curRow++;
	tableControlLayout->addWidget(new QLabel("Yaw"),      curRow, 0, 1, 1);
	tableControlLayout->addWidget(yawControlEditor_,      curRow, 1, 1, 1);

	setLayout(tableControlLayout);
}

AMExtendedControlEditor * BioXASEndstationTableView::createControlEditorView(AMControl* control)
{
	AMExtendedControlEditor *controlEditor_ = new AMExtendedControlEditor(control);
	controlEditor_->setControlFormat('f', 3);
	controlEditor_->hideBorder();

	return controlEditor_;
}
