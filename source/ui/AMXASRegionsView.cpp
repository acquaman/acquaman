#include "AMXASRegionsView.h"

/// Creates buttons for add and delete region. \todo Connect add and delete to something.
/// Defines a new model and views using table view.
/// Adds all items to form layout.
AMXASRegionsView::AMXASRegionsView(QList<AMXASRegion*> *regions, QWidget *parent) :
    QWidget(parent)
{
	this->setMaximumSize(400, 400);
	addButton_ = new QPushButton("Add Region", this);
	deleteButton_ = new QPushButton("Delete Region", this);
	QHBoxLayout *hl_ = new QHBoxLayout();
	hl_->addWidget(addButton_);
	hl_->addWidget(deleteButton_);
	rm_ = new AMXASRegionModel(regions, this);
	tv_ = new QTableView(this);
	tv_->setModel(rm_);
	tv_->resize(tv_->sizeHint());
	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(tv_);
	fl_->addRow(hl_);
	setLayout(fl_);
}
