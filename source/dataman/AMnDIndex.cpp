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
AMnDIndex::AMnDIndex() {
	rank_ = 0;
	extras_ = 0;
}

/// Create an index for 1D data
AMnDIndex::AMnDIndex(int row) {
	rank_ = 1;
	extras_ = 0;
	i_ = row;
}

/// Create an index for 2D data
AMnDIndex::AMnDIndex(int row, int col) {
	rank_ = 2;
	extras_ = 0;
	i_ = row;
	j_ = col;
}
/// Create an index for 3D data
AMnDIndex::AMnDIndex(int row, int col, int slice) {
	rank_ = 3;
	extras_ = 0;
	i_ = row;
	j_ = col;
	k_ = slice;
}

/// Create an index for 4D data
AMnDIndex::AMnDIndex(int i, int j, int k, int l) {
	rank_ = 4;
	extras_ = 0;
	i_ = i;
	j_ = j;
	k_ = k;
	l_ = l;
}

AMnDIndex::AMnDIndex(int dimension, bool initToZero)  {
	rank_ = dimension;
	if(initToZero) {
		if(rank_ > 8)
			extras_ = new QVector<int>(rank_-8, 0);
		else
			extras_ = 0;
		i_ = j_ = k_ = l_ = m_ = n_ = o_ = p_ = 0;
	}

	else {	// do not init
		if(rank_ > 8)
			extras_ = new QVector<int>(rank_-8);
		else
			extras_ = 0;
	}
}

