/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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

#include <QStackedWidget>
#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

VESPERSDeviceStatusView::VESPERSDeviceStatusView(QWidget *parent) :
	QWidget(parent)
{
	VESPERSDiagnosticsView *temperature = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->temperatureSet(), 1, true);
	VESPERSDiagnosticsView *pressure = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->pressureSet(), 1, true);
	VESPERSDiagnosticsView *valves = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->valveSet(), 1, false);
	VESPERSDiagnosticsView *ionPumps = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->ionPumpSet(), 2, false);
	VESPERSDiagnosticsView *flowSwitch = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->flowSwitchSet(), 2, false);
	VESPERSDiagnosticsView *flowTransducer = new VESPERSDiagnosticsView(VESPERSBeamline::vespers()->flowTransducerSet(), 2, true);

	QStackedWidget *stack = new QStackedWidget;
	stack->addWidget(temperature);
	stack->addWidget(pressure);
	stack->addWidget(valves);
	stack->addWidget(ionPumps);
	stack->addWidget(flowSwitch);
	stack->addWidget(flowTransducer);

	QButtonGroup *device = new QButtonGroup;
	QHBoxLayout *buttons = new QHBoxLayout;
	buttons->setSpacing(0);
	buttons->addStretch();

	QToolButton *temp = new QToolButton;
	temp->setCheckable(true);
	temp->setChecked(true);
	temp->setText("Temperature");
	device->addButton(temp, 0);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Pressure");
	device->addButton(temp, 1);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Valves");
	device->addButton(temp, 2);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Ion Pumps");
	device->addButton(temp, 3);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Flow Switches");
	device->addButton(temp, 4);
	buttons->addWidget(temp);

	temp = new QToolButton;
	temp->setCheckable(true);
	temp->setText("Flow Transducers");
	device->addButton(temp, 5);
	buttons->addWidget(temp);

	buttons->addStretch();

	connect(device, SIGNAL(buttonClicked(int)), stack, SLOT(setCurrentIndex(int)));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addStretch();
	layout->addLayout(buttons);
	layout->addWidget(stack);
	layout->addStretch();

	setLayout(layout);
}
