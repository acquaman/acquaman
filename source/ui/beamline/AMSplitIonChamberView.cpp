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


#include "AMSplitIonChamberView.h"

#include <QMenu>
#include <QAction>
#include <QGridLayout>

AMSplitIonChamberView::AMSplitIonChamberView(AMSplitIonChamber *chamber, QWidget *parent)
	: AMIonChamberView(chamber, parent)
{
	isLocked_ = true;

	minusA_ = new QToolButton;
	minusA_->setMaximumSize(25, 25);
	minusA_->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minusA_, SIGNAL(clicked()), splitIonChamberAM(), SLOT(decreaseSensitivityA()));
	connect(splitIonChamberAM(), SIGNAL(minimumSensitivityA(bool)), minusA_, SLOT(setDisabled(bool)));

	plusA_ = new QToolButton;
	plusA_->setMaximumSize(25, 25);
	plusA_->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plusA_, SIGNAL(clicked()), splitIonChamberAM(), SLOT(increaseSensitivityA()));
	connect(splitIonChamberAM(), SIGNAL(maximumSensitivityA(bool)), plusA_, SLOT(setDisabled(bool)));

	minusB_ = new QToolButton;
	minusB_->setMaximumSize(25, 25);
	minusB_->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minusB_, SIGNAL(clicked()), splitIonChamberAM(), SLOT(decreaseSensitivityB()));
	connect(splitIonChamberAM(), SIGNAL(minimumSensitivityB(bool)), minusB_, SLOT(setDisabled(bool)));

	plusB_ = new QToolButton;
	plusB_->setMaximumSize(25, 25);
	plusB_->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plusB_, SIGNAL(clicked()), splitIonChamberAM(), SLOT(increaseSensitivityA()));
	connect(splitIonChamberAM(), SIGNAL(maximumSensitivityB(bool)), plusB_, SLOT(setDisabled(bool)));

	minusA_->hide();
	plusA_->hide();
	minusB_->hide();
	plusB_->hide();

	QGridLayout *plusMinusButtons = new QGridLayout;
	plusMinusButtons->addWidget(minusA_, 0, 0);
	plusMinusButtons->addWidget(plusA_, 0, 1);
	plusMinusButtons->addWidget(minusB_, 1, 0);
	plusMinusButtons->addWidget(plusB_, 1, 1);

	ionChamberViewLayout_->insertLayout(3, plusMinusButtons);
}

void AMSplitIonChamberView::onReadingsChanged()
{
	QFont font(this->font());
	QPalette palette(this->palette());

	switch(state_){

	case Counts:
		font.setBold(false);
		font.setPointSize(9);
		palette.setColor(QPalette::WindowText, Qt::black);
		output_->setText(QString::number(splitIonChamberAM()->countsA()) + " counts\n" + QString::number(splitIonChamberAM()->countsB()) + "counts");
		break;

	case Voltage:
		font.setBold(false);
		font.setPointSize(9);
		palette.setColor(QPalette::WindowText, Qt::black);
		output_->setText(QString::number(splitIonChamberAM()->voltageA()) + " V\n" + QString::number(splitIonChamberAM()->voltageB()) + " V");
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

void AMSplitIonChamberView::onModeViewChanged()
{
	if (isLocked_){

		minusA_->hide();
		plusA_->hide();
		minusB_->hide();
		plusB_->hide();

		minus_->show();
		plus_->show();
	}
	else {

		minus_->hide();
		plus_->hide();

		minusA_->show();
		plusA_->show();
		minusB_->show();
		plusB_->show();
	}
}

void AMSplitIonChamberView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Status View");
	temp->setDisabled(state_ == Status);

	temp = popup.addAction("Counts View");
	temp->setDisabled(state_ == Counts);

	temp = popup.addAction("Voltage View");
	temp->setDisabled(state_ == Voltage);

	popup.addSeparator();

	temp = popup.addAction("Lock Sensitivity");
	temp->setCheckable(true);
	temp->setChecked(isLocked_);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Status View"){

			state_ = Status;
			onReadingsChanged();
		}
		else if (temp->text() == "Counts View"){

			state_ = Counts;
			onReadingsChanged();
		}
		else if (temp->text() == "Voltage View"){

			state_ = Voltage;
			onReadingsChanged();
		}
		else if (temp->text() == "Lock Sensitivity"){

			isLocked_ = temp->isChecked();
			onModeViewChanged();
		}
	}
}
