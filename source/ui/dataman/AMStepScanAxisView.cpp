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


#include "AMStepScanAxisView.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

// AMStepScanAxisElementView
/////////////////////////////////////////////

AMStepScanAxisElementView::AMStepScanAxisElementView(AMScanAxisRegion *region, QWidget *parent, QString startLabel, QString endLabel)
	: QWidget(parent)
{
	region_ = region;

	start_ = new QDoubleSpinBox;
	start_->setRange(0, 100000);
	start_->setSuffix(" eV");
    start_->setDecimals(3);
	start_->setValue(double(region_->regionStart()));
	start_->setAlignment(Qt::AlignCenter);
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartPositionUpdated()));

	delta_ = new QDoubleSpinBox;
	delta_->setRange(0, 100000);
	delta_->setSingleStep(0.05);
	delta_->setSuffix(" eV");
    delta_->setDecimals(3);
	delta_->setValue(double(region_->regionStep()));
	delta_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setDeltaSpinBox(AMNumber)));
	connect(delta_, SIGNAL(editingFinished()), this, SLOT(onDeltaPositionUpdated()));

	end_ = new QDoubleSpinBox;
	end_->setRange(0, 100000);
	end_->setSuffix(" eV");
    end_->setDecimals(3);
	end_->setValue(double(region_->regionEnd()));
	end_->setAlignment(Qt::AlignCenter);
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndPositionUpdated()));

	time_ = new QDoubleSpinBox;
	time_->setRange(0, 100000);
	time_->setSuffix(" s");
	time_->setDecimals(2);
	time_->setValue(double(region_->regionTime()));
	time_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setTimeSpinBox(AMNumber)));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(onTimeUpdated()));

	QHBoxLayout *elementViewLayout = new QHBoxLayout;
	elementViewLayout->addWidget(new QLabel(startLabel));
	elementViewLayout->addWidget(start_);
	elementViewLayout->addWidget(new QLabel(QString::fromUtf8("Δ")));
	elementViewLayout->addWidget(delta_);
	elementViewLayout->addWidget(new QLabel(endLabel));
	elementViewLayout->addWidget(end_);
	elementViewLayout->addWidget(new QLabel("Time"));
	elementViewLayout->addWidget(time_);

	setLayout(elementViewLayout);
}

void AMStepScanAxisElementView::setStartSpinBox(const AMNumber &value)
{
	if (double(value) != start_->value()){
		start_->setValue(double(value));
		onStartPositionUpdated();
	}
}

void AMStepScanAxisElementView::setDeltaSpinBox(const AMNumber &value)
{
	if (double(value) != delta_->value()){
		delta_->setValue(double(value));
		onDeltaPositionUpdated();
		}
}

void AMStepScanAxisElementView::setEndSpinBox(const AMNumber &value)
{
	if (double(value) != end_->value()){
		end_->setValue(double(value));
		onEndPositionUpdated();
	}
}

void AMStepScanAxisElementView::setTimeSpinBox(const AMNumber &value)
{
	if (double(value) != time_->value()){
		time_->setValue(double(value));
		onTimeUpdated();
	}
}

void AMStepScanAxisElementView::onStartPositionUpdated()
{
	region_->setRegionStart(start_->value());
	emit startValueChanged(start_->value());
}

void AMStepScanAxisElementView::onDeltaPositionUpdated()
{
	region_->setRegionStep(delta_->value());
}

void AMStepScanAxisElementView::onEndPositionUpdated()
{
	region_->setRegionEnd(end_->value());
	emit endValueChanged(end_->value());
}

void AMStepScanAxisElementView::onTimeUpdated()
{
	region_->setRegionTime(time_->value());
}

// AMStepScanAxisView
/////////////////////////////////////////////

AMStepScanAxisView::AMStepScanAxisView(const QString &title, AMStepScanConfiguration *configuration, QWidget *parent, QString startLabel, QString endLabel)
	: QWidget(parent)
{
	configuration_ = configuration;

	deleteButtonGroup_ = new QButtonGroup;
	scanAxisViewLayout_ = new QVBoxLayout;

	startLabelString_ = startLabel;
	endLabelString_ = endLabel;

	addRegionButton_ = new QToolButton;
	addRegionButton_->setIcon(QIcon(":22x22/list-add.png"));
	connect(addRegionButton_, SIGNAL(clicked()), this, SLOT(onAddRegionButtonClicked()));

	lockRegionsButton_ = new QToolButton;
	lockRegionsButton_->setCheckable(true);
	onLockRegionsToggled(false);
	connect(lockRegionsButton_, SIGNAL(toggled(bool)), this, SLOT(onLockRegionsToggled(bool)));

	QHBoxLayout *topRowLayout = new QHBoxLayout;
	topRowLayout->addWidget(new QLabel(title));
	topRowLayout->addStretch();
	topRowLayout->addWidget(lockRegionsButton_);
	topRowLayout->addWidget(addRegionButton_);
	scanAxisViewLayout_->addLayout(topRowLayout);

	foreach (AMScanAxisRegion *region, configuration_->scanAxisAt(0)->regions().toList())
		buildScanAxisRegionView(configuration_->scanAxisAt(0)->regionCount(), region);

	connect(deleteButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDeleteButtonClicked(QAbstractButton*)));

	connectRegions();

	setLayout(scanAxisViewLayout_);
}

void AMStepScanAxisView::onLockRegionsToggled(bool toggled)
{
	if (toggled){

		lockRegionsButton_->setIcon(QIcon(":22x22/unlock.png"));
		lockRegionsButton_->setToolTip("Lock regions together.");
		disconnectRegions();
	}

	else {

		lockRegionsButton_->setIcon(QIcon(":/22x22/lock.png"));
		lockRegionsButton_->setToolTip("Unlock the regions from each other.");
		connectRegions();
	}
}

void AMStepScanAxisView::connectRegions() const
{
	for (int i = 0, size = lockedElementViewList_.size(); i < size; i++){

		if (i != 0)
			connect(lockedElementViewList_.at(i), SIGNAL(startValueChanged(AMNumber)), lockedElementViewList_.at(i-1), SLOT(setEndSpinBox(AMNumber)));

		if (i != (size-1))
			connect(lockedElementViewList_.at(i), SIGNAL(endValueChanged(AMNumber)), lockedElementViewList_.at(i+1), SLOT(setStartSpinBox(AMNumber)));
	}
}

void AMStepScanAxisView::disconnectRegions() const
{
	for (int i = 0, size = lockedElementViewList_.size(); i < size; i++){

		if (i != 0)
			disconnect(lockedElementViewList_.at(i), SIGNAL(startValueChanged(AMNumber)), lockedElementViewList_.at(i-1), SLOT(setEndSpinBox(AMNumber)));

		if (i != (size-1))
            disconnect(lockedElementViewList_.at(i), SIGNAL(endValueChanged(AMNumber)), lockedElementViewList_.at(i+1), SLOT(setStartSpinBox(AMNumber)));
	}
}

void AMStepScanAxisView::onAddRegionButtonClicked()
{
	if (configuration_->scanAxisAt(0)->regionCount() == 0){

		AMScanAxisRegion *region = new AMScanAxisRegion(-1, -1, -1, -1);
		configuration_->scanAxisAt(0)->appendRegion(region);
		buildScanAxisRegionView(0, region);
	}

	else {

		QMenu menu(addRegionButton_);
		QAction *temp = menu.addAction("Add region to...");
		temp->setDisabled(true);

		menu.addAction("Beginning");

		for (int i = 1, size = configuration_->scanAxisAt(0)->regionCount(); i < size; i++)
			menu.addAction(QString("Between %1 and %2").arg(i).arg(i+1));

		menu.addAction("End");

		temp = menu.exec(mapToGlobal(addRegionButton_->pos()));

		if (temp){

			if (regionsLocked())
				disconnectRegions();

			if (temp->text() == "Beginning"){

				AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(0)->createCopy();
				region->setRegionEnd(configuration_->scanAxisAt(0)->regionAt(0)->regionStart());
				region->setRegionStart(double(region->regionEnd())-10.0);
				configuration_->scanAxisAt(0)->insertRegion(0, region);
				buildScanAxisRegionView(0, region);
			}

			else if (temp->text() == "End"){

				int indexOfEnd = configuration_->scanAxisAt(0)->regionCount();
				AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(indexOfEnd-1)->createCopy();
				region->setRegionStart(configuration_->scanAxisAt(0)->regionAt(indexOfEnd-1)->regionEnd());
				region->setRegionEnd(double(region->regionEnd())+10.0);
				configuration_->scanAxisAt(0)->insertRegion(indexOfEnd, region);
				buildScanAxisRegionView(indexOfEnd, region);
			}

			else {

				int index = temp->text().split(" ").at(1).toInt();
				AMScanAxisRegion *region = configuration_->scanAxisAt(0)->regionAt(index)->createCopy();
				region->setRegionStart(configuration_->scanAxisAt(0)->regionAt(index-1)->regionEnd());
				region->setRegionEnd(configuration_->scanAxisAt(0)->regionAt(index)->regionStart());
				configuration_->scanAxisAt(0)->insertRegion(index, region);
				buildScanAxisRegionView(index, region);
			}

			if (regionsLocked())
				connectRegions();
		}
	}
}

void AMStepScanAxisView::onDeleteButtonClicked(QAbstractButton *button)
{
	if (regionsLocked())
		disconnectRegions();

	AMStepScanAxisElementView *view = regionMap_.value(button);
	configuration_->scanAxisAt(0)->removeRegion(view->region());
	deleteButtonGroup_->removeButton(button);
	regionMap_.remove(button);
	lockedElementViewList_.removeOne(view);
	layout()->removeItem(layoutMap_.value(button));
	layoutMap_.take(button)->deleteLater();
	view->region()->deleteLater();
	view->deleteLater();
	button->deleteLater();

	if (regionsLocked())
		connectRegions();
}

void AMStepScanAxisView::insertRegion(int index, AMScanAxisRegion *region)
{
	if (regionsLocked())
		disconnectRegions();

	configuration_->scanAxisAt(0)->insertRegion(index, region);
	buildScanAxisRegionView(index, region);

	if (regionsLocked())
		connectRegions();
}

void AMStepScanAxisView::removeRegion(int index)
{
	if (regionsLocked())
		disconnectRegions();

	onDeleteButtonClicked(regionMap_.key(lockedElementViewList_.at(index)));

	if (regionsLocked())
		connectRegions();
}

void AMStepScanAxisView::buildScanAxisRegionView(int index, AMScanAxisRegion *region)
{
	AMStepScanAxisElementView *elementView = new AMStepScanAxisElementView(region, 0, startLabelString_, endLabelString_);

	QToolButton *deleteButton = new QToolButton;
	deleteButton->setIcon(QIcon(":22x22/list-remove-2.png"));
	deleteButton->setToolTip("Delete region");
	deleteButton->setAutoRaise(true);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(elementView);
	layout->addWidget(deleteButton);

	// Need to account for the title row.
	scanAxisViewLayout_->insertLayout(index+1, layout);
	deleteButtonGroup_->addButton(deleteButton);
	regionMap_.insert(deleteButton, elementView);
	layoutMap_.insert(deleteButton, layout);
	lockedElementViewList_.insert(index, elementView);
}
