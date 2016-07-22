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


#ifndef AM1DKSPACECALCULATORAB_H
#define AM1DKSPACECALCULATORAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/// This analysis block converts an energy into a k-space value.
/*!
	This class is meant to be used inside of EXAFS scans - which scan over
	energy but often like to be represented by their k-space equivalent.
	To this end, there are two parameters that are very important.

		1) The only data source should be the energy PV.  This is the one
		where the values() are the feedback position and the axisValues()
		are the setpoint positions.

		2) The edge energy.  This is necessary for any k-space calculation.
  */
class AM1DKSpaceCalculatorAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(double edgeEnergy READ edgeEnergy WRITE setEdgeEnergy)

	Q_CLASSINFO("AMDbObject_Attributes", "description=k-space calculator block")

public:
	/// Constructor.
	Q_INVOKABLE AM1DKSpaceCalculatorAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	~AM1DKSpaceCalculatorAB(){}

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - one 1D data source
		- For proper results, order must be the data source with real data and the second must be the normalizer.
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 1; }

	/// Sets the edge energy.
	void setEdgeEnergy(double energy);
	/// Returns the current edge edge energy.
	double edgeEnergy() const { return edgeEnergy_; }

	// Data Retrieval

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension.
	/*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

	\code
	int outputSize = indexStart.totalPointsTo(indexEnd);
	\endcode
	*/
	virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();

protected:
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);
	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

	/// Pointer to the data source that will be analyzed.
	AMDataSource *data_;
	/// The edge energy used for computing the k-space value.
	double edgeEnergy_;
};

#endif // AM1DKSPACECALCULATORAB_H
