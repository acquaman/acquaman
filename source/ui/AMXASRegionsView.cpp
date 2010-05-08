#include "AMXASRegionsView.h"

/// Creates buttons for add and delete region.
/// Defines a new model and views using table view.
/// Adds all items to form layout.
/// \todo Connect add and delete to something.
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
	regions_ = new AMXASRegionsList(this);
	qDebug() << "CONSTRUCTOR SHOULD BE DONE\n\n";
	regions_->setupModel();
	regions_->setDefaultControl(regions->at(0)->control());
	regions_->addRegion(0, 250, 1, 260);
	regions_->addRegion(1, 260.5, 0.5, 270);
	regions_->addRegion(2, 271, 1, 280);
	tv_ = new QTableView(this);
//	tv_->setModel(rm_);
	tv_->setModel(regions_->model());
	tv_->resize(tv_->sizeHint());
	QFormLayout *fl_ = new QFormLayout(this);
	fl_->addRow(tv_);
	fl_->addRow(hl_);
	setLayout(fl_);
	connect(addButton_, SIGNAL(clicked()), this, SIGNAL(addRegionClicked()));
	connect(deleteButton_, SIGNAL(clicked()), this, SLOT(deleteRegion()));
}
