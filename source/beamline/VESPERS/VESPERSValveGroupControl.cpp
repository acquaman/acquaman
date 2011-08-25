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


#include "VESPERSValveGroupControl.h"

#include <QTimer>

VESPERSValveGroupControl::VESPERSValveGroupControl(QList<AMValveControl *> *list, QObject *parent)
	:  QObject(parent)
{
	list_ = list;
	index_ = -1;

	for (int i = 0; i < list_->size(); i++)
		connect(list_->at(i), SIGNAL(stateChanged(int)), this, SLOT(onValveStateChanged()));
}

bool VESPERSValveGroupControl::allValvesOpen()
{
	for (int i = 0; i < list_->size(); i++)
		if (list_->at(i)->state() == 0 || list_->at(i)->state() == 1)
			return false;

	return true;
}

int VESPERSValveGroupControl::indexOf(QString name)
{
	for (int i = 0; i < list_->size(); i++)
		if (list_->at(i)->name().compare(name) == 0)
			return i;

	return -1;
}

void VESPERSValveGroupControl::openAllValves()
{
	index_ = list_->size()-1;
	openAllValvesHelper();
}

void VESPERSValveGroupControl::closeAllValves()
{
	index_ = 0;
	closeAllValvesHelper();
}

void VESPERSValveGroupControl::openAllValvesHelper()
{
	openValve(index_--);

	if (index_ >= 0)
		QTimer::singleShot(150, this, SLOT(openAllValvesHelper()));
}

void VESPERSValveGroupControl::closeAllValvesHelper()
{
	closeValve(index_++);

	if (index_ < list_->size())
		QTimer::singleShot(150, this, SLOT(closeAllValvesHelper()));
}
