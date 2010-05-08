#include "AMXASRegionsView.h"

/// Creates buttons for add and delete region.
/// Defines a new model and views using table view.
/// Adds all items to form layout.
/// \todo Connect add and delete to something.
AMXASRegionsView::AMXASRegionsView(AMXASRegionsList *regions, QWidget *parent) :
	QWidget(parent)
{
	this->setMaximumSize(400, 400);
	addButton_ = new QPushButton("Add Region", this);
	deleteButton_ = new QPushButton("Delete Region", this);
	QHBoxLayout *hl_ = new QHBoxLayout();
	hl_->addWidget(addButton_);
	hl_->addWidget(deleteButton_);
	regions_ = regions;
	tv_ = new QTableView(this);
	tv_->setModel(regions_->model());
	tv_->resize(tv_->sizeHint());
	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(tv_);
	fl_->addRow(hl_);
	setLayout(fl_);
	connect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
}
