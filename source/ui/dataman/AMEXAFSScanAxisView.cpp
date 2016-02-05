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


#include "AMEXAFSScanAxisView.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <math.h>

#include "util/AMEnergyToKSpaceCalculator.h"

// AMEXAFSScanAxisElementView
/////////////////////////////////////////////

AMEXAFSScanAxisElementView::AMEXAFSScanAxisElementView(AMScanAxisEXAFSRegion *region, QWidget *parent)
	: QWidget(parent)
{
	region_ = region;

	start_ = new QDoubleSpinBox;
	start_->setRange(-100000, 100000);
	start_->setSuffix(" eV");
	start_->setDecimals(2);
	start_->setValue(region_->inKSpace() ? double(AMEnergyToKSpaceCalculator::energy(region_->edgeEnergy(), region_->regionStart())) - double(region_->edgeEnergy()) : double(region_->regionStart()) - double(region_->edgeEnergy()));
	start_->setDisabled(region_->inKSpace());
	start_->setAlignment(Qt::AlignCenter);
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartPositionUpdated()));

	delta_ = new QDoubleSpinBox;
	delta_->setRange(-100000, 100000);
	delta_->setSingleStep(0.5);
	delta_->setSuffix(region_->inKSpace() ? "k" : " eV");
	delta_->setDecimals(2);
	delta_->setValue(double(region_->regionStep()));
	delta_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setDeltaSpinBox(AMNumber)));
	connect(delta_, SIGNAL(editingFinished()), this, SLOT(onDeltaPositionUpdated()));

	end_ = new QDoubleSpinBox;
	end_->setRange(-100000, 100000);
	end_->setSuffix(region_->inKSpace() ? "k" : " eV");
	end_->setDecimals(2);
	end_->setValue(region_->inKSpace() ? double(region_->regionEnd()) : double(region_->regionEnd()) - double(region_->edgeEnergy()));
	end_->setAlignment(Qt::AlignCenter);
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndPositionUpdated()));

	time_ = new QDoubleSpinBox;
	time_->setRange(-100000, 100000);
	time_->setSuffix(" s");
	time_->setDecimals(2);
	time_->setValue(double(region_->regionTime()));
	time_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionTimeChanged(AMNumber)), this, SLOT(setTimeSpinBox(AMNumber)));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(onTimeUpdated()));

	maximumTime_ = new QDoubleSpinBox;
	maximumTime_->setRange(-100000, 100000);
	maximumTime_->setSuffix(" s");
	maximumTime_->setDecimals(2);
	maximumTime_->setValue(region_->maximumTime());
	maximumTime_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(maximumTimeChanged(AMNumber)), this, SLOT(setMaximumTimeSpinBox(AMNumber)));
	connect(maximumTime_, SIGNAL(editingFinished()), this, SLOT(onMaximumTimeUpdated()));

	QLabel *maximumTimeLabel = new QLabel("Max Time");
	maximumTimeLabel->setVisible(region_->inKSpace());
	maximumTime_->setVisible(region_->inKSpace());

	if (region_->inKSpace())
		updateKSpaceToolTip();

	QHBoxLayout *elementViewLayout = new QHBoxLayout;
	elementViewLayout->addWidget(new QLabel("Start"), 0, Qt::AlignCenter);
	elementViewLayout->addWidget(start_);
	elementViewLayout->addWidget(new QLabel(QString::fromUtf8("Δ")), 0, Qt::AlignCenter);
	elementViewLayout->addWidget(delta_);
	elementViewLayout->addWidget(new QLabel("End"), 0, Qt::AlignCenter);
	elementViewLayout->addWidget(end_);
	elementViewLayout->addWidget(new QLabel("Time"), 0, Qt::AlignCenter);
	elementViewLayout->addWidget(time_);
	elementViewLayout->addWidget(maximumTimeLabel);
	elementViewLayout->addWidget(maximumTime_);

	setLayout(elementViewLayout);
}

void AMEXAFSScanAxisElementView::setStartSpinBox(const AMNumber &value)
{
	if (double(value) != start_->value()){

		start_->setValue(double(value));

		if (region_->inKSpace())
			region_->setRegionStart(AMEnergyToKSpaceCalculator::k(region_->edgeEnergy(), start_->value()+double(region_->edgeEnergy())));

		onStartPositionUpdated();
	}
}

void AMEXAFSScanAxisElementView::setDeltaSpinBox(const AMNumber &value)
{
	if ((double(value) - delta_->value()) < pow(1, -1*delta_->decimals())){
		delta_->setValue(double(value));
		onDeltaPositionUpdated();
	}
}

void AMEXAFSScanAxisElementView::setEndSpinBox(const AMNumber &value)
{
	if ((double(value) != end_->value())){
		end_->setValue(double(value));
		onEndPositionUpdated();
	}
}

void AMEXAFSScanAxisElementView::setTimeSpinBox(const AMNumber &value)
{
	if (double(value) != time_->value()){
		time_->setValue(double(value));
		onTimeUpdated();
	}
}

void AMEXAFSScanAxisElementView::setMaximumTimeSpinBox(const AMNumber &value)
{
	if (double(value) != maximumTime_->value()){
		maximumTime_->setValue(double(value));
		onMaximumTimeUpdated();
	}
}

void AMEXAFSScanAxisElementView::onStartPositionUpdated()
{
	if (region_->inKSpace()){

		region_->setRegionStart(AMEnergyToKSpaceCalculator::k(region_->edgeEnergy(), start_->value()+double(region_->edgeEnergy())));
		updateKSpaceToolTip();
	}

	else
		region_->setRegionStart(start_->value() + double(region_->edgeEnergy()));

	emit startValueChanged(start_->value());
}

void AMEXAFSScanAxisElementView::onDeltaPositionUpdated()
{
	region_->setRegionStep(delta_->value());
}

void AMEXAFSScanAxisElementView::onEndPositionUpdated()
{
	if (region_->inKSpace()){

		region_->setRegionEnd(end_->value());
		updateKSpaceToolTip();
	}

	else
		region_->setRegionEnd(end_->value() + double(region_->edgeEnergy()));

	emit endValueChanged(end_->value());
}

void AMEXAFSScanAxisElementView::onTimeUpdated()
{
	region_->setRegionTime(time_->value());
}

void AMEXAFSScanAxisElementView::onMaximumTimeUpdated()
{
	region_->setMaximumTime(maximumTime_->value());
}

void AMEXAFSScanAxisElementView::setEdgeEnergy(const AMNumber &energy)
{
	region_->setEdgeEnergy(energy);

	if (!region_->inKSpace()){

		region_->setRegionStart(start_->value() + double(energy));
		region_->setRegionEnd(end_->value() + double(energy));
	}

	else
		updateKSpaceToolTip();
}

void AMEXAFSScanAxisElementView::updateKSpaceToolTip()
{
	setToolTip(QString("Energy Range: %1 to %2 eV")
			   .arg(double(AMEnergyToKSpaceCalculator::energy(region_->edgeEnergy(), region_->regionStart())))
			   .arg(double(AMEnergyToKSpaceCalculator::energy(region_->edgeEnergy(), region_->regionEnd()))));
}

// AMEXAFSScanAxisView
/////////////////////////////////////////////

AMEXAFSScanAxisView::AMEXAFSScanAxisView(const QString &title, AMStepScanConfiguration *configuration, QWidget *parent)
	: QWidget(parent)
{
	configuration_ = configuration;

	deleteButtonGroup_ = new QButtonGroup;
	scanAxisViewLayout_ = new QVBoxLayout;

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

	foreach (AMScanAxisRegion *region, configuration_->scanAxisAt(0)->regions().toList()) {
		AMScanAxisEXAFSRegion *exafsRegion = qobject_cast<AMScanAxisEXAFSRegion*>(region);

		if (exafsRegion)
			buildScanAxisRegionView(configuration_->scanAxisAt(0)->regionCount(), exafsRegion);
	}

	connect(deleteButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDeleteButtonClicked(QAbstractButton*)));

	connectRegions();

	setLayout(scanAxisViewLayout_);
}

AMEXAFSScanAxisView::~AMEXAFSScanAxisView()
{

}

void AMEXAFSScanAxisView::onLockRegionsToggled(bool toggled)
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

void AMEXAFSScanAxisView::connectRegions() const
{
	for (int i = 0, size = lockedElementViewList_.size(); i < size; i++){

		if (i != 0)
			connect(lockedElementViewList_.at(i), SIGNAL(startValueChanged(AMNumber)), lockedElementViewList_.at(i-1), SLOT(setEndSpinBox(AMNumber)));

		if (i != (size-1))
			connect(lockedElementViewList_.at(i), SIGNAL(endValueChanged(AMNumber)), lockedElementViewList_.at(i+1), SLOT(setStartSpinBox(AMNumber)));
	}
}

void AMEXAFSScanAxisView::disconnectRegions() const
{
	for (int i = 0, size = lockedElementViewList_.size(); i < size; i++){

		if (i != 0)
			disconnect(lockedElementViewList_.at(i), SIGNAL(startValueChanged(AMNumber)), lockedElementViewList_.at(i-1), SLOT(setEndSpinBox(AMNumber)));

		if (i != (size-1))
			disconnect(lockedElementViewList_.at(i), SIGNAL(endValueChanged(AMNumber)), lockedElementViewList_.at(i+1), SLOT(setStartSpinBox(AMNumber)));
	}
}

void AMEXAFSScanAxisView::onAddRegionButtonClicked()
{
	if (configuration_->scanAxisAt(0)->regionCount() == 0){

		QMenu menu(addRegionButton_);
		menu.addAction("Add energy region...");
		menu.addAction("Add k-space region...");
		QAction *temp = menu.exec(mapToGlobal(addRegionButton_->pos()));

		if (temp){

			AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion(-1, -1, -1, -1, temp->text().contains("k-space"), -1);
			configuration_->scanAxisAt(0)->appendRegion(region);
			buildScanAxisRegionView(0, region);
		}
	}

	else {

		QMenu menu(addRegionButton_);
		QAction *temp = menu.addAction("Add region to...");
		temp->setDisabled(true);

		menu.addAction("Beginning as energy");

		for (int i = 1, size = configuration_->scanAxisAt(0)->regionCount(); i < size; i++)
			menu.addAction(QString("Between %1 and %2 as energy").arg(i).arg(i+1));

		menu.addAction("End as energy");
		menu.addSeparator();
		menu.addAction("Beginning in k-space");

		for (int i = 1, size = configuration_->scanAxisAt(0)->regionCount(); i < size; i++)
			menu.addAction(QString("Between %1 and %2 in k-space").arg(i).arg(i+1));

		menu.addAction("End in k-space");

		temp = menu.exec(mapToGlobal(addRegionButton_->pos()));

		if (temp){

			if (regionsLocked())
				disconnectRegions();

			if (temp->text().contains("Beginning")){

				AMScanAxisEXAFSRegion *region = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(0)->createCopy());
				region->setRegionEnd(configuration_->scanAxisAt(0)->regionAt(0)->regionStart());
				region->setRegionStart(double(region->regionEnd())-10.0);
				region->setInKSpace(temp->text().contains("k-space"));
				configuration_->scanAxisAt(0)->insertRegion(0, region);
				buildScanAxisRegionView(0, region);
			}

			else if (temp->text().contains("End")){

				int indexOfEnd = configuration_->scanAxisAt(0)->regionCount();
				AMScanAxisEXAFSRegion *region = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(indexOfEnd-1)->createCopy());
				region->setRegionStart(configuration_->scanAxisAt(0)->regionAt(indexOfEnd-1)->regionEnd());
				region->setRegionEnd(double(region->regionEnd())+10.0);
				region->setInKSpace(temp->text().contains("k-space"));
				configuration_->scanAxisAt(0)->insertRegion(indexOfEnd, region);
				buildScanAxisRegionView(indexOfEnd, region);
			}

			else {

				int index = temp->text().split(" ").at(1).toInt();
				AMScanAxisEXAFSRegion *region = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(index)->createCopy());
				region->setRegionStart(configuration_->scanAxisAt(0)->regionAt(index-1)->regionEnd());
				region->setRegionEnd(configuration_->scanAxisAt(0)->regionAt(index)->regionStart());
				region->setInKSpace(temp->text().contains("k-space"));
				configuration_->scanAxisAt(0)->insertRegion(index, region);
				buildScanAxisRegionView(index, region);
			}

			if (regionsLocked())
				connectRegions();
		}
	}
}

void AMEXAFSScanAxisView::onDeleteButtonClicked(QAbstractButton *button)
{
	if (button) {
		if (regionsLocked())
			disconnectRegions();

		AMEXAFSScanAxisElementView *view = regionMap_.value(button);
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
}

void AMEXAFSScanAxisView::insertEXAFSRegion(int index, AMScanAxisEXAFSRegion *region)
{
	if (regionsLocked())
		disconnectRegions();

	configuration_->scanAxisAt(0)->insertRegion(index, region);
	buildScanAxisRegionView(index, region);

	if (regionsLocked())
		connectRegions();
}

void AMEXAFSScanAxisView::removeEXAFSRegion(int index)
{
	if (regionsLocked())
		disconnectRegions();


	onDeleteButtonClicked(regionMap_.key(lockedElementViewList_.at(index), 0));

	if (regionsLocked())
		connectRegions();
}

void AMEXAFSScanAxisView::buildScanAxisRegionView(int index, AMScanAxisEXAFSRegion *region)
{
	AMEXAFSScanAxisElementView *elementView = new AMEXAFSScanAxisElementView(region);

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

void AMEXAFSScanAxisView::setEdgeEnergy(const AMNumber &energy)
{
	foreach (AMEXAFSScanAxisElementView *view, regionMap_.values())
		view->setEdgeEnergy(energy);
}
