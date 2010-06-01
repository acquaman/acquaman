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

#endif // AMBIHASH_H
