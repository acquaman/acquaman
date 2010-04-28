#ifndef AMDATATREECOLUMN_H
#define AMDATATREECOLUMN_H

#include <QString>
#include <QVector>
#include <QVectorIterator>

/// The type of (numeric?) data stored. We're taking the overhead of using doubles for everything, for the simplicity of being able to accurately hold any numeric type.
typedef double AMNumericType;



/// An AMDataTree consists of an arbitrary number of columns of data. This class represents one of those columns.
/*! Essentially just a vector of values, this class provides some extra goodies:
	- a short (unique) name()
	- a longer description()
	- descriptive units()
	- and maximum() and minimum() values.

	This class is used internally by AMDataTree.  Sometimes (for performance) a read-only (const) reference to it is shared with objects that need to access the data in the tree.
	AMDataTrees don't give out non-const references to their columns, because they need to enforce that all of their columns have AMDataTree:count() datapoints.

	Copying a AMDataTreeColumn takes constant time, because its QVector base class is implicitly shared. This makes returning an AMDataTreeColumn from a function very fast. If a shared instance is modified, it will be copied (copy-on-write), and that takes linear time.
	*/
class AMDataTreeColumn : public QVector<AMNumericType> {

public:

	/// Types: typdefs to match QVector
	typedef QVector<AMNumericType>::reference reference;
	typedef QVector<AMNumericType>::iterator iterator;

	/// constructor (TODOTODOTOD remove default argument once QHash storage done)
	AMDataTreeColumn(const QString& name = "x", const QString& description = QString(), const QString& units = QString(), unsigned size = 0)
		: QVector<AMNumericType>(size),
		name_(name),
		description_(description),
		units_(units)
	{
		maxIndex_ = -1;
		minIndex_ = -1;

	}

	/// copy constructor
	AMDataTreeColumn(const AMDataTreeColumn& other)
		: QVector<AMNumericType>(other),
		name_(other.name_),
		description_(other.description_),
		units_(other.units_)
	{
		maxIndex_ = other.maxIndex_;
		minIndex_ = other.minIndex_;
	}

	/// assignment operator
	AMDataTreeColumn& operator=( const AMDataTreeColumn& other ) {
		QVector<AMNumericType>::operator=(other);
		name_ = other.name_;
		description_ = other.description_;
		units_ = other.units_;

		maxIndex_ = other.maxIndex_;
		minIndex_ = other.minIndex_;

		return *this;
	}

	/// accessor methods for metadata
	QString name() const { return name_; }
	QString description() const { return description_; }
	QString units() const { return units_; }

	/// setter methods for metadata
	void setName(const QString& newName) { name_ = newName; }
	void setDescription(const QString& description) { description_ = description; }
	void setUnits(const QString& units) { units_ = units; }

	/// maximum() and minimum() use a constant-time operation on every insert/append to avoid having to do a linear search every time they are called.
	/*! Another philosophy would just be to do simple caching (mark maxIndex and minIndex as dirty every time a non-const operation occurs). We'll test out this way first.
	  */
	AMNumericType maximum() const {
		if(isEmpty())
			return 0;
		if(maxIndex_ < 0)
			searchMax();
		return at(maxIndex_);
	}

	/// maximum() and minimum() use a constant-time operation on every insert/append to avoid having to do a linear search every time they are called.
	/*! Another philosophy would just be to do simple caching (mark maxIndex and minIndex as dirty every time a non-const operation occurs). We'll test out this way first.
	  */
	AMNumericType minimum() const {
		if(isEmpty())
			return 0;
		if(minIndex_ < 0)
			searchMin();
		return at(minIndex_);
	}

	/// overiding non-const base class methods to ensure tracking of min/max values
	// These are in the header file so that hopefully they will be inlined.
	//////////////////////////////////

	void append ( const AMNumericType & value ) {

		QVector<AMNumericType>::append(value);

		if(maxIndex_>=0 && value > at(maxIndex_))
			maxIndex_ = count() - 1;
		if(minIndex_>=0 && value < at(minIndex_))
			minIndex_ = count() - 1;

	}

	reference back () {
		maxIndex_ = minIndex_ = -1;
		return QVector<AMNumericType>::back();
	}

	iterator begin () {
		maxIndex_ = minIndex_ = -1;
		return QVector<AMNumericType>::begin();
	}

	void clear () {
		maxIndex_ = minIndex_ = -1;
		QVector<AMNumericType>::clear();
	}

	AMNumericType* data () {
		maxIndex_ = minIndex_ = -1;
		return QVector<AMNumericType>::data();
	}

	iterator end () {
		maxIndex_ = minIndex_ = -1;
		return QVector<AMNumericType>::end();
	}

	iterator erase ( iterator pos ) {
		if(maxIndex_>=0 && pos == data() + maxIndex_)
			maxIndex_ = -1;
		if(minIndex_>=0 && pos == data() + minIndex_)
			minIndex_ = -1;

		return QVector<AMNumericType>::erase(pos);
	}

	iterator erase ( iterator begin, iterator end ) {
		if(maxIndex_>=0 && data()+maxIndex_ >= begin && data()+maxIndex_ <= end)
			maxIndex_ = -1;
		if(minIndex_>=0 && data()+minIndex_ >= begin && data()+minIndex_ <= end)
			minIndex_ = -1;

		return QVector<AMNumericType>::erase(begin, end);

	}

	AMDataTreeColumn& fill ( const AMNumericType& value, int size = -1 ) {
		maxIndex_ = minIndex_ = -1;

		QVector<AMNumericType>::fill(value, size);

		return *this;
	}

	AMNumericType& first () {
		if(maxIndex_ == 0)
			maxIndex_ = -1;
		if(minIndex_ == 0)
			minIndex_ = -1;
		return QVector<AMNumericType>::first();
	}

	AMNumericType& front () {
		if(maxIndex_ == 0)
			maxIndex_ = -1;
		if(minIndex_ == 0)
			minIndex_ = -1;
		return QVector<AMNumericType>::front();
	}

	void insert ( int i, const AMNumericType& value ) {

		// are tracking the max, and inserting at or in front of maxIndex: must bump up maxIndex
		if(maxIndex_>=0 && i<=maxIndex_)
			maxIndex_++;
		if(minIndex_>=0 && i<=minIndex_)
			minIndex_++;

		QVector<AMNumericType>::insert(i, value);

		// this has guaranteed that maxIndex or minIndex are not == i. Now check: should the insert be boss?
		if(maxIndex_>=0 && value > at(maxIndex_))
			maxIndex_ = i;
		if(minIndex_>=0 && value < at(minIndex_))
			minIndex_ = i;

	}

	iterator insert ( iterator before, int count, const AMNumericType& value ) {
		// too lazy to figure this one out. flag both as dirty:
		maxIndex_ = minIndex_ = -1;
		return QVector<AMNumericType>::insert(before, count, value);
	}

	void insert ( int i, int count, const AMNumericType& value ) {
		// too lazy to figure this one out. flag both as dirty:
		maxIndex_ = minIndex_ = -1;
		return QVector<AMNumericType>::insert(i, count, value);
	}

	iterator insert ( iterator before, const AMNumericType& value ) {

		// are tracking the max, and inserting at or in front of maxIndex: must bump up maxIndex
		if(maxIndex_>=0 && before<=data()+maxIndex_)
			maxIndex_++;
		if(minIndex_>=0 && before<=data()+minIndex_)
			minIndex_++;

		iterator retval = QVector<AMNumericType>::insert(before, value);

		// this has guaranteed that maxIndex or minIndex are not == i. Now check: should the insert be boss?
		if(maxIndex_>=0 && value > at(maxIndex_))
			maxIndex_ = before-data();
		if(minIndex_>=0 && value < at(minIndex_))
			minIndex_ = before-data();

		return retval;
	}

	AMNumericType& last () {
		if(maxIndex_ == count()-1)
			maxIndex_ = -1;
		if(minIndex_ == count()-1)
			minIndex_ = -1;
		return QVector<AMNumericType>::last();
	}

	void pop_back () {
		if(maxIndex_ == count()-1)
			maxIndex_ = -1;
		if(minIndex_ == count()-1)
			minIndex_ = -1;

		QVector<AMNumericType>::pop_back();
	}

	void pop_front (){
		if(maxIndex_ == 0)
			maxIndex_ = -1;
		if(minIndex_ == 0)
			minIndex_ = -1;

		QVector<AMNumericType>::pop_front();
	}

	void prepend ( const AMNumericType& value ) {

		// if we're tracking the max, its index will increase by one
		if(maxIndex_>=0)
			maxIndex_++;
		if(minIndex_>=0)
			minIndex_++;

		QVector<AMNumericType>::prepend(value);

		// check for potential new bosses (at front)
		if(maxIndex_>=0 && value>at(maxIndex_))
			maxIndex_ = 0;
		if(minIndex_>=0 && value<at(minIndex_))
			minIndex_ = 0;

	}

	void push_back ( const AMNumericType& value ) {

		QVector<AMNumericType>::push_back(value);

		if(maxIndex_>=0 && value > at(maxIndex_))
			maxIndex_ = count() - 1;
		if(minIndex_>=0 && value < at(minIndex_))
			minIndex_ = count() - 1;
	}

	void push_front ( const AMNumericType& value ) {
		// if we're tracking the max, its index will increase by one
		if(maxIndex_>=0)
			maxIndex_++;
		if(minIndex_>=0)
			minIndex_++;

		QVector<AMNumericType>::push_front(value);

		// check for potential new bosses (at front)
		if(maxIndex_>=0 && value>at(maxIndex_))
			maxIndex_ = 0;
		if(minIndex_>=0 && value<at(minIndex_))
			minIndex_ = 0;
	}

	void remove ( int i ) {
		if(maxIndex_ == i)
			maxIndex_ = -1;
		if(minIndex_ == i)
			minIndex_ = -1;

		QVector<AMNumericType>::remove(i);
	}

	void remove ( int i, int count ) {
		if(maxIndex_>=i && maxIndex_ <= i+count)
			maxIndex_ = -1;
		if(minIndex_>=i && minIndex_ <= i+count)
			minIndex_ = -1;

		QVector<AMNumericType>::remove(i,count);
	}

	void replace ( int i, const AMNumericType& value ) {
		if(maxIndex_ == i)
			maxIndex_ = -1;
		if(minIndex_ == i)
			minIndex_ = -1;

		QVector<AMNumericType>::replace(i, value);
	}

	void resize ( int size ) {
		maxIndex_ = minIndex_ = -1;
		QVector<AMNumericType>::resize(size);
	}

	AMDataTreeColumn& operator+= ( const AMDataTreeColumn& other ) {
		maxIndex_ = minIndex_ = -1;
		QVector<AMNumericType>::operator +=(other);

		/// \todo What to do about names, units, description?
		// name_ += " + " + other.name_;
		// description_ += ...

		return *this;
	}

	AMDataTreeColumn& operator+= ( const AMNumericType& value ) {
		QVector<AMNumericType>::operator+=(value);

		if(maxIndex_>=0 && value > at(maxIndex_))
			maxIndex_ = count() - 1;
		if(minIndex_>=0 && value < at(minIndex_))
			minIndex_ = count() - 1;

		return *this;
	}

	AMDataTreeColumn& operator<< ( const AMNumericType& value ) {

		QVector<AMNumericType>::operator<<(value);

		if(maxIndex_>=0 && value > at(maxIndex_))
			maxIndex_ = count() - 1;
		if(minIndex_>=0 && value < at(minIndex_))
			minIndex_ = count() - 1;

		return *this;
	}

	AMDataTreeColumn& operator<< ( const AMDataTreeColumn& other ) {

		// if either aren't tracking their max, it's going to be unknown
		if(maxIndex_<0 || other.maxIndex_<0)
			maxIndex_ = -1;
		// otherwise (both tracking...) if other's max is bigger, it's boss. Note that it'll be ahead by count() after we append.
		else if( other.at(other.maxIndex_) > at(maxIndex_) )
			maxIndex_ = other.maxIndex_ + count();

		if(minIndex_<0 || other.minIndex_<0)
			minIndex_ = -1;
		else if( other.at(other.minIndex_) < at(minIndex_))
			minIndex_ = other.minIndex_ + count();

		QVector<AMNumericType>::operator<<(other);

		return *this;

	}


	AMNumericType& operator[] ( int i ) {
		// this reference can be used to change values, possibly creating a new max or new min at this index. Therefore, to be safe, both max and min trackers are now invalid.

		maxIndex_ = minIndex_ = -1;
		// for better max/min tracking performance, use setValue(AMNumericType value, int i) instead.

		return QVector<AMNumericType>::operator[](i);
	}


	// aparently if you overide one of a base-class's overloaded functions, you have to overide them all. Otherwise the compiler only tries using the one you overrode.
	// here we trivially call the base implementation
	AMNumericType operator[] ( int i ) const  {
		return QVector<AMNumericType>::operator[](i);
	}

	/// This method of changing values is preferred over using operator[], because it can maintain the max and min tracking as valid.
	void setValue(const AMNumericType& value, int i) {

		// if we're replacing what used to be the max, and this value is smaller, we don't know who the max is anymore.
		if(i == maxIndex_ && value < at(maxIndex_) )
			maxIndex_ = -1;
		if(i == minIndex_ && value > at(minIndex_) )
			minIndex_ = -1;

		// if we're tracking the maximum, and this value is larger, it becomes the new maximum
		if(maxIndex_>=0 && value > at(maxIndex_) )
			maxIndex_ = i;
		if(minIndex_>=0 && value < at(minIndex_) )
			minIndex_ = i;

		QVector<AMNumericType>::operator[](i) = value;

	}

	//////////////////////////////////

protected:
	/// used internally to locate the max value if a search is req'd
	void searchMax() const;
	/// used internally to locate the min value if a search is req'd
	void searchMin() const;

	/// used to track the maximum and minimum values
	mutable int maxIndex_, minIndex_;

	/// Metadata parameters
	QString name_, description_, units_;

};

#endif // AMDATATREECOLUMN_H
