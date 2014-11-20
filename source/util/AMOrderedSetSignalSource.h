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


#ifndef AMORDEREDSETSIGNALSOURCE_H
#define AMORDEREDSETSIGNALSOURCE_H

#include <QObject>
#include <QSet>
#include <QEvent>



/// Template classes cannot have signals or slots, so this class provides a proxy to emit signals from any AMOrderedList<value> or AMOrderedSet<key,value> class.  Simply connect to the AMOrderedList::signalSource() or AMOrderedSet::signalSource() object.  You should never need to create an instance of this object directly.

class AMOrderedSetSignalSource : public QObject {
	Q_OBJECT

public:
 	virtual ~AMOrderedSetSignalSource();
	AMOrderedSetSignalSource();

	/// Do not call this function, it should be considered only available to AMOrderedSet.
	void emitItemAboutToBeAdded(int atIndex) { emit itemAboutToBeAdded(atIndex); }
	/// Do not call this function, it should be considered only available to AMOrderedSet.
	void emitItemAdded(int atIndex) { emit itemAdded(atIndex); }
	/// Do not call this function, it should be considered only available to AMOrderedSet.
	void emitItemAboutToBeRemoved(int atIndex) { emit itemAboutToBeRemoved(atIndex); }
	/// Do not call this function, it should be considered only available to AMOrderedSet.
	void emitItemRemoved(int atIndex) { emit itemRemoved(atIndex); }
	/// Do not call this function, it should be considered only available to AMOrderedSet. This immediately emits the itemChanged() signal, after an AMOrderedSet::replace()
	void emitItemChanged(int index) { emit itemChanged(index); }

	/// Do not call this function, it should be considered only available to AMOrderedSet. It schedules the itemChanged() signal to be emitted when control returns to the event loop. (Useful to call after the non-const AMOrderedSet::operator[] is used to access and possibly modify an item.)
	void scheduleItemChanged(int index);

	/// Returns true if a delayed itemChanged() signal is pending to be issued.
	bool delayedItemChangedScheduled() const { return delayedItemChangedScheduled_; }

signals:
	/// Emitted just before a new item is added, the new item will end up at \c index, but it's not there yet.
	void itemAboutToBeAdded(int index);
	/// Emitted after a new item is added at \c index
	void itemAdded(int index);
	/// Emitted before an item is removed from the position at \c index
	void itemAboutToBeRemoved(int index);
	/// Emitted after an item is removed from the position at \c index. It's no longer there.
	void itemRemoved(int index);
	/// Emitted after the item at \c index is replaced, or (possibly) changed in some way. This signal is emitted immediately after AMOrderedSet::replace() finishes. It's also emitted once when control goes back to the event loop after the non-const version of AMOrderedSet::operator[]() is used one or more times to access (and possibly modify) an item.  This merges multiple non-const accesses into one itemChanged() signal, but has the drawback that this signal will not be emitted when using AMOrderedSet outside of an event loop.
	void itemChanged(int index);

protected:
	/// true if a delayed itemChanged() notification is scheduled.
	bool delayedItemChangedScheduled_;
	/// Set of item indexes: includes all the items that have changed during this call
	QSet<int> itemsChanged_;

	/// Receive delayed AMOrderedSetSignalSourceItemChangedEvent events
	bool event(QEvent* event);
	void onDelayedItemChanged();
};

#endif // AMORDEREDSETSIGNALSOURCE_H
