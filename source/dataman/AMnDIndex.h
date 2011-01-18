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

#include <QVarLengthArray>

/// AMnDArray will avoid using dynamic memory for up to this many dimensions. This makes it very fast to create.  If you routinely need multi-dimensional indexes for more than 8 dimensions, and create them in tight loops, you can increase this number.
#define AMNDARRAY_BASE_SIZE 8

/// This class provides an n-dimensional index.  It is used within the dataman framework whenever you need to specify an index into a multi-dimensional data set.
/*! Convenience constructors are provided to create an index suitable for 1D, 2D, 3D, and 4D datasets.  You can access the index for the first, second, third, and fourth axes with .i(), .j(), .k(), and .l()... (or, in Igor-style notation: .row(), .col(), .slice(), .segment()) respectively.

Internally, AMnDIndex is just a QVarLengthArray<int>, so you can always use the QVector/QVarLengthArray API to append elements to increase the rank beyond 4 dimensions, as well as access the indexes for these higher dimensions using operator[].

\note AMnDIndex is a QVarLength array instead of a QVector, which means that it can be created very fast -- it avoids the overhead associated with dynamic memory allocation.  Room is available for AMNDARRAY_BASE_SIZE dimensions on the stack... If you append more than (8) dimensions, it will do automatically do a heap allocation and use dynamic memory instead. For more information, see the documentation for QVarLengthArray.

\note For speed of access, there is no bounds checking on the i(), j(), k(), l(), and row()/col()/slice()/segment() convenience functions.  Accessing higher-dimensionality than the rank() of the AMnDIndex will either return garbage data (up to AMNDARRAY_BASE_SIZE), or cause a crash.


<b>Examples</b>
<code>
  - 1D: AMnDIndex(3)     // Use to access the 4th element in a 1D array   ---> [3]
  - 2D: AMnDIndex(1, 0)  // Use to access the element at the 2nd row and 1st column of a 2D dataset --> [1, 0]
  - 3D: AMnDIndex(1,2,1) // you get the picture... ---> [1,2,1]

  AMnDIndex myIndex(1,2,7);
  cout << myIndex.i();	// '1'
  cout << myIndex.j();	// '2'
  cout << myIndex.k();	// '7'
  cout << myIndex.l();	// Will cause a crash. For speed, there is no bounds checking
</code>

*/

class AMnDIndex : public QVarLengthArray<int, AMNDARRAY_BASE_SIZE>
{
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
	inline int rank() const { return count(); }

	/// Is the index valid? Invalid indexes have a rank of 0.
	inline bool isValid() const { return !isEmpty(); }

	/// Return the index for 1st independent axis of the multi-dimensional data set
	inline int i() const { return (*this)[0]; }
	/// Return the index for 2nd independent axis of the multi-dimensional data set
	inline int j() const { return (*this)[1]; }
	/// Return the index for 3rd independent axis of the multi-dimensional data set
	inline int k() const { return (*this)[2]; }
	/// Return the index for 4th independent axis of the multi-dimensional data set
	inline int l() const { return (*this)[3]; }

	/// Synonym for i()
	inline int row() const { return (*this)[0]; }
	/// Synonym for j()
	inline int col() const { return (*this)[1]; }
	/// Synonym for k()
	inline int slice() const { return (*this)[2]; }
	/// Synonym for l()
	inline int segment() const { return (*this)[3]; }

	/// Append all the indices from \c another AMnDIndex to this one.
	inline void append(const AMnDIndex& another) {
		QVarLengthArray<int, AMNDARRAY_BASE_SIZE>::append(another.constData(), another.size());
	}

	/// Check to see if another AMnDIndex has the same dimensions as this one
	inline bool dimensionsMatch(const AMnDIndex& other){
		if( count() != other.rank() )
			return false;
		return true;
	}

	/// Check to see if another AMnDIndex refers to valid index within this one
	inline bool inBounds(const AMnDIndex& other){
		for(int x=0; x < count(); x++)
			if( other[x] >= (*this)[x] )
				return false;
		return true;
	}

	inline bool operator==(const AMnDIndex& other) const {
		if(other.size() != size())
			return false;
		for(int i=0; i<size(); i++) {
			if(other[i] != (*this)[i])
				return false;
		}
		return true;
	}

	inline bool operator!=(const AMnDIndex& other) const {
		return !(*this == other);
	}

};


 #include <QMetaType>
Q_DECLARE_METATYPE(AMnDIndex)

#endif // AMNDINDEX_H
