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


#include "CLSSplitIonChamberView.h"

#include <QMenu>
#include <QAction>
#include <QGridLayout>

CLSSplitIonChamberView::CLSSplitIonChamberView(CLSSplitIonChamber *chamber, QWidget *parent)
	: AMSplitIonChamberView(chamber, parent)
{
	isBasic_ = true;

	value_ = new QComboBox;
	value_->hide();
	value_->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500");
	connect(value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)));
	connect(splitIonChamberCLS(), SIGNAL(sensitivityValueIndexAChanged(int)), this, SLOT(onValueChanged(int)));

	units_ = new QComboBox;
	units_->hide();
	units_->addItems(QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V");
	connect(units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)));
	connect(splitIonChamberCLS(), SIGNAL(sensitivityUnitsAChanged(QString)), this, SLOT(onUnitsChanged(QString)));

	valueA_ = new QComboBox;
	valueA_->hide();
	valueA_->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500");
	connect(valueA_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueAComboBoxChanged(int)));
	connect(splitIonChamberCLS(), SIGNAL(sensitivityValueIndexAChanged(int)), this, SLOT(onValueAChanged(int)));

	unitsA_ = new QComboBox;
	unitsA_->hide();
	unitsA_->addItems(QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V");
	connect(unitsA_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsAComboBoxChanged(int)));
	connect(splitIonChamberCLS(), SIGNAL(sensitivityUnitsAChanged(QString)), this, SLOT(onUnitsAChanged(QString)));

	valueB_ = new QComboBox;
	valueB_->hide();
	valueB_->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500");
	connect(valueB_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueBComboBoxChanged(int)));
	connect(splitIonChamberCLS(), SIGNAL(sensitivityValueIndexBChanged(int)), this, SLOT(onValueBChanged(int)));

	unitsB_ = new QComboBox;
	unitsB_->hide();
	unitsB_->addItems(QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V");
	connect(unitsB_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)));
	connect(splitIonChamberCLS(), SIGNAL(sensitivityUnitsBChanged(QString)), this, SLOT(onUnitsBChanged(QString)));

	QGridLayout *valueUnitsBoxes = new QGridLayout;
	valueUnitsBoxes->addWidget(valueA_, 0, 0);
	valueUnitsBoxes->addWidget(unitsA_, 0, 1);
	valueUnitsBoxes->addWidget(valueB_, 1, 0);
	valueUnitsBoxes->addWidget(unitsB_, 1, 1);

	ionChamberViewLayout_->insertWidget(3, value_);
	ionChamberViewLayout_->insertWidget(4, units_);
	ionChamberViewLayout_->insertLayout(5, valueUnitsBoxes);
}

void CLSSplitIonChamberView::onValueChanged(int value)
{
	if (isLocked_){

		setValueComboBox(value_, value);
		onValueAChanged(value);
		onValueBChanged(value);
	}
}

void CLSSplitIonChamberView::onUnitsChanged(QString units)
{
	if (isLocked_){

		setUnitsComboBox(units_, units);
		onUnitsAChanged(units);
		onUnitsBChanged(units);
	}
}

void CLSSplitIonChamberView::onModeViewChanged()
{
	value_->hide();
	units_->hide();

	minusA_->hide();
	plusA_->hide();
	minusB_->hide();
	plusB_->hide();

	valueA_->hide();
	unitsA_->hide();
	valueB_->hide();
	unitsB_->hide();

	plus_->hide();
	minus_->hide();

	if (isBasic_ && isLocked_){

		plus_->show();
		minus_->show();
		state_ = Status;
		onReadingsChanged();
	}
	else if (isBasic_ && !isLocked_){

		minusA_->show();
		plusA_->show();
		minusB_->show();
		plusB_->show();
		state_ = Status;
		onReadingsChanged();
	}
	else if (!isBasic_ && isLocked_){

		value_->show();
		units_->show();
		state_ = Voltage;
		onReadingsChanged();
	}
	else if (!isBasic_ && !isLocked_){

		valueA_->show();
		unitsA_->show();
		valueB_->show();
		unitsB_->show();
		state_ = Voltage;
		onReadingsChanged();
	}
}

void CLSSplitIonChamberView::onCustomContextMenuRequested(QPoint pos)
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

	popup.addSeparator();

	temp = popup.addAction("Lock Sensitivity");
	temp->setCheckable(true);
	temp->setChecked(isLocked_);

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
		else if (temp->text() == "Lock Sensitivity"){

			isLocked_ = temp->isChecked();

			if (isLocked_){

				onValueChanged(splitIonChamberCLS()->sensitivityValueA());
				onUnitsChanged(splitIonChamberCLS()->sensitivityUnitsA());
			}

			onModeViewChanged();
		}
	}
}

void CLSSplitIonChamberView::setValueComboBox(QComboBox *valueBox, int value)
{
	chamber_->blockSignals(true);
	valueBox->setCurrentIndex(value);
	chamber_->blockSignals(false);
}

void CLSSplitIonChamberView::setUnitsComboBox(QComboBox *unitsBox, QString units)
{
	chamber_->blockSignals(true);

	if (units == "pA/V")
		unitsBox->setCurrentIndex(0);
	else if (units == "nA/V")
		unitsBox->setCurrentIndex(1);
	else if (units == "uA/V")
		unitsBox->setCurrentIndex(2);
	else if (units == "mA/V")
		unitsBox->setCurrentIndex(3);

	chamber_->blockSignals(false);
}
