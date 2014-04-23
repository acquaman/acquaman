#include "AMEXAFSScanAxisView.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

// AMEXAFSScanAxisElementView
/////////////////////////////////////////////

AMEXAFSScanAxisElementView::AMEXAFSScanAxisElementView(AMScanAxisEXAFSRegion *region, QWidget *parent)
	: QWidget(parent)
{
	region_ = region;

	inKSpace_ = new QCheckBox;
	inKSpace_->setText("k-space");
	inKSpace_->setChecked(region_->inKSpace());
	connect(region_, SIGNAL(inKSpaceChanged(bool)), inKSpace_, SLOT(setChecked(bool)));
	connect(inKSpace_, SIGNAL(toggled(bool)), this, SLOT(onInKSpaceUpdated(bool)));

	start_ = new QDoubleSpinBox;
	start_->setRange(-100000, 100000);
	start_->setSuffix(region_->inKSpace() ? "k" : " eV");
	start_->setDecimals(2);
	start_->setValue(region_->regionStart());
	start_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionStartChanged(AMNumber)), this, SLOT(setStartSpinBox(AMNumber)));
	connect(start_, SIGNAL(editingFinished()), this, SLOT(onStartPositionUpdated()));

	delta_ = new QDoubleSpinBox;
	delta_->setRange(-100000, 100000);
	delta_->setSuffix(region_->inKSpace() ? "k" : " eV");
	delta_->setDecimals(2);
	delta_->setValue(region_->regionStep());
	delta_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionStepChanged(AMNumber)), this, SLOT(setDeltaSpinBox(AMNumber)));
	connect(delta_, SIGNAL(editingFinished()), this, SLOT(onDeltaPositionUpdated()));

	end_ = new QDoubleSpinBox;
	end_->setRange(-100000, 100000);
	end_->setSuffix(region_->inKSpace() ? "k" : " eV");
	end_->setDecimals(2);
	end_->setValue(region_->regionEnd());
	end_->setAlignment(Qt::AlignCenter);
	connect(region_, SIGNAL(regionEndChanged(AMNumber)), this, SLOT(setEndSpinBox(AMNumber)));
	connect(end_, SIGNAL(editingFinished()), this, SLOT(onEndPositionUpdated()));

	time_ = new QDoubleSpinBox;
	time_->setRange(-100000, 100000);
	time_->setSuffix(" s");
	time_->setDecimals(2);
	time_->setValue(region_->regionTime());
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


	QHBoxLayout *elementViewLayout = new QHBoxLayout;
	elementViewLayout->addWidget(inKSpace_);
	elementViewLayout->addWidget(new QLabel("Start"));
	elementViewLayout->addWidget(start_);
	elementViewLayout->addWidget(new QLabel(QString::fromUtf8("Δ")));
	elementViewLayout->addWidget(delta_);
	elementViewLayout->addWidget(new QLabel("End"));
	elementViewLayout->addWidget(end_);
	elementViewLayout->addWidget(new QLabel("Time"));
	elementViewLayout->addWidget(time_);
	elementViewLayout->addWidget(new QLabel("Max Time"));
	elementViewLayout->addWidget(maximumTime_);

	setLayout(elementViewLayout);
}

void AMEXAFSScanAxisElementView::setStartSpinBox(const AMNumber &value)
{
	start_->setValue(double(value));
}

void AMEXAFSScanAxisElementView::setDeltaSpinBox(const AMNumber &value)
{
	delta_->setValue(double(value));
}

void AMEXAFSScanAxisElementView::setEndSpinBox(const AMNumber &value)
{
	end_->setValue(double(value));
}

void AMEXAFSScanAxisElementView::setTimeSpinBox(const AMNumber &value)
{
	time_->setValue(double(value));
}

void AMEXAFSScanAxisElementView::setMaximumTimeSpinBox(const AMNumber &value)
{
	maximumTime_->setValue(double(value));
}

void AMEXAFSScanAxisElementView::onInKSpaceUpdated(bool inKSpace)
{
	region_->setInKSpace(inKSpace);

	if (inKSpace){

		start_->setSuffix("k");
		delta_->setSuffix("k");
		end_->setSuffix("k");
		maximumTime_->show();
	}

	else {

		start_->setSuffix(" eV");
		delta_->setSuffix(" eV");
		end_->setSuffix(" eV");
		maximumTime_->hide();
	}
}

void AMEXAFSScanAxisElementView::onStartPositionUpdated()
{
	region_->setRegionStart(start_->value());
}

void AMEXAFSScanAxisElementView::onDeltaPositionUpdated()
{
	region_->setRegionStep(delta_->value());
}

void AMEXAFSScanAxisElementView::onEndPositionUpdated()
{
	region_->setRegionEnd(end_->value());
}

void AMEXAFSScanAxisElementView::onTimeUpdated()
{
	region_->setRegionTime(time_->value());
}

void AMEXAFSScanAxisElementView::onMaximumTimeUpdated()
{
	region_->setMaximumTime(maximumTime_->value());
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

	foreach (AMScanAxisRegion *region, configuration_->scanAxisAt(0)->regions().toList())
		buildScanAxisRegionView(configuration_->scanAxisAt(0)->regionCount(), qobject_cast<AMScanAxisEXAFSRegion *>(region));

	connect(deleteButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDeleteButtonClicked(QAbstractButton*)));

	setLayout(scanAxisViewLayout_);
}

void AMEXAFSScanAxisView::onLockRegionsToggled(bool toggled)
{
	if (toggled){

		lockRegionsButton_->setIcon(QIcon(":22x22/unlock.png"));
		lockRegionsButton_->setToolTip("Lock regions together.");
	}

	else {

		lockRegionsButton_->setIcon(QIcon(":/22x22/lock.png"));
		lockRegionsButton_->setToolTip("Unlock the regions from each other.");
	}
}

void AMEXAFSScanAxisView::onAddRegionButtonClicked()
{
	if (configuration_->scanAxisAt(0)->regionCount() == 0){

		AMScanAxisEXAFSRegion *region = new AMScanAxisEXAFSRegion(-1, -1, -1, -1, false, -1);
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


			if (temp->text() == "Beginning"){

				AMScanAxisEXAFSRegion *region = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(0)->createCopy());
				configuration_->scanAxisAt(0)->insertRegion(0, region);
				buildScanAxisRegionView(0, region);
			}

			else if (temp->text() == "End"){

				int indexOfEnd = configuration_->scanAxisAt(0)->regionCount();
				AMScanAxisEXAFSRegion *region = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(indexOfEnd-1)->createCopy());
				configuration_->scanAxisAt(0)->insertRegion(indexOfEnd, region);
				buildScanAxisRegionView(indexOfEnd, region);
			}

			else {

				int index = temp->text().split(" ").at(1).toInt();
				AMScanAxisEXAFSRegion *region = qobject_cast<AMScanAxisEXAFSRegion *>(configuration_->scanAxisAt(0)->regionAt(index)->createCopy());
				configuration_->scanAxisAt(0)->insertRegion(index, region);
				buildScanAxisRegionView(index, region);
			}
		}
	}
}

void AMEXAFSScanAxisView::onDeleteButtonClicked(QAbstractButton *button)
{
	AMEXAFSScanAxisElementView *view = regionMap_.value(button);
	configuration_->scanAxisAt(0)->removeRegion(view->region());
	deleteButtonGroup_->removeButton(button);
	regionMap_.remove(button);
	layout()->removeItem(layoutMap_.value(button));
	layoutMap_.take(button)->deleteLater();
	view->region()->deleteLater();
	view->deleteLater();
	button->deleteLater();
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
	index++;

	scanAxisViewLayout_->insertLayout(index, layout);
	deleteButtonGroup_->addButton(deleteButton);
	regionMap_.insert(deleteButton, elementView);
	layoutMap_.insert(deleteButton, layout);
}

