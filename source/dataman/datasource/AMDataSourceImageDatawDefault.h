/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AMDATASOURCEIMAGEDATAWDEFAULT_H
#define AMDATASOURCEIMAGEDATAWDEFAULT_H

#include <QObject>
#include "dataman/datasource/AMDataSourceImageData.h"

/// This class extends the standard AMDataSourceImageData to include a default value that is not used when computing the range.
/*!
	The purpose of this class is to provide a model for images that can give an accurate visual
	representation of the data even if there are preset data values.  As an example, the preset data values could
	be used for ensuring the image is the correct size before real data values have been added
	to the model.
  */
class AMDataSourceImageDatawDefault : public AMDataSourceImageData
{
	Q_OBJECT

public:
	/// Constructor. \param dataSource is the data source we wish to encapsulate and \param defaultValue is the value that is ignored when computing the range.
 	virtual ~AMDataSourceImageDatawDefault();
	AMDataSourceImageDatawDefault(const AMDataSource *dataSource, double defaultValue, QObject *parent = 0);

	/// Returns the default value.
	double defaultValue() const { return defaultValue_; }
	/// Sets the default value to \param value.
	void setDefaultValue(double value) { defaultValue_ = value; MPlotAbstractImageData::emitDataChanged(); }

protected:
	/// Searches for minimum and maximum z value; stores in minMaxCache_.  Re-implemented from MPlotAbstractImageData to ignore default values when computing the minimum.
	virtual void minMaxSearch() const;


	/// The default value.
	qreal defaultValue_;
};

#endif // AMDATASOURCEIMAGEDATAWDEFAULT_H
