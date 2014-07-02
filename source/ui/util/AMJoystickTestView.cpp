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


#include "AMJoystickTestView.h"
#include "ui_AMJoystickTestView.h"

#include "util/AMJoystick.h"
#include <QPushButton>
#include <QToolButton>
#include <QProgressBar>

AMJoystickTestView::AMJoystickTestView(AMJoystick* joystick, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AMJoystickTestView)
{
    ui->setupUi(this);

    joystick_ = joystick;
    onJoystickConnected(joystick_->isConnected());

    connect(joystick_, SIGNAL(connected(bool)), this, SLOT(onJoystickConnected(bool)));
    connect(joystick_, SIGNAL(buttonChanged(int,bool,quint32)), this, SLOT(onButtonPressed(int,bool)));
    connect(joystick_, SIGNAL(axisChanged(int,int,quint32)), this, SLOT(onAxisMoved(int,int)));
}

AMJoystickTestView::~AMJoystickTestView()
{
    delete ui;
}

void AMJoystickTestView::onJoystickConnected(bool isConnected)
{
    // delete any existing button widgets and progress bar widgets.
    qDeleteAll(axes_);
    qDeleteAll(buttons_);
    qDeleteAll(labels_);
    axes_.clear();
    buttons_.clear();
    labels_.clear();


    if(isConnected) {
        // set id and description
        ui->idLabel->setText(joystick_->deviceIdentifier());
        ui->descriptionLabel->setText(joystick_->deviceDescription());
        ui->connectedButton->setEnabled(true);
        ui->connectedButton->setText("Connected");

        // create buttons to visualize joystick buttons
        for(int i=0,cc=joystick_->buttonCount(); i<cc; i++) {
            QToolButton* button = new QToolButton;
            button->setCheckable(true);
            buttons_.append(button);
            QLabel* label = new QLabel(QString("%1").arg(i+1));
            labels_.append(label);
            ui->buttonLayout->addWidget(label, i, 0);
            ui->buttonLayout->addWidget(button, i, 1);
        }

        // create progress bars to visualize axes
        for(int i=0,cc=joystick_->axisCount(); i<cc; i++) {
            QProgressBar* progressBar = new QProgressBar();
            progressBar->setRange(-32767, 32767);   // this is the range that joystick axes can take.
            progressBar->setFormat("%v");
            axes_.append(progressBar);
            QLabel* label = new QLabel(QString("%1").arg(i+1));
            labels_.append(label);
            ui->axisLayout->addWidget(label, i, 0);
            ui->axisLayout->addWidget(progressBar, i, 1);
        }
    }
    else {
        ui->idLabel->setText(joystick_->deviceIdentifier());
        ui->descriptionLabel->setText("");
        ui->connectedButton->setEnabled(false);
        ui->connectedButton->setText("Disconnected");
    }
}

void AMJoystickTestView::onButtonPressed(int id, bool value)
{
    buttons_.at(id)->setChecked(value);
}

void AMJoystickTestView::onAxisMoved(int id, int position)
{
    axes_.at(id)->setValue(position);
}
