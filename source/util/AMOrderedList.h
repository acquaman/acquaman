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


#ifndef AMORDEREDLIST_H
#define AMORDEREDLIST_H

#include <QList>
#include "util/AMOrderedSetSignalSource.h"

/// This container class extends the functionality of QList by adding signalling for item insert, remove, and modification.  It's exactly like AMOrderedSet but without key-based indexing.

/*!
<b>Performance</b>

Access by index, appending, removing and inserting are all constant-time (O(1)) operations.

<b>Insert and Remove signalling</b>

As a template class, an AMOrderedSet instance cannot emit signals itself.  However, you can connect to the AMOrderedSet::signalSource() object, which will emit itemAboutToBeAdded(), itemAdded() and itemAboutToBeRemoved(), itemRemoved() signals before and after each new item is added, and before and after each item is removed, respectively.
*/

template <class Tvalue>
class AMOrderedList
{
public:
	/// Default constructor.
	AMOrderedList() {
		signalSource_ = new AMOrderedSetSignalSource();
	}

	/// Copy constructor.
	AMOrderedList(const AMOrderedList& other) {
		signalSource_ = new AMOrderedSetSignalSource();

		values_ = other.values_;
	}

	/// Assignment operator. The itemAboutToBeRemoved() and itemRemoved() signals are emitted for all existing items, and the itemAboutToBeAdded() and itemAdded() signals are emitted for all the new items that we copy in from \c other.
	AMOrderedList& operator=(const AMOrderedList& other) {
		// check for self-assignment
		if(this != &other) {
			// we use clear() and append() so that added/removed signals are emitted properly.
			clear();

			for(int i=0; i<other.count(); i++)
				append(other.at(i));
		}

		return *this;
	}

	/// Destructor
	virtual ~AMOrderedList() { delete signalSource_; }

	// const access functions:
	//=====================================

	/// Number of items in the set:
	int count() const { return values_.count(); }
	/// Returns true if the container is empty.
	bool isEmpty() const { return values_.isEmpty(); }
	/// Returns a const reference to the item at index \c i.  (\c i must be between [0, count()) )
	const Tvalue& at(int i) const { return values_.at(i); }
	/// Returns a const reference to the item at index \c i. (\c i must be between [0, count()) )
	const Tvalue& operator[](int i) const { return values_.at(i); }


	/// Access the AMOrderedSetSignalSource which provides signals for this container.
	const AMOrderedSetSignalSource* signalSource() const { return signalSource_; }

	/// Returns a QList containing all the items in the set, in order. This is fast, thanks to implicit sharing.
	QList<Tvalue> toList() const { return values_; }


	// non-const access functions
	//=====================================

	/// Returns a modifiable reference to the item at index \c i.  (\c i must be between [0, count()) ).  The itemChanged() signal will be emitted after control returns to the event loop (and thus after your modification).
	Tvalue& operator[](int i) { signalSource_->scheduleItemChanged(i); return values_[i]; }


	/// Append a new item to the end of the set.
	void append(const Tvalue& item) {
		insert(count(), item);
	}

	/// Insert a new item at \c index into the set. If \c index = -1 or count(), the item as appended to the end of the set.
	void insert(int index, const Tvalue& item) {
		if(index<0 || index>count())
			index = count();

		signalSource_->emitItemAboutToBeAdded(index);
		values_.insert(index, item);

		signalSource_->emitItemAdded(index);
	}

	/// Replace the item at index \c i with a \c newItem.  The old item is returned.  (\c i must be between [0, count()) )
	Tvalue replace(int i, const Tvalue& newItem) {
		Tvalue oldValue = values_.at(i);
		values_[i] = newItem;

		signalSource_->emitItemChanged(i);
		return oldValue;
	}

	/// Remove the item at \c index.  The AMOrderedSetSignalSource::itemAboutToBeRemoved() signal will be emitted first.  Does nothing if \c index < 0 or \c index >= count().
	void remove(int index) {
		if(index<0 || index>=count())
			return;

		signalSource_->emitItemAboutToBeRemoved(index);

		values_.removeAt(index);

		signalSource_->emitItemRemoved(index);
	}

	/// Remove and return the item at \c index.  The AMOrderedSetSignalSource::itemAboutToBeRemoved() signal will be emitted first.  (\c index must be >= 0 and < count(). )
	Tvalue takeAt(int index) {
		Tvalue returnMe = at(index);
		remove(index);
		return returnMe;
	}

	/// Clears the set, emitting aboutToBeRemoved() and removed() signals for each item.  \todo If nothing is connected to the signalSource(), there's a much faster way of doing this.
	void clear() {
		while(count())
			remove(count()-1);
	}



protected:
	QList<Tvalue> values_;
	AMOrderedSetSignalSource* signalSource_;
};

#endif // AMORDEREDLIST_H
