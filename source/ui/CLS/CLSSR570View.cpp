#include "CLSSR570View.h"

#include <QHBoxLayout>
#include <QAction>
#include <QMenu>

CLSSR570View::CLSSR570View(CLSSR570 *sr570, QWidget *parent)
	: QWidget(parent)
{
	sr570_ = sr570;

	minus_ = new QToolButton;
	minus_->setMaximumSize(25, 25);
	minus_->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minus_, SIGNAL(clicked()), sr570_, SLOT(decreaseSensitivity()));
	connect(sr570_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)));
    if(sr570_->isConnected() && sr570_->atMinimumValue())
		minus_->setDisabled(true);

	plus_ = new QToolButton;
	plus_->setMaximumSize(25, 25);
	plus_->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plus_, SIGNAL(clicked()), sr570_, SLOT(increaseSensitivity()));
	connect(sr570_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)));
    if(sr570_->isConnected() && sr570_->atMaximumValue())
		plus_->setDisabled(true);

	value_ = new QComboBox;
	value_->hide();
	value_->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500");
	connect(value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)));
	connect(sr570_, SIGNAL(valueIndexChanged(int)), this, SLOT(onValueChanged(int)));
	if(sr570_->isConnected())
		onValueChanged(sr570_->valueIndex());

	units_ = new QComboBox;
	units_->hide();
	units_->addItems(QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V");
	connect(units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)));
	connect(sr570_, SIGNAL(unitsChanged(QString)), this, SLOT(onUnitsChanged(QString)));
	if(sr570_->isConnected())
		onUnitsChanged(sr570_->units());

	setViewMode(Basic);

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(minus_);
	layout->addWidget(plus_);
	layout->addWidget(value_);
	layout->addWidget(units_);

	setLayout(layout);
}

CLSSR570View::~CLSSR570View(){}

void CLSSR570View::onValueComboBoxChanged(int index)
{
//	sr570_->setValueIndex(index);
}

void CLSSR570View::onUnitsComboBoxChanged(int index)
{
//	sr570_->setUnits(units_->itemText(index));
}

void CLSSR570View::onValueChanged(int value)
{
	sr570_->blockSignals(true);
	value_->setCurrentIndex(value);
	sr570_->blockSignals(false);
}

void CLSSR570View::onUnitsChanged(QString units)
{
	sr570_->blockSignals(true);

	if (units == "pA/V")
		units_->setCurrentIndex(0);

	else if (units == "nA/V")
		units_->setCurrentIndex(1);

	else if (units == "uA/V")
		units_->setCurrentIndex(2);

	else if (units == "mA/V")
		units_->setCurrentIndex(3);

	sr570_->blockSignals(false);
}

void CLSSR570View::setViewMode(ViewMode mode)
{
	mode_ = mode;

	if (mode_ == Basic){

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

	emit viewModeChanged(mode_);
}

void CLSSR570View::onCustomContextMenuRequested(QPoint pos)
{
	QMenu popup(this);

	QAction *temp = popup.addAction("Basic View");
	temp->setDisabled(mode_ == Basic);

	temp = popup.addAction("Advanced View");
	temp->setDisabled(mode_ == Advanced);

	temp = popup.exec(mapToGlobal(pos));

	// If a valid action was selected.
	if (temp){

		if (temp->text() == "Basic View")
			setViewMode(Basic);

		else if (temp->text() == "Advanced View")
			setViewMode(Advanced);
	}
}
