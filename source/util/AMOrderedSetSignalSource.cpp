/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMOrderedSetSignalSource.h"
#include <QCoreApplication>
#include "acquaman.h"

 AMOrderedSetSignalSource::~AMOrderedSetSignalSource(){}
AMOrderedSetSignalSource::AMOrderedSetSignalSource() :
	QObject()
{
	delayedItemChangedScheduled_ = false;
}

void AMOrderedSetSignalSource::scheduleItemChanged(int index) {
	itemsChanged_ << index;

	if(!delayedItemChangedScheduled_) {
		delayedItemChangedScheduled_ = true;
		QCoreApplication::postEvent(this, new QEvent((QEvent::Type)AM::ItemChangedEvent), Qt::HighEventPriority);
	}
}


void AMOrderedSetSignalSource::onDelayedItemChanged() {
	if(!delayedItemChangedScheduled_)
		return;

	delayedItemChangedScheduled_ = false;

	QSetIterator<int> itemsChangedIterator(itemsChanged_);
	while(itemsChangedIterator.hasNext())
		emit itemChanged(itemsChangedIterator.next());

	itemsChanged_.clear();
}


bool AMOrderedSetSignalSource::event(QEvent *event) {
	if(event->type() == (QEvent::Type)AM::ItemChangedEvent) {
		onDelayedItemChanged();
		event->accept();
		return true;
	}
	else
		return QObject::event(event);
}
