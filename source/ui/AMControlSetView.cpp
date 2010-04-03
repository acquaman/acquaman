#include "AMControlSetView.h"

AMControlSetView::AMControlSetView(AMControlSet *viewSet, QWidget *parent) :
	QGroupBox(parent)
{
/*	viewSet_ = viewSet;
	ml_ = new QHBoxLayout(this);
	gb_ = new QGroupBox(viewSet_->name(), this);
	QVBoxLayout *vl = new QVBoxLayout(gb_);
	QHBoxLayout *tmpHl;
	QLabel *tmpText;
	QAbstractSpinBox *tmpASB;
	AMControl *tmpCtrl;
	for(int x = 0; x < viewSet_->controls().count(); x++){
		tmpCtrl = viewSet_->controls().at(x);
		tmpHl = new QHBoxLayout(this);
		tmpText = new QLabel(tmpCtrl->objectName(), this, 0);
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
		tmpHl->addWidget(tmpText);
		tmpHl->addWidget(tmpASB);
		qDebug() << "Before add";
		vl->addLayout(tmpHl);
		qDebug() << "After add";
	}
	gb_->setLayout(vl);
	gb_->setFixedSize(250, 150);
	ml_->addWidget(gb_);
	this->setLayout(ml_);
*/

	viewSet_ = viewSet;
	setTitle(viewSet->name());
//	ml_ = new QHBoxLayout(this);
//	gb_ = new QGroupBox(viewSet_->name(), this);
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
//	gb_->setLayout(fl);
	setLayout(fl);
//	gb_->setFixedSize(250, 150);
	setFixedSize(250, 200);
//	ml_->addWidget(gb_);
//	this->setLayout(ml_);

}
