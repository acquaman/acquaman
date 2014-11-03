#include "AMEnergyListView.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

// AMEnergyListElementView
/////////////////////////////////////////////

AMEnergyListElementView::AMEnergyListElementView(double energy, QWidget *parent)
	: QWidget(parent)
{
	energy_ = energy;

	energySpinBox_ = new QDoubleSpinBox;
	energySpinBox_->setRange(0, 100000);
	energySpinBox_->setSuffix(" eV");
	energySpinBox_->setDecimals(2);
	energySpinBox_->setValue(energy_);
	energySpinBox_->setAlignment(Qt::AlignCenter);
	connect(energySpinBox_, SIGNAL(editingFinished()), this, SLOT(onEnergyUpdated()));

	QHBoxLayout *elementViewLayout = new QHBoxLayout;
	elementViewLayout->addWidget(new QLabel("Energy"));
	elementViewLayout->addWidget(energySpinBox_);

	setLayout(elementViewLayout);
}

void AMEnergyListElementView::setEnergySpinBox(double energy)
{
	if (energy != energySpinBox_->value()){

		energySpinBox_->setValue(energy);
		onEnergyUpdated();
	}
}

void AMEnergyListElementView::onEnergyUpdated()
{
	energy_ = energySpinBox_->value();
	emit energyChanged(energy_);
}

void AMEnergyListElementView::setEnergyRange(const AMRange &newRange)
{
	if (newRange.isValid())
		energySpinBox_->setRange(newRange.minimum(), newRange.maximum());
}

// AMEnergyListView
/////////////////////////////////////////////

AMEnergyListView::AMEnergyListView(const QString &title, const AMEnergyList &list, QWidget *parent)
	: QWidget(parent)
{
	energyRange_ = AMRange();

	foreach (double energy, list.energies())
		energyList_.insertEnergy(energyList_.energies().size()-1, energy);

	deleteButtonGroup_ = new QButtonGroup;
	energyViewLayout_ = new QVBoxLayout;

	addRegionButton_ = new QToolButton;
	addRegionButton_->setIcon(QIcon(":22x22/list-add.png"));
	connect(addRegionButton_, SIGNAL(clicked()), this, SLOT(onAddEnergyButtonClicked()));

	QHBoxLayout *topRowLayout = new QHBoxLayout;
	topRowLayout->addWidget(new QLabel(title));
	topRowLayout->addStretch();
	topRowLayout->addWidget(addRegionButton_);
	energyViewLayout_->addLayout(topRowLayout);

	foreach (double energy, energyList_.energies())
		buildEnergyElementView(energyList_.energyCount(), energy);

	connect(deleteButtonGroup_, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(onDeleteButtonClicked(QAbstractButton*)));

	setLayout(energyViewLayout_);
}

void AMEnergyListView::onAddEnergyButtonClicked()
{
	if (energyList_.energyCount() == 0){

		energyList_.insertEnergy(0, 10);
		buildEnergyElementView(0, 10);
	}

	else {

		QMenu menu(addRegionButton_);
		QAction *temp = menu.addAction("Add region to...");
		temp->setDisabled(true);

		menu.addAction("Beginning");

		for (int i = 1, size = energyList_.energyCount(); i < size; i++)
			menu.addAction(QString("Between %1 and %2").arg(i).arg(i+1));

		menu.addAction("End");

		temp = menu.exec(mapToGlobal(addRegionButton_->pos()));

		if (temp){

			if (temp->text() == "Beginning"){


				energyList_.insertEnergy(0, 0);
				buildEnergyElementView(0, 0);
			}

			else if (temp->text() == "End"){

				int indexOfEnd = energyList_.energyCount();
				energyList_.insertEnergy(indexOfEnd, 0);
				buildEnergyElementView(indexOfEnd, 0);
			}

			else {

				int index = temp->text().split(" ").at(1).toInt();
				energyList_.insertEnergy(index, 0);
				buildEnergyElementView(index, 0);
			}
		}
	}
}

void AMEnergyListView::onDeleteButtonClicked(QAbstractButton *button)
{
	AMEnergyListElementView *view = energyMap_.value(button);
	energyList_.removeEnergy(energyElementViewList_.indexOf(view));
	deleteButtonGroup_->removeButton(button);
	energyMap_.remove(button);
	energyElementViewList_.removeOne(view);
	layout()->removeItem(layoutMap_.value(button));
	layoutMap_.take(button)->deleteLater();
	view->deleteLater();
	button->deleteLater();
}

void AMEnergyListView::insertEnergy(int index, double energy)
{
	energyList_.insertEnergy(index, energy);
	buildEnergyElementView(index, energy);
}

void AMEnergyListView::removeEnergy(int index)
{
	onDeleteButtonClicked(energyMap_.key(energyElementViewList_.at(index)));
}

void AMEnergyListView::buildEnergyElementView(int index, double energy)
{
	AMEnergyListElementView *elementView = new AMEnergyListElementView(energy);
	elementView->setEnergyRange(energyRange_);
	connect(elementView, SIGNAL(energyChanged(double)), this, SLOT(onElementEnergyChanged()));

	QToolButton *deleteButton = new QToolButton;
	deleteButton->setIcon(QIcon(":22x22/list-remove-2.png"));
	deleteButton->setToolTip("Delete region");
	deleteButton->setAutoRaise(true);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(elementView);
	layout->addWidget(deleteButton);

	// Need to account for the title row.
	energyViewLayout_->insertLayout(index+1, layout);
	deleteButtonGroup_->addButton(deleteButton);
	energyMap_.insert(deleteButton, elementView);
	layoutMap_.insert(deleteButton, layout);
	energyElementViewList_.insert(index, elementView);
}

void AMEnergyListView::onElementEnergyChanged()
{
	for (int i = 0, size = energyElementViewList_.size(); i < size; i++)
		energyList_.setEnergy(i, energyElementViewList_.at(i)->energy());
}

void AMEnergyListView::setRange(double minimum, double maximum)
{
	energyRange_ = AMRange(minimum, maximum);

	foreach (AMEnergyListElementView *view, energyElementViewList_)
		view->setEnergyRange(energyRange_);
}
