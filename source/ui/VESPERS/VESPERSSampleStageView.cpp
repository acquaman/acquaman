/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "VESPERSSampleStageView.h"

#include <QGroupBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

VESPERSSampleStageView::VESPERSSampleStageView(QWidget *parent) :
	QWidget(parent)
{
	sampleStage_ = VESPERSBeamline::vespers()->pseudoSampleStage();
	connect(sampleStage_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(sampleStage_, SIGNAL(movingChanged(bool)), this, SLOT(onMovingChanged(bool)));
	connect(sampleStage_, SIGNAL(horizontalMoveError(bool)), this, SLOT(onHorizontalMoveError(bool)));
	connect(sampleStage_, SIGNAL(verticalMoveError(bool)), this, SLOT(onVerticalMoveError(bool)));
	connect(sampleStage_, SIGNAL(normalMoveError(bool)), this, SLOT(onNormalMoveError(bool)));

	QFont font(this->font());
	font.setBold(true);

	QLabel *h = new QLabel("H :");
	h->setFont(font);
	QLabel *v = new QLabel("V :");
	v->setFont(font);
	QLabel *jog = new QLabel("Jog :");
	jog->setFont(font);

	jog_ = new QDoubleSpinBox;
	jog_->setSuffix(" mm");
	jog_->setSingleStep(0.001);
	jog_->setMaximum(5.0);
	jog_->setMinimum(0.0000);
	jog_->setValue(0.050);
	jog_->setDecimals(3);
	jog_->setAlignment(Qt::AlignCenter);
	jog_->setFixedWidth(110);

	QHBoxLayout *jogLayout = new QHBoxLayout;
	jogLayout->addWidget(jog, 0, Qt::AlignRight);
	jogLayout->addWidget(jog_);

	horizontal_ = new QDoubleSpinBox;
	horizontal_->setSuffix(" mm");
	horizontal_->setSingleStep(0.001);
	horizontal_->setRange(-100, 100);
	horizontal_->setDecimals(3);
	horizontal_->setAlignment(Qt::AlignCenter);
	horizontal_->setFixedWidth(110);
	connect(horizontal_, SIGNAL(editingFinished()), this, SLOT(onHorizontalSetpoint()));
	connect(sampleStage_, SIGNAL(horizontalSetpointChanged(double)), horizontal_, SLOT(setValue(double)));

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->addWidget(h, 0, Qt::AlignRight);
	hLayout->addWidget(horizontal_);

	vertical_ = new QDoubleSpinBox;
	vertical_->setSuffix(" mm");
	vertical_->setSingleStep(0.001);
	vertical_->setRange(-100, 100);
	vertical_->setDecimals(3);
	vertical_->setAlignment(Qt::AlignCenter);
	vertical_->setFixedWidth(110);
	connect(vertical_, SIGNAL(editingFinished()), this, SLOT(onVerticalSetpoint()));
	connect(sampleStage_, SIGNAL(verticalSetpointChanged(double)), vertical_, SLOT(setValue(double)));

	QHBoxLayout *vLayout = new QHBoxLayout;
	vLayout->addWidget(v, 0, Qt::AlignRight);
	vLayout->addWidget(vertical_);

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/OFF.png").pixmap(25));

	goUp_ = new QToolButton;
	goUp_->setIcon(QIcon(":/go-up.png"));
	connect(goUp_, SIGNAL(clicked()), this, SLOT(onUpClicked()));

	goDown_ = new QToolButton;
	goDown_->setIcon(QIcon(":/go-down.png"));
	connect(goDown_, SIGNAL(clicked()), this, SLOT(onDownClicked()));

	goLeft_ = new QToolButton;
	goLeft_->setIcon(QIcon(":/go-previous.png"));
	connect(goLeft_, SIGNAL(clicked()), this, SLOT(onLeftClicked()));

	goRight_ = new QToolButton;
	goRight_->setIcon(QIcon(":/go-next.png"));
	connect(goRight_, SIGNAL(clicked()), this, SLOT(onRightClicked()));

	buttons_ = new QButtonGroup(this);
	buttons_->addButton(goUp_, 0);
	buttons_->addButton(goDown_, 1);
	buttons_->addButton(goLeft_, 2);
	buttons_->addButton(goRight_, 3);

	QToolButton *stop = new QToolButton;
	stop->setIcon(QIcon(":/stop.png"));
	connect(stop, SIGNAL(clicked()), sampleStage_, SLOT(stopAll()));

	QGridLayout *arrowLayout = new QGridLayout;
	arrowLayout->addWidget(goUp_, 0, 1);
	arrowLayout->addWidget(goDown_, 2, 1);
	arrowLayout->addWidget(goLeft_, 1, 0);
	arrowLayout->addWidget(goRight_, 1, 2);
	arrowLayout->addWidget(stop, 1, 1);
	arrowLayout->addWidget(status_, 0, 0);

	QVBoxLayout *absoluteValueLayout = new QVBoxLayout;
	absoluteValueLayout->addLayout(hLayout);
	absoluteValueLayout->addLayout(vLayout);
	absoluteValueLayout->addLayout(jogLayout);

	QHBoxLayout *sampleStageLayout = new QHBoxLayout;
	sampleStageLayout->addLayout(arrowLayout);
	sampleStageLayout->addLayout(absoluteValueLayout);

	setLayout(sampleStageLayout);
}

void VESPERSSampleStageView::onUpClicked()
{
	sampleStage_->moveVertical(sampleStage_->verticalPosition() + jog_->value());
}

void VESPERSSampleStageView::onDownClicked()
{
	sampleStage_->moveVertical(sampleStage_->verticalPosition() - jog_->value());
}

void VESPERSSampleStageView::onLeftClicked()
{
	sampleStage_->moveHorizontal(sampleStage_->horizontalPosition() - jog_->value());
}

void VESPERSSampleStageView::onRightClicked()
{
	sampleStage_->moveHorizontal(sampleStage_->horizontalPosition() + jog_->value());
}

void VESPERSSampleStageView::onMovingChanged(bool isMoving)
{
	if (isMoving)
		status_->setPixmap(QIcon(":/ON.png").pixmap(25));
	else
		status_->setPixmap(QIcon(":/OFF.png").pixmap(25));

	for (int i = 0; i < 4; i++)
		buttons_->button(i)->setDisabled(isMoving);
}

void VESPERSSampleStageView::onHorizontalSetpoint()
{
	sampleStage_->moveHorizontal(horizontal_->value());
}

void VESPERSSampleStageView::onVerticalSetpoint()
{
	sampleStage_->moveVertical(vertical_->value());
}

void VESPERSSampleStageView::onHorizontalMoveError(bool direction)
{
	if (direction && goRight_->isEnabled()){

		goRight_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the horizontal motor.  Consider changing the position of your sample on the sample mount."));
	}
	else if (!direction && goLeft_->isEnabled()){

		goLeft_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the horizontal motor.  Consider changing the position of your sample on the sample mount."));
	}
}

void VESPERSSampleStageView::onVerticalMoveError(bool direction)
{
	if (direction && goDown_->isEnabled()){

		goDown_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the vertical motor.  Consider changing the position of your sample on the sample mount."));
	}
	else if (!direction && goUp_->isEnabled()){

		goUp_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the vertical motor.  Consider changing the position of your sample on the sample mount."));
	}
}

void VESPERSSampleStageView::onNormalMoveError(bool direction)
{
	if (direction && goDown_->isEnabled()){

		goDown_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the normal motor.  Consider changing the position of your sample on the sample mount."));
	}
	else if (!direction && goUp_->isEnabled()){

		goUp_->setEnabled(false);
		QMessageBox::warning(this, "End of travel!", QString("You have reached the end of travel for the normal motor.  Consider changing the position of your sample on the sample mount."));
	}
}

