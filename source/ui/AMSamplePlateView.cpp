#include "AMSamplePlateView.h"

AMSamplePlateView::AMSamplePlateView(QString title, QWidget *parent) :
	QGroupBox(title, parent)
{
	samplePlate_ = new AMSamplePlate(this);

	plateNameLabel_ = new QLabel(samplePlate_->plateName());
	plateNameLabel_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	existingPlates_ = new QComboBox();
	existingPlates_->addItem("Load Existing");
	sampleList_ = new QGroupBox("Samples");
	sampleList_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	vl_ = new QVBoxLayout();
	vl_->addWidget(plateNameLabel_, 1, Qt::AlignTop);
	vl_->addWidget(existingPlates_, 1, Qt::AlignTop);
	vl_->addWidget(sampleList_, 5, Qt::AlignTop);
	setLayout(vl_);
}
