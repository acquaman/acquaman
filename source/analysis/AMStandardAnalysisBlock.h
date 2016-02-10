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


#ifndef AMSTANDARDANALYSISBLOCK_H
#define AMSTANDARDANALYSISBLOCK_H

#include "dataman/AMAnalysisBlock.h"

/// This class provides many of the "boiler-plate" functions required for an AMAnalysisBlock, including managing the input data sources and representing the output AMDataSource axes.  Subclasses can make use of it by using the protected variables sources_ and axes_ to store their input data sources and output axes, respectively.
/*! To implement an analysis block using AMStandardAnalysisBlock, you are responsible for 4 things:

  - Handling input source changes: implement areInputDataSourcesAcceptable() and setInputDataSourcesImplementation().  Remember to store your input sources in sources_.
  - AMDataSource outputs: value() and axisValue().  Remember to store your axis information in axes_, and AMStandardAnalysisBlock will expose it for you.
  - Managing data source state: calling setState() when the state of your output changes.  This ensures that isValid() is correct whenever it should be.
  - Signalling changes to the nature of the output data source ( emitValuesChanged(), emitSizeChanged(), emitAxisInfoChanged(), emitInfoChanged() ).  You may need to listen to changes in the input data sources to do that.
  */
class AMStandardAnalysisBlock : public AMAnalysisBlock
{
	Q_OBJECT
public:
	/// Create a new AMAnalysisBlock. The block is also an AMDataSource of output data, \c outputName is the name for this AMDataSource.
	virtual ~AMStandardAnalysisBlock();
	AMStandardAnalysisBlock(const QString& outputName, QObject *parent = 0);

	// Access to input data sources
	//////////////////////////

	/// Access input data source by index.  If \c index >= inputDataCount(), returns null pointer.
	virtual AMDataSource* inputDataSourceAt(int index) const;

	/// Access the data sources which are currently providing input to this block
	virtual QList<AMDataSource*> inputDataSources() const { return sources_; }

	/// Number of current input sources
	virtual int inputDataSourceCount() const { return sources_.count(); }

	/// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name, if they do, this returns the first one.) Returns -1 if no input source found with this name.
	/*! This might be involve a slow lookup; users should not call repeatedly.*/
	virtual int indexOfInputSource(const QString& dataSourceName) const;

	/// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
	virtual int indexOfInputSource(const AMDataSource* source) const ;


	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const { return axes_; }

	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return axes_.count(); }

	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const;

	/// Returns the size along a single axis \c axisNumber. This should be fast.
	virtual int size(int axisId) const { return axes_.at(axisId).size; }

	/// Returns a bunch of information about a particular axis.
	virtual AMAxisInfo axisInfoAt(int axisId) const { return axes_.at(axisId); }

	/// Returns the id of an axis, by name. (By id, we mean the index of the axis. We called it id to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly.
	virtual int idOfAxis(const QString& axisName) const ;

protected:
	QList<AMDataSource*> sources_;
	QList<AMAxisInfo> axes_;
};

#endif // AMSTANDARDANALYSISBLOCK_H
