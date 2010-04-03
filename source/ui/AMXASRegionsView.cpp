#include "AMXASRegionsView.h"

AMXASRegionsView::AMXASRegionsView(QList<AMXASRegion*> *regions, QWidget *parent) :
    QWidget(parent)
{
//	setFixedSize(300, 300);
	this->setMaximumSize(400, 400);
	addButton_ = new QPushButton("Add Region", this);
	deleteButton_ = new QPushButton("Delete Region", this);
	hl_ = new QHBoxLayout();
	hl_->addWidget(addButton_);
	hl_->addWidget(deleteButton_);
	rm_ = new AMXASRegionModel(regions, this);
	tv_ = new QTableView(this);
	tv_->setModel(rm_);
	tv_->resize(tv_->sizeHint());
//	tv_->setFixedSize(300, 250);
//	vl_ = new QVBoxLayout(this);
//	vl_->addLayout(hl_);
//	vl_->addWidget(tv_, 0);
	fl_ = new QFormLayout(this);
	fl_->addRow(tv_);
//	fl_->addRow(addButton_, deleteButton_);
	fl_->addRow(hl_);
	setLayout(fl_);
}
