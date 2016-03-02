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


#ifndef AMBIHASH_H
#define AMBIHASH_H

#include <QHash>

/// This utility class provides a container class that provides forward- and reverse- hash lookups of a <valueKey, valueKey> pair.  Each value can be used as a key to lookup the other value.
/*! Both types must be suitable as keys in a QHash, and provide a default-constructed value.
  */

template <class T1, class T2>
class AMBiHash
{
public:
	/// Default constructor
 	virtual ~AMBiHash(){}
	AMBiHash<T1, T2>() {}

	/// return the value for a key (forward lookup)
	T2 valueF(const T1& key1) const {
		return fwd_.value(key1);
	}

	/// return a "key" for a "value" (reverse lookup)
	T1 valueR(const T2& key2) const {
		return rev_.value(key2);
	}

	/// true if it contains the forward (first) key
	bool containsF(const T1& key1) const {
		return fwd_.contains(key1);
	}

	/// true if it contains the reverse (second) key
	bool containsR(const T2& key2) const {
		return rev_.contains(key2);
	}

	/// the number of pairs in the set:
	unsigned count() const {
		return fwd_.count();
	}

	/// clear the data
	void clear() {
		fwd_.clear();
		rev_.clear();
	}

	/// add or overwrite a keyValue-keyValue pair
	void set(const T1& key1, const T2& key2) {
		fwd_[key1] = key2;
		rev_[key2] = key1;
	}

	/// remove based on the forward (first) key
	void removeF(const T1& key1) {
		rev_.remove(fwd_[key1]);
		fwd_.remove(key1);
	}

	/// remove based on the reverse (second) key
	void removeR(const T2& key2) {
		fwd_.remove(rev_[key2]);
		rev_.remove(key2);
	}

	/// Returns a QHashIterator that will allow traversing the whole set of pairs
	QHashIterator<T1, T2> iterator() const {
		QHashIterator<T1, T2> i(fwd_);
		return i;
	}


protected:
	QHash<T1, T2> fwd_;
	QHash<T2, T1> rev_;
};

template <class T1, class T2>
class AMBiHashWChecking : public AMBiHash<T1, T2>
{
public:
	/// Default constructor
 	virtual ~AMBiHashWChecking(){}
	AMBiHashWChecking<T1, T2>() :
		AMBiHash<T1, T2>()
	{
		lookupFailed_ = false;
	}

	/// return the value for a key (forward lookup)
	T2 valueF(const T1& key1) const {
		if(!AMBiHash<T1, T2>::containsF(key1))
			lookupFailed_ = true;
		return AMBiHash<T1, T2>::valueF(key1);
	}

	/// return a "key" for a "value" (reverse lookup)
	T1 valueR(const T2& key2) const {
		if(!AMBiHash<T1, T2>::containsR(key2))
			lookupFailed_ = true;
		return AMBiHash<T1, T2>::valueR(key2);
	}

	bool lookupFailed() const { return lookupFailed_; }
	void resetLookup() { lookupFailed_ = false; }

protected:
	mutable bool lookupFailed_;
};

#endif // AMBIHASH_H
