#include "VESPERSBeamSelectorView.h"

#include "beamline/AMBeamlineActionItem.h"

#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

VESPERSBeamSelectorView::VESPERSBeamSelectorView(QWidget *parent)
	: QWidget(parent)
{
	currentBeam_ = 0;

	beams_ = new QButtonGroup;

	QToolButton *temp = new QToolButton;
	temp->setText("Pink");
	temp->setCheckable(true);
	beams_->addButton(temp, 0);

	temp = new QToolButton;
	temp->setText("10%");
	temp->setCheckable(true);
	beams_->addButton(temp, 1);

	temp = new QToolButton;
	temp->setText("1.6%");
	temp->setCheckable(true);
	beams_->addButton(temp, 2);

	temp = new QToolButton;
	temp->setText("Si");
	temp->setCheckable(true);
	beams_->addButton(temp, 3);

	connect(beams_, SIGNAL(buttonClicked(int)), this, SLOT(changeBeam(int)));
	connect(VESPERSBeamline::vespers(), SIGNAL(currentBeamChanged(VESPERSBeamline::Beam)), this, SLOT(onCurrentBeamChanged(VESPERSBeamline::Beam)));

	progressBar_ = new QProgressBar;
	progressBar_->hide();
	progressBar_->setRange(0, 100);

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
	AMBeamlineActionItem *action;

	switch(id){

	case 0:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERSBeamline::Pink);
		break;

	case 1:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERSBeamline::TenPercent);
		break;

	case 2:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERSBeamline::OnePointSixPercent);
		break;

	case 3:
		action = VESPERSBeamline::vespers()->createBeamChangeAction(VESPERSBeamline::Si);
		break;
	}

	progressBar_->show();
	connect(action, SIGNAL(finished()), this, SLOT(onBeamChangeCompleted()));
	connect(action, SIGNAL(progress(double,double)), this, SLOT(onProgressUpdate(double,double)));
	action->start();
}

void VESPERSBeamSelectorView::onProgressUpdate(double current, double end)
{
	Q_UNUSED(end)
	progressBar_->setValue((int)(current*100));
}

void VESPERSBeamSelectorView::onCurrentBeamChanged(VESPERSBeamline::Beam beam)
{
	if (currentBeam_)
		currentBeam_->setPalette(this->palette());

	switch(beam){

	case VESPERSBeamline::Pink:
		currentBeam_ = beams_->button(0);
		break;

	case VESPERSBeamline::TenPercent:
		currentBeam_ = beams_->button(1);
		break;

	case VESPERSBeamline::OnePointSixPercent:
		currentBeam_ = beams_->button(2);
		break;

	case VESPERSBeamline::Si:
		currentBeam_ = beams_->button(3);
		break;

	case VESPERSBeamline::None:
		currentBeam_ = 0;
		break;
	}

	if (currentBeam_)
		currentBeam_->setPalette(QPalette(Qt::green));
}
