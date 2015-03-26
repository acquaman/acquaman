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


#include "CLSDarkCurrentWidget.h"

CLSDarkCurrentWidget::CLSDarkCurrentWidget(double dwellSeconds, QWidget *parent) :
    QGroupBox("Dark current correction", parent)
{
	// Create UI elements.

	QLabel *timePrompt = new QLabel("Dwell time (s): ");
	timeEntry_ = new QDoubleSpinBox();
	collectButton_ = new QPushButton("Collect");

	// Create and set layouts.

	QHBoxLayout *layout = new QHBoxLayout();
	layout->addWidget(timePrompt);
	layout->addWidget(timeEntry_);
	layout->addWidget(collectButton_);

	setLayout(layout);

	// Initial settings.

	timeEntry_->setMinimum(DARK_CURRENT_DWELL_TIME_MIN);
	timeEntry_->setMaximum(DARK_CURRENT_DWELL_TIME_MAX);
	timeEntry_->setValue(dwellSeconds);

	collectButton_->setEnabled(true);

	// Make connections.

	connect( collectButton_, SIGNAL(clicked()), this, SLOT(onCollectButtonClicked()) );
}

CLSDarkCurrentWidget::~CLSDarkCurrentWidget()
{
}

void CLSDarkCurrentWidget::setCollectButtonEnabled(bool isEnabled)
{
    collectButton_->setEnabled(isEnabled);
}

void CLSDarkCurrentWidget::onCollectButtonClicked()
{
    double timeEntered = timeEntry_->text().toDouble();

    if (timeEntered > 0) {
	    emit collectButtonClicked(timeEntered);
    }
}
