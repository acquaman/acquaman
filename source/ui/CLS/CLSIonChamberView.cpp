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


#include "CLSIonChamberView.h"

#include <QAction>
#include <QMenu>

CLSIonChamberView::CLSIonChamberView(CLSIonChamber *ionChamber, QWidget *parent)
	: AMIonChamberView(ionChamber, parent)
{
	isBasic_ = true;

	value_ = new QComboBox;
	value_->hide();
	value_->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500");
	connect(value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)));
	connect(ionChamberCLS(), SIGNAL(sensitivityValueChanged(int)), this, SLOT(onValueChanged(int)));

	units_ = new QComboBox;
	units_->hide();
	units_->addItems(QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V");
	connect(units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)));
	connect(ionChamberCLS(), SIGNAL(sensitivityUnitsChanged(QString)), this, SLOT(onUnitsChanged(QString)));

	ionChamberViewLayout_->insertWidget(3, value_);
	ionChamberViewLayout_->insertWidget(4, units_);
}

void CLSIonChamberView::onValueChanged(int value)
{
	chamber_->blockSignals(true);

	switch(value){

	case 1:
		value_->setCurrentIndex(0);
		break;
	case 2:
		value_->setCurrentIndex(1);
		break;
	case 5:
		value_->setCurrentIndex(2);
		break;
	case 10:
		value_->setCurrentIndex(3);
		break;
	case 20:
		value_->setCurrentIndex(4);
		break;
	case 50:
		value_->setCurrentIndex(5);
		break;
	case 100:
		value_->setCurrentIndex(6);
		break;
	case 200:
		value_->setCurrentIndex(7);
		break;
	case 500:
		value_->setCurrentIndex(8);
		break;
	}

	chamber_->blockSignals(false);
}

void CLSIonChamberView::onUnitsChanged(QString units)
{
	chamber_->blockSignals(true);

	if (units == "pA/V")
		units_->setCurrentIndex(0);
	else if (units == "nA/V")
		units_->setCurrentIndex(1);
	else if (units == "uA/V")
		units_->setCurrentIndex(2);
	else if (units == "mA/V")
		units_->setCurrentIndex(3);

	chamber_->blockSignals(false);
}

void CLSIonChamberView::onModeViewChanged()
{
	if (isBasic_){

		value_->hide();
		units_->hide();
		plus_->show();
		minus_->show();
	}
	else {

		plus_->hide();
		minus_->hide();
		value_->show();
		units_->show();
	}
}

void CLSIonChamberView::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Basic View");
	if (isBasic_)
		temp->setDisabled(true);

	temp = popup.addAction("Advanced View");
	if (!isBasic_)
		temp->setDisabled(true);

	popup.addSeparator();

	temp = popup.addAction("Status View");
	if (state_ == Status)
		temp->setDisabled(true);

	temp = popup.addAction("Counts View");
	if (state_ == Counts)
		temp->setDisabled(true);

	temp = popup.addAction("Voltage View");
	if (state_ == Voltage)
		temp->setDisabled(true);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Basic View"){

			isBasic_ = true;
			onModeViewChanged();
		}
		else if (temp->text() == "Advanced View"){

			isBasic_ = false;
			onModeViewChanged();
		}
		else if (temp->text() == "Status View"){

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
	}
}
