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


#include "AMnDIndex.h"

// Create an invalid index (rank of 0)
 AMnDIndex::~AMnDIndex(){}
AMnDIndex::AMnDIndex() {
	rank_ = 0;
	extras_ = 0;
}

// Create an index for 1D data
AMnDIndex::AMnDIndex(long row) {
	rank_ = 1;
	extras_ = 0;
	dims_[0] = row;
}

// Create an index for 2D data
AMnDIndex::AMnDIndex(long row, long col) {
	rank_ = 2;
	extras_ = 0;
	dims_[0] = row;
	dims_[1] = col;
}
// Create an index for 3D data
AMnDIndex::AMnDIndex(long row, long col, long slice) {
	rank_ = 3;
	extras_ = 0;
	dims_[0] = row;
	dims_[1] = col;
	dims_[2] = slice;
}

// Create an index for 4D data
AMnDIndex::AMnDIndex(long i, long j, long k, long l) {
	rank_ = 4;
	extras_ = 0;
	dims_[0] = i;
	dims_[1] = j;
	dims_[2] = k;
	dims_[3] = l;
}

// Create an index for 5D data
AMnDIndex::AMnDIndex(long i, long j, long k, long l, long m) {
	rank_ = 5;
	extras_ = 0;
	dims_[0] = i;
	dims_[1] = j;
	dims_[2] = k;
	dims_[3] = l;
	dims_[4] = m;
}

AMnDIndex::AMnDIndex(int rank, InitializeFlag initialize, long initialValue)  {
	rank_ = rank;
	if(initialize == DoInit) {
		if(rank_ > 8)
			extras_ = new QVector<long>(rank_, initialValue);
		else
			extras_ = 0;

		if(initialValue == 0)
			memset(dims_, 0, 8*sizeof(long));
		else
			for(int mu=0;mu<8;++mu)
				dims_[mu] = initialValue;
	}

	else {	// do not init
		if(rank_ > 8)
			extras_ = new QVector<long>(rank_);
		else
			extras_ = 0;
	}
}

AMnDIndex::AMnDIndex(const AMnDIndex &other) {
	rank_ = other.rank_;
	memcpy(dims_, other.dims_, 8*sizeof(long));

	if(AM_UNLIKELY(rank_ > 8))
		extras_ = new QVector<long>(*(other.extras_));
	else
		extras_ = 0;
}

AMnDIndex& AMnDIndex::operator=(const AMnDIndex& other) {
	if(AM_UNLIKELY(other.rank_ > 8)) {
		if(rank_ > 8)	// already have an extras_; assign from other's.
			*extras_ = *(other.extras_);
		else	// need to make an extras_
			extras_ = new QVector<long>(*(other.extras_));
	}

	// need to get rid of our extras_ ?
	if(AM_UNLIKELY(rank_ > 8 && other.rank_ <=8)) {
		delete extras_;
		extras_ = 0;
	}

	rank_ = other.rank();
	memcpy(dims_, other.dims_, 8*sizeof(long));

	return *this;
}


#include <QStringList>

QString AMnDIndex::toString(const QString &separator) const {
	QStringList rv;
	for(int i=0; i<rank(); i++)
		rv << QString::number(at(i));
	return rv.join(separator);
}

