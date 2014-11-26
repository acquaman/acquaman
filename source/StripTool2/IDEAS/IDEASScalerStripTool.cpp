#include "IDEASScalerStripTool.h"

IDEASScalerStripTool::IDEASScalerStripTool(QWidget *parent) :
    STWidget(parent)
{
	variables_->addVariable("BL08B2-1:mcs00:fbk");
	STVariable *sr1Current = variables_->variablesWithName("BL08B2-1:mcs00:fbk").last();
	sr1Current->setDescription("I_0 Ion Chamber");
	sr1Current->setUnits("");
	sr1Current->setColor(QColor(Qt::green));
}


void IDEASScalerStripTool::onCustomContextMenuRequested(QPoint position)
{
    QMenu menu(this);

    QAction *action = menu.addAction("Add variable");
    action = menu.addAction("Add SR1 Current");
    action = menu.addAction("Add Sample");
    action = menu.addAction("Add Reference");
    action = menu.addAction("Add Picoammeter");

    menu.addSeparator();
    action = menu.addAction("Clear variables");
    action = menu.addAction("Edit variables");
    if (variables_->variableCount() == 0)
	action->setEnabled(false);
    action = menu.addAction("Edit plot");

    QAction *selected = menu.exec(mapToGlobal(position));

    if (selected) {

	if (selected->text() == "Add variable") {
	    toAddVariable();

	} else if (selected->text() == "Add SR1 Current") {
	    addSR1Current();
	} else if (selected->text() == "Add Sample") {
	    addSample();
	} else if (selected->text() == "Add Reference") {
	    addReference();
	} else if (selected->text() == "Add Picoammeter") {
	    addPicoammeter();

	} else if (selected->text() == "Clear variables") {
		removeAll();
	} else if (selected->text() == "Edit variables") {
	    toEditVariables();

	} else if (selected->text() == "Edit plot") {
	    toEditPlot();

	}
    }
}

void IDEASScalerStripTool::addSample()
{
    variables_->addVariable("BL08B2-1:mcs01:fbk");

    STVariable *sr1Current = variables_->variablesWithName("BL08B2-1:mcs01:fbk").last();
    sr1Current->setDescription("Sample Ion Chamber");
    sr1Current->setUnits("");
    sr1Current->setColor(QColor(Qt::red));
}

void IDEASScalerStripTool::addReference()
{
    variables_->addVariable("BL08B2-1:mcs02:fbk");

    STVariable *sr1Current = variables_->variablesWithName("BL08B2-1:mcs02:fbk").last();
    sr1Current->setDescription("Reference Ion Chamber");
    sr1Current->setUnits("");
    sr1Current->setColor(QColor(Qt::darkRed));
}

void IDEASScalerStripTool::addPicoammeter()
{
    variables_->addVariable("A1608-9-01:A:fbk");

    STVariable *sr1Current = variables_->variablesWithName("A1608-9-01:A:fbk").last();
    sr1Current->setDescription("Picoammeter");
    sr1Current->setUnits("A");
    sr1Current->setColor(QColor(Qt::magenta));
}

void IDEASScalerStripTool::removeAll()
{
	while (variables_->variableCount() != 0)
	    variables_->deleteVariableAt(0);
}
