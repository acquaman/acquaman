#include "CLSSR570CompositeView.h"

#include <QHBoxLayout>
#include <QAction>
#include <QMenu>

CLSSR570CompositeView::~CLSSR570CompositeView(){}

CLSSR570CompositeView::CLSSR570CompositeView(CLSSR570 *sr1, CLSSR570 *sr2, QWidget *parent)
	: QWidget(parent)
{
	firstSR570_ = sr1;
	secondSR570_ = sr2;

	minus_ = new QToolButton;
	minus_->setFixedSize(25, 25);
	minus_->setIcon(QIcon(":/22x22/list-remove.png"));
	connect(minus_, SIGNAL(clicked()), firstSR570_, SLOT(decreaseSensitivity()));
	connect(minus_, SIGNAL(clicked()), secondSR570_, SLOT(decreaseSensitivity()));
	connect(firstSR570_, SIGNAL(minimumSensitivity(bool)), minus_, SLOT(setDisabled(bool)));

	plus_ = new QToolButton;
	plus_->setFixedSize(25, 25);
	plus_->setIcon(QIcon(":/22x22/list-add.png"));
	connect(plus_, SIGNAL(clicked()), firstSR570_, SLOT(increaseSensitivity()));
	connect(plus_, SIGNAL(clicked()), secondSR570_, SLOT(increaseSensitivity()));
	connect(firstSR570_, SIGNAL(maximumSensitivity(bool)), plus_, SLOT(setDisabled(bool)));

	value_ = new QComboBox;
	value_->hide();
	value_->setFixedHeight(25);
	value_->addItems(QStringList() << "1" << "2" << "5" << "10" << "20" << "50" << "100" << "200" << "500");
	connect(value_, SIGNAL(currentIndexChanged(int)), this, SLOT(onValueComboBoxChanged(int)));
	connect(firstSR570_, SIGNAL(valueIndexChanged(int)), this, SLOT(onValueChanged(int)));

	units_ = new QComboBox;
	units_->hide();
	units_->setFixedHeight(25);
	units_->addItems(QStringList() << "pA/V" << "nA/V" << "uA/V" << "mA/V");
	connect(units_, SIGNAL(currentIndexChanged(int)), this, SLOT(onUnitsComboBoxChanged(int)));
	connect(firstSR570_, SIGNAL(unitsChanged(QString)), this, SLOT(onUnitsChanged(QString)));

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

void CLSSR570CompositeView::onValueComboBoxChanged(int index)
{
//	firstSR570_->setValueIndex(index);
//	secondSR570_->setValueIndex(index);
}

void CLSSR570CompositeView::onUnitsComboBoxChanged(int index)
{
//	firstSR570_->setUnits(units_->itemText(index));
//	secondSR570_->setUnits(units_->itemText(index));
}

void CLSSR570CompositeView::onValueChanged(int value)
{
	firstSR570_->blockSignals(true);
	secondSR570_->blockSignals(true);
	value_->setCurrentIndex(value);
	firstSR570_->blockSignals(false);
	secondSR570_->blockSignals(false);
}

void CLSSR570CompositeView::onUnitsChanged(QString units)
{
	firstSR570_->blockSignals(true);
	secondSR570_->blockSignals(true);

	if (units == "pA/V")
		units_->setCurrentIndex(0);

	else if (units == "nA/V")
		units_->setCurrentIndex(1);

	else if (units == "uA/V")
		units_->setCurrentIndex(2);

	else if (units == "mA/V")
		units_->setCurrentIndex(3);

	firstSR570_->blockSignals(false);
	secondSR570_->blockSignals(false);
}

void CLSSR570CompositeView::setViewMode(ViewMode mode)
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

void CLSSR570CompositeView::onCustomContextMenuRequested(QPoint pos)
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
