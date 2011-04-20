/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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
/*! Convenience constructors are provided to create an index suitable for 1D, 2D, 3D, and 4D datasets.  You can access the index for the first, second, third, and fourth axes with .i(), .j(), .k(), and .l()... (or, in Igor-style notation: .row(), .col(), .slice(), .segment()) respectively.

You can always use the access the indexes for the higher dimensions using at() and operator[]().

\note AMnDIndex uses stack memory, which means that it can be created very fast -- it avoids the overhead associated with dynamic memory allocation.  Room is available for 8 dimensions on the stack... If you change the rank to more than (8) dimensions (using setRank), it will do automatically do a heap allocation and use dynamic memory as well.

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

class AMnDIndex
{
protected:
	int i_, j_, k_, l_, m_, n_, o_, p_;
	int rank_;
	QVector<int>* extras_;

public:
	/// Create an invalid index (rank of 0)
	AMnDIndex();
	/// Create an index for 1D data
	AMnDIndex(int row);
	/// Create an index for 2D data
	AMnDIndex(int row, int col);
	/// Create an index for 3D data
	AMnDIndex(int row, int col, int slice);
	/// Create an index for 4D data
	AMnDIndex(int i, int j, int k, int l);

	/// Optimized for performance: create an index of the initial size \c dimension.  If \c initToZero is true, all the values are initialized to 0.
	AMnDIndex(int dimension, bool initToZero);

	/// Returns the rank (# of dimensions that this index has available)
	inline int rank() const { return rank_; }
	/// Synonym for rank()
	inline int size() const { return rank_; }

	/// Is the index valid? Invalid indexes have a rank of 0.
	inline bool isValid() const { return rank_ != 0; }

	/// Return the index for 1st independent axis of the multi-dimensional data set
	inline int i() const { return i_; }
	/// Return the index for 2nd independent axis of the multi-dimensional data set
	inline int j() const { return j_; }
	/// Return the index for 3rd independent axis of the multi-dimensional data set
	inline int k() const { return k_; }
	/// Return the index for 4th independent axis of the multi-dimensional data set
	inline int l() const { return l_; }

	inline int m() const { return m_; }
	inline int n() const { return n_; }
	inline int o() const { return o_; }
	inline int p() const { return p_; }




	/// Synonym for i()
	inline int row() const { return i_; }
	/// Synonym for j()
	inline int col() const { return j_; }
	/// Synonym for k()
	inline int slice() const { return k_; }
	/// Synonym for l()
	inline int segment() const { return l_; }

	/// Index-based read-only access:
	inline int at(int dimIndex) const {
		switch(dimIndex) {
		case 0: return i_;
		case 1: return j_;
		case 2: return k_;
		case 3: return l_;
		case 4: return m_;
		case 5: return n_;
		case 6: return o_;
		case 7: return p_;
		default:
			return extras_->at(dimIndex-8);
		}
	}

	/// Index-based modify access:
	inline int& operator[](int dimIndex) {
		switch(dimIndex) {
		case 0: return i_;
		case 1: return j_;
		case 2: return k_;
		case 3: return l_;
		case 4: return m_;
		case 5: return n_;
		case 6: return o_;
		case 7: return p_;
		default:
			return extras_->operator[](dimIndex-8);
		}
	}

	/*
	/// Index-based modify access:
	inline const int& operator[](int dimIndex) const {
		switch(dimIndex) {
		case 0: return i_;
		case 1: return j_;
		case 2: return k_;
		case 3: return l_;
		case 4: return m_;
		case 5: return n_;
		case 6: return o_;
		case 7: return p_;
		default:
			return extras_->operator[](dimIndex-8);
		}
	}*/


	/// Change the size (rank) of the index
	inline void setRank(int newRank) {
		rank_ = newRank;
		if(rank_ < 9) {
			if(extras_) {
				delete extras_;
				extras_ = 0;
			}
		}
		else {	// rank >= 9. need extra space
			if(!extras_)
				extras_ = new QVector<int>(rank_-8);
			else
				extras_->resize(rank_-8);
		}
	}

	/// Append all the indices from \c another AMnDIndex to this one.
	inline void append(const AMnDIndex& another) {
		int originalRank = rank_;
		int combinedRank = rank_+another.rank_;
		setRank(combinedRank);
		for(int mu=originalRank; mu<combinedRank; mu++)
			operator[](mu) = another.at(mu-originalRank);
	}

	/// Check to see if another AMnDIndex has the same dimensions as this one
	inline bool dimensionsMatch(const AMnDIndex& other){
		if( rank_ != other.rank_ )
			return false;
		return true;
	}

	/// Check to see if another AMnDIndex refers to valid index within this one.
	/// \warning Assumes that \c other is the same rank as this one.
	inline bool inBounds(const AMnDIndex& other){
		for(int x=0; x < rank_; x++)
			if( other.at(x) >= at(x) )
				return false;
		return true;
	}

	inline bool operator==(const AMnDIndex& other) const {
		if(other.rank_ != rank_)
			return false;
		for(int i=0; i<rank_; i++) {
			if(other.at(i) != at(i))
				return false;
		}
		return true;
	}

	inline bool operator!=(const AMnDIndex& other) const {
		return !(*this == other);
	}

	/// Print out dimensions: ex: "3 x 7 x 256"
	QString toString(const QString& separator = QString(" x ")) const;

};


 #include <QMetaType>
Q_DECLARE_METATYPE(AMnDIndex)

#endif // AMNDINDEX_H
