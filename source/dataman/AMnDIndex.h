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


#ifndef AMNDINDEX_H
#define AMNDINDEX_H



/// This class provides an n-dimensional index.  It is used within the dataman framework whenever you need to specify an index into a multi-dimensional data set.
/*! Convenience constructors are provided to create an index suitable for 1D, 2D, 3D, and 4D datasets.  You can access the index for the first, second, third, and fourth axes with .i(), .j(), .k(), and .l()...

You can always use the access the indexes for the higher dimensions using at() and operator[]().

\note AMnDIndex uses stack memory, which means that it can be created very fast -- it avoids the overhead associated with dynamic memory allocation.  Room is available for 8 dimensions on the stack... If you change the rank to more than (8) dimensions (using setRank), it will automatically do a heap allocation and use dynamic memory as well.

\note For speed of access, there is no bounds checking on the i(), j(), k(), l(), and row()/col()/slice()/segment() convenience functions.  Accessing higher-dimensionality than the rank() of the AMnDIndex will either return garbage data (up to 8), or cause a crash.


<b>Examples</b>
<code>
  - 1D: AMnDIndex(3)     // Use to access the 4th element in a 1D array   ---> [3]
  - 2D: AMnDIndex(1, 0)  // Use to access the element at the 2nd row and 1st column of a 2D dataset --> [1, 0]
  - 3D: AMnDIndex(1,2,1) // you get the picture... ---> [1,2,1]

  AMnDIndex myIndex(1,2,7);
  cout << myIndex.i();	// '1'
  cout << myIndex.j();	// '2'
  cout << myIndex.k();	// '7'
  cout << myIndex.l();	// Will return garbage. For speed, there is no bounds checking
</code>

*/

#include <QVector>
#include <QString>

#include "util/amlikely.h"

class AMnDIndex
{
protected:
	/// For ranks less than 9, dimension values are stored in this static array.
	long dims_[8];
	/// The number of dimensions.
	int rank_;
	/// For ranks larger than 9, the remaining dimension values are stored in extras_, but at indexes 8 and higher. This allows a fast implementation of constData(). Note that dims_ is always the authoritative storage location for the first 8 indexes.
	QVector<long>* extras_;

public:
	/// Create an invalid index (rank of 0)
	AMnDIndex();
	/// Create an index for 1D data
	AMnDIndex(long row);
	/// Create an index for 2D data
	AMnDIndex(long row, long col);
	/// Create an index for 3D data
	AMnDIndex(long row, long col, long slice);
	/// Create an index for 4D data
	AMnDIndex(long i, long j, long k, long l);
	/// Create an index for 5D data
	AMnDIndex(long i, long j, long k, long l, long m);
	/// Destructor.
	virtual ~AMnDIndex();

	/// Flag to indicate whether the dimension values should be initialized in the following constructor.
	enum InitializeFlag { DoNotInit, DoInit };
	/// Optimized for performance: Construct an index with the initial size of \c rank.  If \c initialize is AMnDIndex::Init, all the values are initialized to \c initialValue.
	AMnDIndex(int rank, InitializeFlag initialize, long initialValue = 0);


	/// Copy constructor
	AMnDIndex(const AMnDIndex& other);
	/// Assignment operator
	AMnDIndex& operator=(const AMnDIndex& other);

	// Constant access functions
	///////////////////////////////////

	/// Returns the rank (# of dimensions that this index has available)
	inline int rank() const { return rank_; }

	/// Is the index valid? Invalid indexes have a rank of 0.
	inline bool isValid() const { return rank_ != 0; }

	/// Return the index for 1st independent axis of the multi-dimensional data set
	inline long i() const { return dims_[0]; }
	/// Return the index for 2nd independent axis of the multi-dimensional data set
	inline long j() const { return dims_[1]; }
	/// Return the index for 3rd independent axis of the multi-dimensional data set
	inline long k() const { return dims_[2]; }
	/// Return the index for 4th independent axis of the multi-dimensional data set
	inline long l() const { return dims_[3]; }

	inline long m() const { return dims_[4]; }
	inline long n() const { return dims_[5]; }
	inline long o() const { return dims_[6]; }
	inline long p() const { return dims_[7]; }


	/// Index-based read-only access:
	inline long at(int dimIndex) const {
		if(AM_LIKELY(dimIndex < 8))
			return dims_[dimIndex];
		else
			return extras_->at(dimIndex);
	}

	/// Lots of C math / data handling libraries often need a plain array of longs to use as multi-dimension indexes. The pointer returned by this function is only valid while the AMnDIndex still exists.
	inline const long* constData() const {
		if(AM_LIKELY(rank_ <= 8))
			return dims_;
		else {
			// copy first 8 elements from dims_ into extras_, before we return extras_. [Most of the time, the first 8 elements of extras_ are ignored/garbage.]
			memcpy(extras_->data(), dims_, 8*sizeof(long));
			return extras_->constData();
		}
	}

	// Modification functions
	////////////////////////////////

	/// Index-based modify access:
	inline long& operator[](int dimIndex) {
		if(AM_LIKELY(dimIndex < 8))
			return dims_[dimIndex];
		else
			return extras_->operator[](dimIndex);
	}
	/// Change the size (rank) of the index
	inline void setRank(int newRank) {
		rank_ = newRank;
		if(rank_ < 9) {
			if(extras_) {
				delete extras_;
				extras_ = 0;
			}
		}
		else {	// rank > 8. need extra space
			if(!extras_)
				extras_ = new QVector<long>(rank_);
			else
				extras_->resize(rank_);
		}
	}

	/// Set all the dimensions of the index to \c value
	inline void setAll(long value) {
		for(int mu=0; mu<rank_; ++mu)
			operator[](mu) = value;
	}

	/// Append all the indices from \c another AMnDIndex to this one.
	inline void append(const AMnDIndex& another) {
		int originalRank = rank_;
		int combinedRank = rank_+another.rank_;
		setRank(combinedRank);
		for(int mu=originalRank; mu<combinedRank; mu++)
			operator[](mu) = another.at(mu-originalRank);
	}

	// Comparison and other functions
	/////////////////////////////////////

	/// Check to see if another AMnDIndex has the same dimensions as this one
	inline bool dimensionsMatch(const AMnDIndex& other){
		if( rank_ != other.rank_ )
			return false;
		return true;
	}

	/// Check to see if another AMnDIndex refers to valid index within this one.
	/*! \warning Assumes that \c other is the same rank as us. */
	inline bool inBounds(const AMnDIndex& other){
		for(int x=0; x < rank_; x++)
			if( other.at(x) >= at(x) )
				return false;
		return true;
	}

	/// Check to see if this index is valid within the bounds of a multi-dimensional array \c fullSize
	/*! \warning Assumes that \c fullSize is the same rank as us. */
	inline bool validInArrayOfSize(const AMnDIndex& fullSize) const {
		for(int mu=0; mu<rank_; ++mu)
			if(at(mu) >= fullSize.at(mu))
				return false;
		return true;
	}

	/// Comparison operator
	inline bool operator==(const AMnDIndex& other) const {
		if(other.rank_ != rank_)
			return false;
		for(int i=0; i<rank_; i++) {
			if(other.at(i) != at(i))
				return false;
		}
		return true;
	}

	/// Inequality operator
	inline bool operator!=(const AMnDIndex& other) const {
		return !(*this == other);
	}

	/// Calculate the product of all the dimensions. (If the AMnDIndex represents the size of a multidimensional array, this calculates the total number of elements in that array.)
	inline long product() const {
		long rv = 1;
		for(int mu=rank_-1; mu>=0; --mu)
			rv *= at(mu);
		return rv;
	}

	/// Calculate the total number of points in the space of the data block between this index and \c toIndex (inclusive).  For example, if this index is (2,2) and \c toIndex = (3,3), the block includes (2,2),(2,3),(3,2),(3,3) and this function would return 4.
	/*! \note Assumes that \c toIndex has the same rank as us, and also that all of its corresponding values are larger than ours. */
	inline long totalPointsTo(const AMnDIndex& toIndex) const {
		long rv = 1;
		for(int mu = rank_-1; mu >= 0; --mu)
			rv *= (toIndex.at(mu) - at(mu) + 1);
		return rv;
	}

	/// Calculate the location where this index would end up in a flattened (1D) array, where \c fullSize gives the total size of the multi-dimensional array.  (This assumes row-major order, where the first index varies the slowest.)  For example, for a 4-dimensional AMnDIndex(3,4,5,600) and an array that has dimensions \c fullSize = (5,10,100,1000), the flat index is 3*10*100*1000 + 4*100*1000 + 5*1000 + 600.
	/*! \note Assumes that \c fullSize has the same rank() as us.*/
	inline long flatIndexInArrayOfSize(const AMnDIndex& fullSize) const {
		long rv;

		switch(rank_) {
		case 0:
			rv =  0; break;
		case 1:
			rv = i();
			break;
		case 2:
			rv = i()*fullSize.at(1)
					+ j();
			break;
		case 3: {
			rv = i()*fullSize.at(1)*fullSize.at(2)
					+ j()*fullSize.at(2)
					+ k();
			break; }

		case 4: {
			rv = i()*fullSize.at(1)*fullSize.at(2)*fullSize.at(3)
					+ j()*fullSize.at(2)*fullSize.at(3)
					+ k()*fullSize.at(3)
					+ l();
			break; }

		default: {
			rv = 0;
			for(int mu=0; mu<rank_; ++mu) {
				long multiplier = 1;
				for(int nu=mu+1; nu<rank_; ++nu)
					multiplier *= fullSize.at(nu);
				rv += at(mu)*multiplier;
			}
			break; }
		}

		return rv;
	}

	/// Returns a multidimensional index corresponding to the \c flatIndex in a 1D array , where the multidimensional array has size \c fullSize.    Like flatIndexInArrayOfSize(), this assumes row-major order where the last index varies the fastest.
	static AMnDIndex fromFlatIndexInArrayOfSize(const AMnDIndex& fullSize, long flatIndex) {
		int rank = fullSize.rank_;
		AMnDIndex rv(rank, AMnDIndex::DoNotInit);

		switch(rank) {
		case 0:
			break;
		case 1:
			rv[0] = flatIndex;
			break;
		case 2:
			rv[0] = flatIndex / fullSize.j();
			rv[1] = flatIndex % fullSize.j();
			break;
		case 3: {
			long multiplier = fullSize.j()*fullSize.k();
			rv[0] = flatIndex / multiplier;
			flatIndex = flatIndex % multiplier;
			rv[1] = flatIndex / fullSize.k();
			rv[2] = flatIndex % fullSize.k();
			break; }

		case 4: {
			long multiplier = fullSize.j()*fullSize.k()*fullSize.l();
			rv[0] = flatIndex / multiplier;
			flatIndex = flatIndex % multiplier;

			multiplier = fullSize.k()*fullSize.l();
			rv[1] = flatIndex / multiplier;
			flatIndex = flatIndex % multiplier;

			multiplier = fullSize.l();
			rv[2] = flatIndex / multiplier;
			rv[3] = flatIndex % multiplier;
			break; }

		default: {
			for(int mu=0; mu<rank; ++mu) {
				long multiplier = 1;
				for(int nu=mu+1; nu<rank; ++nu)
					multiplier *= fullSize.at(nu);
				rv[mu] = flatIndex / multiplier;
				flatIndex = flatIndex % multiplier;
			}
			break; }
		}

		return rv;
	}


	// Overloaded operators: increasing or decreasing an index dimension-by-dimension
	/////////////////////////////

	/// Adds \c other to this index, dimension by dimension.
	/*! \warning Assumes that \c other is the same rank as us. */
	inline AMnDIndex& operator+=(const AMnDIndex& other) {
		for(int mu=0; mu<rank_; ++mu)
			(*this)[mu] += other.at(mu);
		return *this;
	}

	/// Returns an index that is the dimension-by-dimension sum of us and \c other.
	/*! \warning Assumes that \c other has the same rank as us. */
	inline AMnDIndex operator+(const AMnDIndex& other) const {
		AMnDIndex rv(*this);
		rv += other;
		return rv;
	}

	/// Subtracts \c other from this index, dimension by dimension.
	/*! \warning Assumes that \c other has the same rank as us. */
	inline AMnDIndex& operator-=(const AMnDIndex& other) {
		for(int mu=0; mu<rank_; ++mu)
			(*this)[mu] -= other.at(mu);
		return *this;
	}

	/// Returns an index that is the dimension-by-dimension subtraction of \c other from this index.
	/*! \warning Assumes that \c other has the same rank as us. */
	inline AMnDIndex operator-(const AMnDIndex& other) const {
		AMnDIndex rv(*this);
		rv -= other;
		return rv;
	}

	/// Adds \c offset to every dimension in this index.
	inline AMnDIndex& operator+=(long offset) {
		for(int mu=0; mu<rank_; ++mu)
			(*this)[mu] += offset;
		return *this;
	}

	/// Returns an index created by adding \c offset to every dimension of this index.
	inline AMnDIndex operator+(long offset) const {
		AMnDIndex rv(*this);
		rv += offset;
		return rv;
	}

	/// Subtracts \c offset from every dimension in this index.
	inline AMnDIndex& operator-=(long offset) {
		for(int mu=0; mu<rank_; ++mu)
			(*this)[mu] -= offset;
		return *this;
	}

	/// Returns an index created by subtracting \c offset from every dimension of this index.
	inline AMnDIndex operator-(long offset) const {
		AMnDIndex rv(*this);
		rv -= offset;
		return rv;
	}


	// Misc.
	//////////////////////////

	/// Print out dimensions: ex: "3 x 7 x 256"
	QString toString(const QString& separator = QString(" x ")) const;

};



 #include <QMetaType>
Q_DECLARE_METATYPE(AMnDIndex)

#endif // AMNDINDEX_H
