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


#include "VESPERSDeviceStatusView.h"
#include "ui/VESPERS/VESPERSDiagnosticsView.h"
#include "beamline/VESPERS/VESPERSBeamline.h"
#include "ui/AMTopFrame.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>

VESPERSDeviceStatusView::VESPERSDeviceStatusView(QWidget *parent) :
	QWidget(parent)
{
	AMTopFrame *top = new AMTopFrame("Device Status Page");
	top->setIcon(QIcon(":/system-software-update.png"));

	VESPERSDiagnosticsView *temperature = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->temperatureSet(), 1, true);
	VESPERSDiagnosticsView *pressure = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->pressureSet(), 1, true);
	VESPERSDiagnosticsView *valves = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->valveSet(), 2, false);
	VESPERSDiagnosticsView *ionPumps = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->ionPumpSet(), 2, false);
	VESPERSDiagnosticsView *flowSwitch = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->flowSwitchSet(), 2, false);
	VESPERSDiagnosticsView *flowTransducer = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->flowTransducerSet(), 2, true);

	connect(VESPERSBeamline::vespers(), SIGNAL(pressureStatus(bool)), this, SLOT(onPressureStatusChanged(bool)));
	connect(VESPERSBeamline::vespers(), SIGNAL(valveStatus(bool)), this, SLOT(onValveStatusChanged(bool)));
	connect(VESPERSBeamline::vespers(), SIGNAL(ionPumpStatus(bool)), this, SLOT(onIonPumpStatusChanged(bool)));
	connect(VESPERSBeamline::vespers(), SIGNAL(temperatureStatus(bool)), this, SLOT(onTemperatureStatusChanged(bool)));
	connect(VESPERSBeamline::vespers(), SIGNAL(flowSwitchStatus(bool)), this, SLOT(onFlowSwitchStatusChanged(bool)));
	connect(VESPERSBeamline::vespers(), SIGNAL(flowTransducerStatus(bool)), this, SLOT(onFlowTransducerStatusChanged(bool)));

	QStackedWidget *diagnosticViews = new QStackedWidget;
	diagnosticViews->addWidget(temperature);
	diagnosticViews->addWidget(pressure);
	diagnosticViews->addWidget(valves);
	diagnosticViews->addWidget(ionPumps);
	diagnosticViews->addWidget(flowSwitch);
	diagnosticViews->addWidget(flowTransducer);

	deviceButtonGroup_ = new QButtonGroup;
	QHBoxLayout *buttons = new QHBoxLayout;
	buttons->setSpacing(0);
	buttons->addStretch();

	QToolButton *temp = new QToolButton;
	temp->setCheckable(true);
	temp->setChecked(true);
	temp->setText("Temperature");
	temp->setPalette(QPalette(Qt::red));
	deviceButtonGroup_->addButton(temp, 0);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Pressure");
	temp->setPalette(QPalette(Qt::red));
	deviceButtonGroup_->addButton(temp, 1);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Valves");
	temp->setPalette(QPalette(Qt::red));
	deviceButtonGroup_->addButton(temp, 2);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Ion Pumps");
	temp->setPalette(QPalette(Qt::red));
	deviceButtonGroup_->addButton(temp, 3);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Flow Switches");
	temp->setPalette(QPalette(Qt::red));
	deviceButtonGroup_->addButton(temp, 4);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Flow Transducers");
	temp->setPalette(QPalette(Qt::red));
	deviceButtonGroup_->addButton(temp, 5);
	buttons->addWidget(temp);

	buttons->addStretch();

	connect(deviceButtonGroup_, SIGNAL(buttonClicked(int)), diagnosticViews, SLOT(setCurrentIndex(int)));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(top);
	layout->addStretch();
	layout->addLayout(buttons);
	layout->addWidget(diagnosticViews);
	layout->addStretch();

	setLayout(layout);
}

VESPERSDeviceStatusView::~VESPERSDeviceStatusView(){}

void VESPERSDeviceStatusView::showDiagnosticsPage(const QString &name)
{
	if (name == "Valves")
		deviceButtonGroup_->button(2)->click();

	else if (name == "Pressure")
		deviceButtonGroup_->button(1)->click();

	else if (name == "Temperature")
		deviceButtonGroup_->button(0)->click();

	else if (name == "Ion Pumps")
		deviceButtonGroup_->button(3)->click();

	else if (name == "Water Switches")
		deviceButtonGroup_->button(4)->click();

	else if (name == "Water Pressure")
		deviceButtonGroup_->button(5)->click();
}
