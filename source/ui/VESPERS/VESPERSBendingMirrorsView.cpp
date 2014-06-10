/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "VESPERSBendingMirrorsView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QDoubleSpinBox>
#include <QLineEdit>

 VESPERSBendingMirrorsView::~VESPERSBendingMirrorsView(){}
VESPERSBendingMirrorsView::VESPERSBendingMirrorsView(QWidget *parent)
	: QWidget(parent)
{
	QHBoxLayout *labelLayout = new QHBoxLayout;
	labelLayout->addSpacing(25);
	labelLayout->addWidget(new QLabel("Name"));
	labelLayout->addSpacing(15);
	labelLayout->addWidget(new QLabel("Status"));
	labelLayout->addWidget(new QLabel("Setpoint"));
	labelLayout->addSpacing(55);
	labelLayout->addWidget(new QLabel("Jog"));
	labelLayout->addWidget(new QLabel("Position"), 0, Qt::AlignRight);
	labelLayout->addWidget(new QLabel("Stop"), 0, Qt::AlignRight);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addLayout(labelLayout);
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M3 - Pitch", "SMTR1607-2-B21-06:deg:sp", "SMTR1607-2-B21-06:deg", "SMTR1607-2-B21-06:status", "SMTR1607-2-B21-06:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M3 - Z", "SMTR1607-2-B21-05:mm:sp", "SMTR1607-2-B21-05:mm", "SMTR1607-2-B21-05:status", "SMTR1607-2-B21-05:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M3 - Upstream", "SMTR1607-2-B21-07:mm:sp", "SMTR1607-2-B21-07:mm", "SMTR1607-2-B21-07:status", "SMTR1607-2-B21-07:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M3 - Downstream", "SMTR1607-2-B21-08:mm:sp", "SMTR1607-2-B21-08:mm", "SMTR1607-2-B21-08:status", "SMTR1607-2-B21-08:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M4 - Pitch", "SMTR1607-2-B21-10:deg:sp", "SMTR1607-2-B21-10:deg", "SMTR1607-2-B21-10:status", "SMTR1607-2-B21-10:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M4 - Z", "SMTR1607-2-B21-09:mm:sp", "SMTR1607-2-B21-09:mm", "SMTR1607-2-B21-09:status", "SMTR1607-2-B21-09:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M4 - Upstream", "SMTR1607-2-B21-11:mm:sp", "SMTR1607-2-B21-11:mm", "SMTR1607-2-B21-11:status", "SMTR1607-2-B21-11:stop", this)));
	layout->addWidget(new VESPERSBendingMirrorsElementView(new AMPVwStatusControl("M4 - Downstream", "SMTR1607-2-B21-12:mm:sp", "SMTR1607-2-B21-12:mm", "SMTR1607-2-B21-12:status", "SMTR1607-2-B21-12:stop", this)));

	setLayout(layout);
}

 VESPERSBendingMirrorsElementView::~VESPERSBendingMirrorsElementView(){}
VESPERSBendingMirrorsElementView::VESPERSBendingMirrorsElementView(AMControl *control, QWidget *parent)
	: QWidget(parent)
{
	control_ = control;
	connect(control_, SIGNAL(connected(bool)), this, SLOT(onControlConnected(bool)));

	QLabel *nameLabel = new QLabel(control_->name());
	nameLabel->setFixedSize(120, 25);

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/22x22/greenLEDOff.png").pixmap(22));
	status_->setFixedSize(25, 25);
	connect(control_, SIGNAL(movingChanged(bool)), this, SLOT(onStatusChanged(bool)));

	setpoint_ = new QLineEdit;
	setpoint_->setAlignment(Qt::AlignCenter);
	setpoint_->setFixedSize(90, 25);
	connect(control_, SIGNAL(setpointChanged(double)), this, SLOT(onSetpointChanged(double)));

	jog_ = new QDoubleSpinBox;
	jog_->setRange(-10000000, 10000000);
	jog_->setSuffix(QString(" %1").arg(control_->units()));
	jog_->setDecimals(3);
	jog_->setFixedSize(90, 25);

	QToolButton *jogMinus = new QToolButton;
	jogMinus->setIcon(QIcon(":/22x22/list-remove.png"));
	jogMinus->setFixedSize(25, 25);
	connect(jogMinus, SIGNAL(clicked()), this, SLOT(onJogMinus()));

	QToolButton *jogPlus = new QToolButton;
	jogPlus->setIcon(QIcon(":/22x22/list-add.png"));
	jogPlus->setFixedSize(25, 25);
	connect(jogPlus, SIGNAL(clicked()), this, SLOT(onJogPlus()));

	feedback_ = new QLabel;
	feedback_->setText(QString("%1 %2").arg(control_->value(), 0, 'f', 3).arg(control_->units()));
	feedback_->setFixedSize(80, 25);
	connect(control_, SIGNAL(valueChanged(double)), this, SLOT(onFeedbackChanged()));

	QToolButton *stopButton = new QToolButton;
	stopButton->setIcon(QIcon(":/stop.png"));
	stopButton->setFixedSize(25, 25);
	connect(stopButton, SIGNAL(clicked()), control_, SLOT(stop()));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(nameLabel);
	layout->addWidget(status_);
	layout->addSpacing(10);
	layout->addWidget(setpoint_);
	layout->addWidget(jogMinus);
	layout->addWidget(jog_);
	layout->addWidget(jogPlus);
	layout->addWidget(feedback_);
	layout->addWidget(stopButton);

	setLayout(layout);
}

void VESPERSBendingMirrorsElementView::onControlConnected(bool isConnected)
{
	jog_->setSuffix(QString(" %1").arg(control_->units()));
	setEnabled(isConnected);
	connect(setpoint_, SIGNAL(returnPressed()), this, SLOT(onMoveRequested()));
}

void VESPERSBendingMirrorsElementView::onStatusChanged(bool moving)
{
	status_->setPixmap(moving ? QIcon(":/22x22/greenLEDOn.png").pixmap(22) : QIcon(":/22x22/greenLEDOff.png").pixmap(22));
}

void VESPERSBendingMirrorsElementView::onSetpointChanged(double val)
{
	setpoint_->setText(QString::number(val, 'f', 3));
}

void VESPERSBendingMirrorsElementView::onMoveRequested()
{
	control_->move(setpoint_->text().toDouble());
}

void VESPERSBendingMirrorsElementView::onJogMinus()
{
	control_->move(setpoint_->text().toDouble()-jog_->value());
}

void VESPERSBendingMirrorsElementView::onJogPlus()
{
	control_->move(setpoint_->text().toDouble()+jog_->value());
}

void VESPERSBendingMirrorsElementView::onFeedbackChanged()
{
	feedback_->setText(QString("%1 %2").arg(control_->value(), 0, 'f', 3).arg(control_->units()));
}
