/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMDataSource.h"

AMDataSourceSignalSource::AMDataSourceSignalSource(AMDataSource *parent)
	: QObject() {
	data_ = parent;
}

AMDataSource::AMDataSource(const QString& name)
	: name_(name)
{
	signalSource_ = new AMDataSourceSignalSource(this);

	visibleInPlots_ = true;
	hiddenFromUsers_ = false;
}

AMDataSource::~AMDataSource() {
	signalSource_->emitDeleted();
	delete signalSource_;
	signalSource_ = 0;
}
