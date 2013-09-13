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


#include "AMIonChamberView.h"

#include <QMenu>
#include <QAction>

AMIonChamberView::AMIonChamberView(AMIonChamber *chamber, QWidget *parent)
	: QWidget(parent)
{
	chamber_ = chamber;
	state_ = Status;
	tooHigh_ = "Too High!";
	tooLow_ = "Too Low!";
	withinRange_ = "Good!";

	name_ = new QLabel(chamber_->description());

	minus_ = new QToolButton;
	minus_->setMaximumSize(25, 25);
	minus_->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minus_, SIGNAL(clicked()), chamber_, SLOT(decreaseSensitivity()));
	connect(chamber_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)));

	plus_ = new QToolButton;
	plus_->setMaximumSize(25, 25);
	plus_->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plus_, SIGNAL(clicked()), chamber_, SLOT(increaseSensitivity()));
	connect(chamber_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)));

	output_ = new QLabel;
	output_->setFixedWidth(100);

	connect(chamber_, SIGNAL(readingsChanged()), this, SLOT(onReadingsChanged()));
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	setContextMenuPolicy(Qt::CustomContextMenu);

	ionChamberViewLayout_ = new QHBoxLayout;
	ionChamberViewLayout_->addWidget(name_, 0, Qt::AlignCenter);
	ionChamberViewLayout_->addWidget(minus_);
	ionChamberViewLayout_->addWidget(plus_);
	ionChamberViewLayout_->addWidget(output_, 0, Qt::AlignCenter);

	setLayout(ionChamberViewLayout_);
}

void AMIonChamberView::onReadingsChanged()
{
	QFont font(this->font());
	QPalette palette(this->palette());

	switch(state_){

	case Counts:
		font.setBold(false);
		font.setPointSize(9);
		palette.setColor(QPalette::WindowText, Qt::black);
		output_->setText(QString::number(chamber_->reading()) + " counts");
		break;

	case Voltage:
		font.setBold(false);
		font.setPointSize(9);
		palette.setColor(QPalette::WindowText, Qt::black);
		output_->setText(QString::number(chamber_->voltage()) + " V");
		break;

	case Status:

		font.setBold(true);
		font.setPointSize(12);

		if (chamber_->withinLinearRange()){

			palette.setColor(QPalette::WindowText, Qt::darkGreen);
			output_->setText(withinRange_);
		}
		else if (chamber_->voltageTooHigh()){

			palette.setColor(QPalette::WindowText, Qt::red);
			output_->setText(tooHigh_);
		}
		else if (chamber_->voltageTooLow()){

			palette.setColor(QPalette::WindowText, Qt::red);
			output_->setText(tooLow_);
		}
		else{

			palette.setColor(QPalette::WindowText, Qt::blue);
			output_->setText("Invalid");
		}

		break;
	}

	output_->setPalette(palette);
	output_->setFont(font);
}

void AMIonChamberView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Status View");
	temp->setDisabled(state_ == Status);

	temp = popup.addAction("Counts View");
	temp->setDisabled(state_ == Counts);

	temp = popup.addAction("Voltage View");
	temp->setDisabled(state_ == Voltage);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Status View")
			state_ = Status;
		else if (temp->text() == "Counts View")
			state_ = Counts;
		else if (temp->text() == "Voltage View")
			state_ = Voltage;

		onReadingsChanged();
	}
}
