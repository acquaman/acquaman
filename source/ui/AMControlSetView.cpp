#include "AMControlSetView.h"

/// Set the title of the group box based on the name() function of the AMControlSet
/// Loop through the list of AMControls in the AMControlSet and create an appropriate spinbox.
/// Add the spin box and a label (from the AMControl objectName() function) and add to an internal form layout.
AMControlSetView::AMControlSetView(AMControlSet *viewSet, QWidget *parent) :
	QGroupBox(parent)
{
	viewSet_ = viewSet;
	setTitle(viewSet->name());
	QFormLayout *fl = new QFormLayout(this);
	QAbstractSpinBox *tmpASB;
	AMControl *tmpCtrl;
	for(int x = 0; x < viewSet_->controls().count(); x++){
		tmpCtrl = viewSet_->controls().at(x);
		if(tmpCtrl->isEnum()){
			tmpASB = new QSpinBox(this);
			((QSpinBox*)tmpASB)->setValue(tmpCtrl->value());
			((QSpinBox*)tmpASB)->setMaximum(tmpCtrl->maximumValue());
			((QSpinBox*)tmpASB)->setMinimum(tmpCtrl->minimumValue());
		}
		else{
			tmpASB = new QDoubleSpinBox(this);
			((QDoubleSpinBox*)tmpASB)->setValue(tmpCtrl->value());
			((QDoubleSpinBox*)tmpASB)->setMaximum(tmpCtrl->maximumValue());
			((QDoubleSpinBox*)tmpASB)->setMinimum(tmpCtrl->minimumValue());
		}
		fl->addRow(tmpCtrl->objectName(), tmpASB);
	}
	setLayout(fl);
	setFixedSize(250, 200);
}
