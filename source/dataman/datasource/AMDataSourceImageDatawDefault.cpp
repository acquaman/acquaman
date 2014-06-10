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


#include "AMDataSourceImageDatawDefault.h"

 AMDataSourceImageDatawDefault::~AMDataSourceImageDatawDefault(){}
AMDataSourceImageDatawDefault::AMDataSourceImageDatawDefault(const AMDataSource *dataSource, double defaultValue, QObject *parent)
	: AMDataSourceImageData(dataSource, parent)
{
	defaultValue_ = defaultValue;
}

void AMDataSourceImageDatawDefault::minMaxSearch() const
{
	QPoint c = count();
	int sizeX = c.x();
	int sizeY = c.y();

	if(sizeX == 0 || sizeY == 0)
		return;

	// performance optimization.  If total points is less than 500, just call z() repeatedly.  If total points is over that, usually faster to allocate a vector and use the block zValues().  However, to limit memory usage, don't allocate blocks over 1MB (125000 doubles)
	if(sizeX*sizeY < 500) {
		qreal minZ, maxZ, d;
		minZ = maxZ = z(0,0);
		for(int xx=0; xx<sizeX; ++xx)
			for(int yy=0; yy<sizeY; ++yy) {
				d = z(xx,yy);
				if(d<minZ && d!=defaultValue_) minZ=d;
				if(d>maxZ) maxZ=d;
			}
		minMaxCache_.first = minZ;
		minMaxCache_.second = maxZ;
		minMaxCacheUpdateRequired_ = false;
	}
	else if(sizeX*sizeY < 125000) {	// less than 1MB buffer: do in one shot
		QVector<qreal> dataBuffer(sizeX*sizeY);
		zValues(0,0, sizeX-1, sizeY-1, dataBuffer.data());
		qreal minZ, maxZ;
		minZ = maxZ = dataBuffer.at(0);
		foreach(qreal d, dataBuffer) {
			if(d<minZ && d!=defaultValue_) minZ=d;
			if(d>maxZ) maxZ=d;
		}
		minMaxCache_.first = minZ;
		minMaxCache_.second = maxZ;
		minMaxCacheUpdateRequired_ = false;
	}
	else {	// large array; don't want to allocate more than 1MB buffer. Do in sections of approximately 1MB.
		int rowsAtOnce = 125000 / sizeY;
		if(rowsAtOnce == 0) rowsAtOnce = 1;
		qreal minZ, maxZ;
		minZ = maxZ = z(0,0);
		QVector<qreal> dataBuffer(rowsAtOnce*sizeY);

		for(int xrow=0; xrow<sizeX; xrow+=rowsAtOnce) {
			int maxRow = qMin(sizeX-1, xrow+rowsAtOnce-1);
			dataBuffer.resize((maxRow-xrow+1)*sizeY);	// for all blocks except the last block, will do nothing. Resizing on the last (partial) block allows us to use foreach, which is faster than a for-loop over the space we know we have.
			zValues(xrow, 0, maxRow, sizeY-1, dataBuffer.data());

			foreach(qreal d, dataBuffer) {
				if(d<minZ && d!=defaultValue_) minZ=d;
				if(d>maxZ) maxZ=d;
			}
		}
		minMaxCache_.first = minZ;
		minMaxCache_.second = maxZ;
		minMaxCacheUpdateRequired_ = false;
	}
}
