#ifndef AMORDEREDSET_H
#define AMORDEREDSET_H

#include <QList>
#include <QHash>

#include "dataman/AMOrderedSetSignalSource.h"

/// This container class offers hybrid functionality of QList and QHash.  Items can be inserted, appended, removed, and replaced in an ordered list, by numerical index.  Iterating through the items by index from 0 to count() is fast and convenient. Additionally, items can also be accessed by key, where the key is any type compatible as a key for QHash.
/*!
<b>Restrictions</b>

It is possible to control whether duplicate entries with the same key are allowed, using setAllowsDuplicateKeys().  When multiple entries with the same key exist, indexOf() returns the first matching item in the list (by index).

<b>Performance</b>

Access by index or key, and appending or removing from the end of the list, are all constant-time (O(1)) operations.

Inserting or removing in the middle of the list can be a linear-time operation. This may be quite slow, since all items after the insertion/removal point need to be re-hashed.

<b>Insert and Remove signalling</b>

As a template class, an AMOrderedSet instance cannot emit signals itself.  However, you can connect to the AMOrderedSet::signalSource() object, which will emit itemAdded() and itemAboutToBeRemoved() signals after each new item is added, and before each item is removed, respectively.
*/

template <class Tkey, class Tvalue>
class AMOrderedSet
{

public:
	explicit AMOrderedSet(bool allowDuplicateKeys = true) {
		allowsDuplicateKeys_ = allowDuplicateKeys;
		signalSource_ = new AMOrderedSetSignalSource();
	}

	~AMOrderedSet() { delete signalSource_; }

	// const access functions:
	//=====================================

	/// Number of items in the set:
	int count() const { return values_.count(); }
	/// Returns a const reference to the item at index \c i.  (\c i must be between [0, count()) )
	const Tvalue& at(int i) const { return values_.at(i); }
	/// Returns a const reference to the item at index \c i. (\c i must be between [0, count()) )
	const Tvalue& operator[](int i) const { return values_.at(i); }


	/// Returns the index of the first item with key \c key, or -1 if the key isn't found. The item can then be returned using at() or operator[].
	int indexOf(const Tkey& key) const {
		typename QHash<Tkey,int>::const_iterator ih = hash_.find(key);
		int lowestIndexFound = count();
		// go through all entries found with that key (there may be multiple, if duplicates allowed!) and find the lowest index -- aka, the first.
		while(ih != hash_.constEnd() && ih.key() == key) {
			if(ih.value() < lowestIndexFound)
				lowestIndexFound = ih.value();
			ih++;
		}
		if(lowestIndexFound < count())
			return lowestIndexFound;
		else
			return -1; // no key found

	}

	const AMOrderedSetSignalSource* signalSource() const { return signalSource_; }

	/// Indicates whether insert() and append() will accept keys that already exist
	bool allowsDuplicateKeys() const { return allowsDuplicateKeys_; }


	// non-const access functions
	//=====================================

	/// Returns a modifiable reference to the item at index \c i.  (\c i must be between [0, count()) )
	Tvalue& operator[](int i) { return values_[i]; }

	// non-const modification functions
	//=====================================

	/// Configure whether insert() and append() will accept keys that already exist. (When duplicate keys are accepted, indexOf() returns the index of the first item with that key.)   Note that if duplicates are already present when you change this from false to true, they will remain.
	void setAllowsDuplicateKeys(bool allowDuplicateKeys) { allowsDuplicateKeys_ = allowDuplicateKeys; }

	/// Append a new item to the end of the set. If allowsDuplicateKeys() is false, and an item with this key already exists in the set, returns false.
	bool append(const Tvalue& item, const Tkey& key) {
		if(!allowsDuplicateKeys_ && hash_.contains(key))
			return false;

		values_ << item;
		keys_ << key;
		int index = values_.count()-1;
		hash_.insertMulti(key, index);
		signalSource_->emitItemAdded(index);
		return true;
	}

	/// Insert a new item at \c index into the set. If allowsDuplicateKeys() is false, and an item with this key already exists in the set, returns false.  If \c index = -1 or count(), the item as appended to the end of the set.
	bool insert(int index, const Tvalue& item, const Tkey& key) {
		if(!allowsDuplicateKeys_ && hash_.contains(key))
			return false;


		// indexes of less than 0, or > count(), should append.
		if(index<0 || index>count())
			index = count();

		// indexes will change for all keys from this index onward. Remove old <key, index> associations from hash.
		for(int i=index; i<keys_.count(); i++)
			hash_.remove(keys_.at(i), i);

		values_.insert(index, item);
		keys_.insert(index, key);

		// re-index all from this index onward. (This includes the new guy, and all items after it, whose indexes were removed previously.)
		for(int i=index; i<keys_.count(); i++)
			hash_.insertMulti(keys_.at(i), i);

		signalSource_->emitItemAdded(index);
		return true;
	}



	/// Replace the item at index \c i with a \c newItem.  The old item is returned.  (\c i must be between [0, count()) )
	Tvalue replace(int i, const Tvalue& newItem, const Tkey& newKey) {
		Tvalue oldValue = values_.at(i);
		Tkey oldKey = keys_.at(i);
		values_[i] = newItem;
		keys_[i] = newKey;

		// remove old map from key to this index 'i', since key has changed.
		if(newKey != oldKey) {
			hash_.remove(oldKey, i);
			hash_.insertMulti(newKey, i);
		}

		signalSource_->emitItemChanged(i);
		return oldValue;
	}

	/// Remove the item at \c index.  The AMOrderedSetSignalSource::itemAboutToBeRemoved() signal will be emitted first.
	void remove(int index) {
		if(index<0 || index>=count())
			return;

		signalSource_->emitItemAboutToBeRemoved(index);

		values_.removeAt(index);

		for(int i=index; i<keys_.count(); i++)
			hash_.remove(keys_.at(i), i);

		keys_.removeAt(index);

		for(int i=index; i<keys_.count(); i++)
			hash_.insertMulti(keys_.at(i), i);

	}

	/// Clears the set, emitting aboutToBeRemoved() signals for each item.  \todo If nothing is connected to the signalSource(), there's a much faster way of doing this.
	void clear() {
		while(this->count())
			this->remove(this->count()-1);
	}


protected:
	QList<Tvalue> values_;
	QList<Tkey> keys_;
	QMultiHash<Tkey, int> hash_;
	bool allowsDuplicateKeys_;
	AMOrderedSetSignalSource* signalSource_;
};

#endif // AMORDEREDSET_H
