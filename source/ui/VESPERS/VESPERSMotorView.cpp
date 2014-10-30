/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "ui/VESPERS/VESPERSMotorView.h"

#include <QGridLayout>

 VESPERSMotorView::~VESPERSMotorView(){}
VESPERSMotorView::VESPERSMotorView(QWidget *parent) :
	QWidget(parent)
{
	control_ = 0;
	type_ = Full;
	title_ = new QLabel;
	title_->setFont(QFont(title_->font().family(), 15, 75));
	status_ = new QLabel;
	status_->setFont(QFont(title_->font().family(), 15, 75));
	status_->setObjectName("Status");
	setpoint_ = new QLineEdit;
	setpoint_->setAlignment(Qt::AlignCenter);
	setpointLabel_ = new QLabel(tr("Setpoint:"));
	jog_ = new QLineEdit;
	jog_->setAlignment(Qt::AlignCenter);
	jogLabel_ = new QLabel(tr("Jog:"));
	units_ = new QLabel;
	jogUnits_ = new QLabel;
	jogLeft_ = new QToolButton;
	jogLeft_->setIcon(QIcon(":/go-previous.png"));
	jogRight_ = new QToolButton;
	jogRight_->setIcon(QIcon(":/go-next.png"));
	firstSetpointButton_ = new QToolButton;
	firstSetpointButton_->setMinimumSize(100, 63);
	firstSetpointButton_->setFont(QFont("Times New Roman", 28, 50));
	secondSetpointButton_ = new QToolButton;
	secondSetpointButton_->setMinimumSize(100, 63);
	secondSetpointButton_->setFont(QFont("Times New Roman", 28, 50));
	stop_ = new QToolButton;
	stop_->setIcon(QIcon(":/stop.png"));
	lowLimit_ = 0;
	highLimit_ = 0;

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(title_, 0, 0);
	layout->addWidget(stop_, 1, 0);
	layout->addWidget(status_, 1, 1, 1, 3, Qt::AlignCenter);
	layout->addWidget(setpointLabel_, 2, 0, Qt::AlignRight);
	layout->addWidget(setpoint_, 2, 1, 1, 4, Qt::AlignCenter);
	layout->addWidget(jogLabel_, 3, 0, Qt::AlignRight);
	layout->addWidget(jogLeft_, 3, 1);
	layout->addWidget(jog_, 3, 2, 1, 2, Qt::AlignCenter);
	layout->addWidget(jogRight_, 3, 4);

	setLayout(layout);
}

void VESPERSMotorView::setControl(AMControl *control)
{
	// Disconnect everything first before starting over.
	hideAndDisconnect();

	control_ = control;
	type_ = Full;
	lowLimit_ = 0;
	highLimit_ = 0;
	if (QString::compare(control_->units(), "") == 0){

		units_->setText("mm");
		jogUnits_->setText("mm");
	}
	else{

		units_->setText(control_->units());
		jogUnits_->setText(control_->units());
	}
	title_->setText(control_->name());
	setpoint_->setText(QString::number(control_->value(), 'f', 3));
	jog_->setText(QString::number(0, 'f', 3));

	connect(stop_, SIGNAL(clicked()), control_, SLOT(stop()));
	connect(control_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(control_, SIGNAL(unitsChanged(QString)), units_, SLOT(setText(QString)));
	connect(control_, SIGNAL(unitsChanged(QString)), jogUnits_, SLOT(setText(QString)));
	connect(control_, SIGNAL(setpointChanged(double)), this, SLOT(updateSetpoint(double)));
	connect(((AMPVwStatusControl *)control_)->movingPV(), SIGNAL(valueChanged(int)), this, SLOT(updateStatus(int)));

	connect(setpoint_, SIGNAL(returnPressed()), this, SLOT(move()));
	connect(jogLeft_, SIGNAL(clicked()), this, SLOT(jogLeft()));
	connect(jogRight_, SIGNAL(clicked()), this, SLOT(jogRight()));

	this->layout()->deleteLater();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(title_, 0, 0, 1, 6);
	layout->addWidget(stop_, 1, 0);
	layout->addWidget(status_, 1, 1, 1, 5, Qt::AlignCenter);
	layout->addWidget(setpointLabel_, 2, 0, Qt::AlignRight);
	layout->addWidget(setpoint_, 2, 1, 1, 5);
	layout->addWidget(units_, 2, 5);
	layout->addWidget(jogLabel_, 3, 0, Qt::AlignRight);
	layout->addWidget(jogLeft_, 3, 1);
	layout->addWidget(jog_, 3, 2, 1, 3);
	layout->addWidget(jogUnits_, 3, 4);
	layout->addWidget(jogRight_, 3, 5);

	setpoint_->show();
	setpointLabel_->show();
	jogLabel_->show();
	jogLeft_->show();
	jog_->show();
	jogRight_->show();
	units_->show();
	jogUnits_->show();

	setLayout(layout);
}

void VESPERSMotorView::setControl(AMControl *control, double lowLimit, double highLimit)
{
	// Disconnect everything first before starting over.
	hideAndDisconnect();

	control_ = control;
	type_ = Full;
	lowLimit_ = lowLimit;
	highLimit_ = highLimit;
	if (QString::compare(control_->units(), "") == 0){

		units_->setText("mm");
		jogUnits_->setText("mm");
	}
	else{

		units_->setText(control_->units());
		jogUnits_->setText(control_->units());
	}
	title_->setText(control_->name());
	setpoint_->setText(QString::number(control_->value(), 'f', 3));
	jog_->setText(QString::number(0, 'f', 3));

	connect(stop_, SIGNAL(clicked()), control_, SLOT(stop()));
	connect(control_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(control_, SIGNAL(unitsChanged(QString)), units_, SLOT(setText(QString)));
	connect(control_, SIGNAL(unitsChanged(QString)), jogUnits_, SLOT(setText(QString)));
	connect(control_, SIGNAL(setpointChanged(double)), this, SLOT(updateSetpoint(double)));
	connect(((AMPVwStatusControl *)control_)->movingPV(), SIGNAL(valueChanged(int)), this, SLOT(updateStatus(int)));

	connect(setpoint_, SIGNAL(returnPressed()), this, SLOT(move()));
	connect(jogLeft_, SIGNAL(clicked()), this, SLOT(jogLeft()));
	connect(jogRight_, SIGNAL(clicked()), this, SLOT(jogRight()));

	this->layout()->deleteLater();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(title_, 0, 0, 1, 6);
	layout->addWidget(stop_, 1, 0);
	layout->addWidget(status_, 1, 1, 1, 5, Qt::AlignCenter);
	layout->addWidget(setpointLabel_, 2, 0, Qt::AlignRight);
	layout->addWidget(setpoint_, 2, 1, 1, 5);
	layout->addWidget(units_, 2, 5);
	layout->addWidget(jogLabel_, 3, 0, Qt::AlignRight);
	layout->addWidget(jogLeft_, 3, 1);
	layout->addWidget(jog_, 3, 2, 1, 3);
	layout->addWidget(jogUnits_, 3, 4);
	layout->addWidget(jogRight_, 3, 5);

	setpoint_->show();
	setpointLabel_->show();
	jogLabel_->show();
	jogLeft_->show();
	jog_->show();
	jogRight_->show();
	units_->show();
	jogUnits_->show();

	setLayout(layout);
}

void VESPERSMotorView::setControl(AMControl *control, double firstSetpoint, double secondSetpoint, QString firstLabel, QString secondLabel)
{
	// Disconnect everything first before starting over.
	hideAndDisconnect();

	control_ = control;
	type_ = Simple;
	lowLimit_ = 0;
	highLimit_ = 0;
	units_->setText(control_->units());
	title_->setText(control_->name());
	firstSetpointButton_->setText(firstLabel);
	secondSetpointButton_->setText(secondLabel);
	firstSetpoint_ = firstSetpoint;
	secondSetpoint_ = secondSetpoint;

	connect(stop_, SIGNAL(clicked()), control_, SLOT(stop()));
	connect(control_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
	connect(control_, SIGNAL(unitsChanged(QString)), units_, SLOT(setText(QString)));
	connect(control_, SIGNAL(unitsChanged(QString)), jogUnits_, SLOT(setText(QString)));
	connect(control_, SIGNAL(setpointChanged(double)), this, SLOT(updateSetpoint(double)));
	connect(((AMPVwStatusControl *)control_)->movingPV(), SIGNAL(valueChanged(int)), this, SLOT(updateStatus(int)));

	connect(firstSetpointButton_, SIGNAL(clicked()), this, SLOT(moveToFirstSetpoint()));
	connect(secondSetpointButton_, SIGNAL(clicked()), this, SLOT(moveToSecondSetpoint()));

	this->layout()->deleteLater();

	QGridLayout *layout = new QGridLayout;
	layout->addWidget(title_, 0, 0, 1, 4);
	layout->addWidget(stop_, 1, 0);
	layout->addWidget(status_, 1, 1, 1, 3, Qt::AlignCenter);
	layout->addWidget(firstSetpointButton_, 2, 0, 1, 2, Qt::AlignCenter);
	layout->addWidget(secondSetpointButton_, 2, 2, 1, 2, Qt::AlignCenter);

	firstSetpointButton_->show();
	secondSetpointButton_->show();

	setLayout(layout);
}

void VESPERSMotorView::hideAndDisconnect()
{
	// If no control has been set yet, then don't touch connections that don't exist.
	if (control_){

		disconnect(stop_, SIGNAL(clicked()), control_, SLOT(stop()));
		disconnect(control_, SIGNAL(connected(bool)), this, SLOT(setEnabled(bool)));
		disconnect(control_, SIGNAL(unitsChanged(QString)), units_, SLOT(setText(QString)));
		disconnect(control_, SIGNAL(unitsChanged(QString)), jogUnits_, SLOT(setText(QString)));
		disconnect(control_, SIGNAL(setpointChanged(double)), this, SLOT(updateSetpoint(double)));
		disconnect(((AMPVwStatusControl *)control_)->movingPV(), SIGNAL(valueChanged(int)), this, SLOT(updateStatus(int)));

		if (type_ == Full){

			disconnect(setpoint_, SIGNAL(returnPressed()), this, SLOT(move()));
			disconnect(jogLeft_, SIGNAL(clicked()), this, SLOT(jogLeft()));
			disconnect(jogRight_, SIGNAL(clicked()), this, SLOT(jogRight()));
			setpoint_->hide();
			setpointLabel_->hide();
			jogLabel_->hide();
			jog_->hide();
			jogLeft_->hide();
			jogRight_->hide();
			units_->hide();
			jogUnits_->hide();
		}
		else if (type_ == Simple){

			disconnect(firstSetpointButton_, SIGNAL(clicked()), this, SLOT(moveToFirstSetpoint()));
			disconnect(secondSetpointButton_, SIGNAL(clicked()), this, SLOT(moveToSecondSetpoint()));
			firstSetpointButton_->hide();
			secondSetpointButton_->hide();
		}
	}
}

void VESPERSMotorView::updateSetpoint(double setpoint)
{
	if (type_ == Full)
		setpoint_->setText(QString::number(setpoint, 'f', 3));
}

void VESPERSMotorView::updateStatus(int status)
{
	switch(status){
	case 0:
		status_->setText("Move Done");
		status_->setStyleSheet("QLabel#Status { color: rgb(0,150,0) }");
		break;
	case 1:
		status_->setText("Move Active");
		status_->setStyleSheet("QLabel#Status { color: rgb(0,200,0) }");
		break;
	case 2:
		status_->setText("At Limit");
		status_->setStyleSheet("QLabel#Status { color: rgb(200,0,0) }");
		break;
	case 3:
		status_->setText("Forced Stop");
		status_->setStyleSheet("QLabel#Status { color: rgb(200,0,0) }");
		break;
	case 4:
		status_->setText("Error");
		status_->setStyleSheet("QLabel#Status { color: rgb(255,0,0) }");
		break;
	}
}
