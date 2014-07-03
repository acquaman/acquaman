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


#include "VESPERSPIDLoopControlView.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

VESPERSPIDLoopControlView::VESPERSPIDLoopControlView(VESPERSPIDLoopControl *pid, QWidget *parent)
	: QWidget(parent)
{
	pid_ = pid;
	connect(pid_, SIGNAL(stateChanged(bool)), this, SLOT(setDisabled(bool)));
	connect(pid_, SIGNAL(stateChanged(bool)), this, SLOT(onHiddenChanged(bool)));

	QFont font(this->font());
	font.setBold(true);

	QLabel *description = new QLabel(pid_->name());
	description->setFont(font);

	fix_ = new QPushButton("Repair");
	connect(fix_, SIGNAL(clicked()), pid_, SLOT(repair()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(description, 0, Qt::AlignLeft);
	layout->addWidget(fix_, 0, Qt::AlignCenter);

	setLayout(layout);

	highlight_ = false;

	timer_ = new QTimer;
	timer_->setInterval(1000);
	timer_->setSingleShot(false);
	connect(timer_, SIGNAL(timeout()), this, SLOT(toggleButtonColor()));
}

VESPERSPIDLoopControlView::~VESPERSPIDLoopControlView()
{
	delete timer_;
}

void VESPERSPIDLoopControlView::toggleButtonColor()
{
	highlight_ = !highlight_;

	if (highlight_){

		QPalette palette(fix_->palette());
		palette.setColor(QPalette::Button, QColor(255, 64, 64));
		fix_->setPalette(palette);
	}
	else
		fix_->setPalette(this->palette());
}

void VESPERSPIDLoopControlView::onHiddenChanged(bool hidden)
{
	setHidden(hidden);

	if (hidden)
		timer_->stop();
	else
		timer_->start();
}
