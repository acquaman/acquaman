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


#include "AMnDIndex.h"

/// Create an invalid index (rank of 0)
AMnDIndex::AMnDIndex() : QVarLengthArray<int, AMNDARRAY_BASE_SIZE>(0) {
}

/// Create an index for 1D data
AMnDIndex::AMnDIndex(int row) : QVarLengthArray<int, AMNDARRAY_BASE_SIZE>(1) {
	(*this)[0] = row;
}

/// Create an index for 2D data
AMnDIndex::AMnDIndex(int row, int col)	: QVarLengthArray<int, AMNDARRAY_BASE_SIZE>(2) {
	(*this)[0] = row;
	(*this)[1] = col;
}
/// Create an index for 3D data
AMnDIndex::AMnDIndex(int row, int col, int slice) : QVarLengthArray<int, AMNDARRAY_BASE_SIZE>(3) {
	(*this)[0] = row;
	(*this)[1] = col;
	(*this)[2] = slice;
}

/// Create an index for 4D data
AMnDIndex::AMnDIndex(int i, int j, int k, int l) : QVarLengthArray<int, AMNDARRAY_BASE_SIZE>(4) {
	(*this)[0] = i;
	(*this)[1] = j;
	(*this)[2] = k;
	(*this)[3] = l;
}

AMnDIndex::AMnDIndex(int dimension, bool initToZero) : QVarLengthArray<int, AMNDARRAY_BASE_SIZE>(dimension) {
	if(initToZero)
		for(int i=0; i<dimension; i++)
			(*this)[i] = 0;
}

