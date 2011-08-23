#include "CLSSynchronizedDwellTimeView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>

CLSSynchronizedDwellTimeElementView::CLSSynchronizedDwellTimeElementView(CLSSynchronizedDwellTimeElement *dwellTimeElement, QWidget *parent)
	: QWidget(parent)
{
	element_ = dwellTimeElement;

	status_ = new QLabel;
	status_->setPixmap(QIcon(":/OFF.png").pixmap(25, 25));
	connect(element_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	QLabel *name = new QLabel(element_->name());
	connect(element_, SIGNAL(nameChanged(QString)), name, SLOT(setText(QString)));

	enable_ = new QPushButton;
	enable_->setCheckable(true);
	enable_->setChecked(false);
	enable_->setPalette(QPalette(Qt::red));
	connect(element_, SIGNAL(enabledChanged(bool)), enable_, SLOT(setChecked(bool)));
	connect(element_, SIGNAL(enabledChanged(bool)), this, SLOT(onEnabledChanged(bool)));
	connect(enable_, SIGNAL(toggled(bool)), this, SLOT(setEnabledState(bool)));

	time_ = new QDoubleSpinBox;
	time_->setDecimals(2);
	time_->setValue(0);
	time_->setMinimum(0);
	time_->setMaximum(1e6);
	time_->setAlignment(Qt::AlignCenter);
	time_->setSuffix(" " + element_->units());
	connect(element_, SIGNAL(timeChanged(double)), time_, SLOT(setValue(double)));
	connect(element_, SIGNAL(timeChanged(double)), this, SLOT(checkTimeUnits()));
	connect(time_, SIGNAL(editingFinished()), this, SLOT(setTime()));

	QHBoxLayout *elementLayout = new QHBoxLayout;
	elementLayout->addWidget(status_);
	elementLayout->addWidget(name);
	elementLayout->addWidget(enable_);
	elementLayout->addWidget(time_);

	setLayout(elementLayout);
}

void CLSSynchronizedDwellTimeElementView::checkTimeUnits()
{
	if (element_->units() != "?"){

		time_->setSuffix(" " + element_->units());
		disconnect(element_, SIGNAL(timeChanged(double)), this, SLOT(checkTimeUnits()));
	}
}

void CLSSynchronizedDwellTimeElementView::setTime()
{
	double newTime = time_->value();

	if (element_->units() == "ms")
		newTime /= 1000;

	if (newTime != element_->time())
		element_->setTime(newTime);
}

CLSSynchronizedDwellTimeView::CLSSynchronizedDwellTimeView(CLSSynchronizedDwellTime *dwellTime, QWidget *parent)
	: QWidget(parent)
{
	dwellTime_ = dwellTime;

	overallStatus_ = new QLabel;
	overallStatus_->setPixmap(QIcon(":/OFF.png").pixmap(30, 30));
	connect(dwellTime_, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	mode_ = new QPushButton;
	mode_->setCheckable(true);
	mode_->setChecked(false);
	connect(dwellTime_, SIGNAL(modeChanged(CLSSynchronizedDwellTime::Mode)), this, SLOT(onModeChanged(CLSSynchronizedDwellTime::Mode)));
	connect(mode_, SIGNAL(toggled(bool)), this, SLOT(setMode(bool)));

	masterTime_ = new QDoubleSpinBox;
	masterTime_->setDecimals(2);
	masterTime_->setValue(0);
	masterTime_->setMinimum(0);
	masterTime_->setMaximum(1000);
	masterTime_->setSuffix(" s");
	masterTime_->setAlignment(Qt::AlignCenter);
	connect(dwellTime_, SIGNAL(timeChanged(double)), masterTime_, SLOT(setValue(double)));
	connect(masterTime_, SIGNAL(editingFinished()), this, SLOT(setTime()));

	QHBoxLayout *topRow = new QHBoxLayout;
	topRow->addWidget(overallStatus_, 0, Qt::AlignCenter);
	topRow->addWidget(mode_);
	topRow->addWidget(masterTime_);

	QVBoxLayout *groupBoxLayout = new QVBoxLayout;
	groupBoxLayout->addLayout(topRow);

	for (int i = 0; i < dwellTime_->elementCount(); i++)
		groupBoxLayout->addWidget(new CLSSynchronizedDwellTimeElementView(dwellTime_->elementAt(i)));

	QGroupBox *dwellTimeBox = new QGroupBox("Synchronized Dwell Time");
	dwellTimeBox->setLayout(groupBoxLayout);

	QVBoxLayout *dwellTimeLayout = new QVBoxLayout;
	dwellTimeLayout->addWidget(dwellTimeBox);

	setMaximumSize(420, 80 + 50*dwellTime_->elementCount());
	setLayout(dwellTimeLayout);
}

void CLSSynchronizedDwellTimeView::setTime()
{
	dwellTime_->blockSignals(true);
	dwellTime_->setTime(masterTime_->value());
	dwellTime_->blockSignals(false);
}

void CLSSynchronizedDwellTimeView::setMode(bool mode)
{
	if (mode && dwellTime_->mode() != CLSSynchronizedDwellTime::Continuous)
		dwellTime_->setMode(CLSSynchronizedDwellTime::Continuous);
	else if (!mode && dwellTime_->mode() != CLSSynchronizedDwellTime::SingleShot)
		dwellTime_->setMode(CLSSynchronizedDwellTime::SingleShot);
}
