#include "VESPERSSampleStageView.h"

#include <QToolButton>
#include <QGroupBox>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

VESPERSSampleStageView::VESPERSSampleStageView(QWidget *parent) :
	QWidget(parent)
{
	jog_ = new QDoubleSpinBox;
	jog_->setSuffix(" mm");
	jog_->setSingleStep(0.001);
	jog_->setMaximum(5.0);
	jog_->setMinimum(0.0000);
	jog_->setValue(0.050);
	jog_->setDecimals(3);
	jog_->setAlignment(Qt::AlignCenter);

	horizontal_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageHorizontal());
	vertical_ = qobject_cast<AMPVwStatusControl *>(VESPERSBeamline::vespers()->sampleStageVertical());

	if (horizontal_ && vertical_){

		connect(horizontal_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
		connect(vertical_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
		connect(VESPERSBeamline::vespers()->sampleStageMotorSet(), SIGNAL(connected(bool)), this, SLOT(onConnectedChanged(bool)));
	}

	QToolButton *goUp = new QToolButton;
	goUp->setIcon(QIcon(":/go-up.png"));
	connect(goUp, SIGNAL(clicked()), this, SLOT(onUpClicked()));

	QToolButton *goDown = new QToolButton;
	goDown->setIcon(QIcon(":/go-down.png"));
	connect(goDown, SIGNAL(clicked()), this, SLOT(onDownClicked()));

	QToolButton *goLeft = new QToolButton;
	goLeft->setIcon(QIcon(":/go-previous.png"));
	connect(goLeft, SIGNAL(clicked()), this, SLOT(onLeftClicked()));

	QToolButton *goRight = new QToolButton;
	goRight->setIcon(QIcon(":/go-next.png"));
	connect(goRight, SIGNAL(clicked()), this, SLOT(onRightClicked()));

	buttons_ = new QButtonGroup(this);
	buttons_->addButton(goUp, 0);
	buttons_->addButton(goDown, 1);
	buttons_->addButton(goLeft, 2);
	buttons_->addButton(goRight, 3);

	QGridLayout *arrowLayout = new QGridLayout;
	arrowLayout->addWidget(goUp, 0, 1);
	arrowLayout->addWidget(goDown, 2, 1);
	arrowLayout->addWidget(goLeft, 1, 0);
	arrowLayout->addWidget(goRight, 1, 2);

	QVBoxLayout *holderLayout = new QVBoxLayout;
	holderLayout->addLayout(arrowLayout);
	holderLayout->addWidget(jog_);

	QGroupBox *holder = new QGroupBox("Sample Stage Control");
	holder->setLayout(holderLayout);
	holder->setFlat(true);

	QVBoxLayout *sampleStageViewLayout = new QVBoxLayout;
	sampleStageViewLayout->addWidget(holder);

	setLayout(sampleStageViewLayout);
}

void VESPERSSampleStageView::onUpClicked()
{
	vertical_->move(jog_->value());
}

void VESPERSSampleStageView::onDownClicked()
{
	vertical_->move(-jog_->value());
}

void VESPERSSampleStageView::onLeftClicked()
{
	horizontal_->move(-jog_->value());
}

void VESPERSSampleStageView::onRightClicked()
{
	horizontal_->move(jog_->value());
}

void VESPERSSampleStageView::onMovingChanged(bool isMoving)
{
	for (int i = 0; i < 4; i++)
		buttons_->button(i)->setDisabled(isMoving);
}

void VESPERSSampleStageView::onConnectedChanged(bool isConnected)
{
	setEnabled(isConnected);
}
