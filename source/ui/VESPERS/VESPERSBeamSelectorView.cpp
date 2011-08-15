#include "VESPERSBeamSelectorView.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

VESPERSBeamSelectorView::VESPERSBeamSelectorView(VESPERSBeamSelector *beamSelector, QWidget *parent)
	: QWidget(parent)
{
	beamSelector_ = beamSelector;

	beams_ = new QButtonGroup;

	QPushButton *temp = new QPushButton("Pink");
	temp->setCheckable(true);
	beams_->addButton(temp, 0);

	temp = new QPushButton("1.6%");
	temp->setCheckable(true);
	beams_->addButton(temp, 1);

	temp = new QPushButton("10%");
	temp->setCheckable(true);
	beams_->addButton(temp, 2);

	temp = new QPushButton("Si");
	temp->setCheckable(true);
	beams_->addButton(temp, 3);

	connect(beams_, SIGNAL(buttonClicked(int)), this, SLOT(changeBeam(int)));
	connect(beamSelector_, SIGNAL(currentBeamChanged(VESPERSBeamSelector::Beam)), this, SLOT(onCurrentBeamChanged(VESPERSBeamSelector::Beam)));

	progressBar_ = new QProgressBar;

	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	buttonsLayout->addWidget(beams_->button(0));
	buttonsLayout->addWidget(beams_->button(1));
	buttonsLayout->addWidget(beams_->button(2));
	buttonsLayout->addWidget(beams_->button(3));

	QVBoxLayout *beamSelectorLayout = new QVBoxLayout;
	beamSelectorLayout->addLayout(buttonsLayout);
	beamSelectorLayout->addWidget(progressBar_);

	setLayout(beamSelectorLayout);
}

void VESPERSBeamSelectorView::changeBeam(int id)
{
	switch(id){

	case 0:
		beamSelector_->changeBeam(VESPERSBeamSelector::Pink);
		break;

	case 1:
		beamSelector_->changeBeam(VESPERSBeamSelector::OnePointSixPercent);
		break;

	case 2:
		beamSelector_->changeBeam(VESPERSBeamSelector::TenPercent);
		break;

	case 3:
		beamSelector_->changeBeam(VESPERSBeamSelector::Si);
		break;
	}
}

void VESPERSBeamSelectorView::onCurrentBeamChanged(VESPERSBeamSelector::Beam beam)
{
	if (currentBeam_)
		currentBeam_->setPalette(this->palette());

	switch(beam){

	case VESPERSBeamSelector::Pink:
		currentBeam_ = beams_->button(0);
		break;

	case VESPERSBeamSelector::OnePointSixPercent:
		currentBeam_ = beams_->button(1);
		break;

	case VESPERSBeamSelector::TenPercent:
		currentBeam_ = beams_->button(2);
		break;

	case VESPERSBeamSelector::Si:
		currentBeam_ = beams_->button(3);
		break;

	case VESPERSBeamSelector::None:
		currentBeam_ = 0;
		break;
	}

	if (currentBeam_)
		currentBeam_->setPalette(QPalette(Qt::green));
}
