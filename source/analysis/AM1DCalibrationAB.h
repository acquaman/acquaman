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


#ifndef AM1DCalibrationAB_H
#define AM1DCalibrationAB_H

#include "analysis/AMStandardAnalysisBlock.h"

/*!
  This analysis block accepts two or more 1D data sources and normalizes one using the other.
  It also provides the ability to scale and shift the data for calibration, and apply those
  shifts to other scans.
  The output data is the same size as the input sources.  There is an assumed order for the
  data sources and that is:

		1) Data
		2) Choice for normalization

The actual data source used by Calibration is determined by the analyzed name provided.
  */
class AM1DCalibrationAB : public AMStandardAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(QString dataName READ dataName WRITE setDataName)
	Q_PROPERTY(QString NormalizationName READ NormalizationName WRITE setNormalizationName)
	Q_PROPERTY(double energyCalibrationOffset READ energyCalibrationOffset WRITE setEnergyCalibrationOffset)
	Q_PROPERTY(double energyCalibrationScaling READ energyCalibrationScaling WRITE setEnergyCalibrationScaling)
	Q_PROPERTY(double energyCalibrationReference READ energyCalibrationReference WRITE setEnergyCalibrationReference)
	Q_PROPERTY(bool isTransmission READ isTransmission WRITE setIsTransmission)
	Q_PROPERTY(bool toEdgeJump READ toEdgeJump WRITE setToEdgeJump)
	Q_PROPERTY(int preEdgePoint READ preEdgePoint WRITE setPreEdgePoint)
	Q_PROPERTY(int postEdgePoint READ postEdgePoint WRITE setPostEdgePoint)


	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Calibration Block")

public:
	/// Constructor.
	Q_INVOKABLE AM1DCalibrationAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
	/// Destructor.
	virtual ~AM1DCalibrationAB(){}

	QString infoDescription() const { return QString(); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - two 1D data sources
		- For proper results, order must be the data source with real data and the second must be the normalizer.
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 1; }

	/// Set the analyzed data source name.
	void setDataName(const QString &name);
	/// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
	QString dataName() const { return dataName_; }
	/// Set the analyzed data source name.
	void setNormalizationName(const QString &name);
	/// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
	QString NormalizationName() const { return normalizationName_; }
	/// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
	bool canAnalyze() const { return canAnalyze_; }
	/// Returns whether the data source can be evaluated by passing in both names, \param dataName and \param NormalizationName.  Even though, the analysis block can be evaluated regardless of the name if there is only one data source, this will return true even if the name doesn't match.
	bool canAnalyze(const QString &dataName, const QString &NormalizationName) const;

	// Data Retrieval

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
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

	/// Returns the cached range of the data contained within the data source.  This is always valid because it is always recomputed when the data is recomputed.
	virtual AMRange dataRange() const { return cachedDataRange_; }

	/// Gets the energy axis offset for calibration
	double energyCalibrationOffset() const {return energyCalibrationOffset_;}
	/// Gets the energy axis scaling for calibration
	double energyCalibrationScaling() const {return energyCalibrationScaling_;}
	/// Gets the energy axis scaling reference point
	double energyCalibrationReference() const {return energyCalibrationReference_;}

	/// Returns the normalization type
	bool isTransmission() const {return isTransmission_;}
	/// Returns edge jump renormalization setting
	bool toEdgeJump() const {return toEdgeJump_;}
	/// Returns the pre edge offset reference point
	int preEdgePoint() const {return preEdgePoint_;}
	/// Returns the post edge scaling reference point
	int postEdgePoint() const {return postEdgePoint_;}


	//////////////////////////////////////////////

	/// Reimplement the create widget method.
	QWidget *createEditorWidget();

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

public slots:
	/// Sets the energy axis offset for calibration
	void setEnergyCalibrationOffset(double offset);
	/// Sets the energy axis scaling for calibration
	void setEnergyCalibrationScaling(double scaling);
	/// Sets the energy axis scaling reference point
	void setEnergyCalibrationReference(double reference);

	/// Sets the normalization type
	void setIsTransmission(bool isTransmission);
	/// Enables edge jump renormalization
	void setToEdgeJump(bool toEdgeJump);
	/// Sets the pre edge offset reference point
	void setPreEdgePoint(int preEdgePoint);
	/// Sets the post edge scaling reference point
	void setPostEdgePoint(int postEdgePoint);


protected:
	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();
	/// Helper method that sets the data_ and normalizer_ pointer to the correct data source based on the current state of analyzedName_.
	void setInputSources();

	/// Computes the cached data for access getters value() and values().
	void computeCachedValues() const;

	/// Pointer to the data source that will be analyzed.
	AMDataSource *data_;
	/// Pointer to the data source that is the normalizer.
	AMDataSource *normalizer_;

	/// Holds the energy axis offset for calibration
	double energyCalibrationOffset_;
	/// Holds the energy axis scaling for calibration
	double energyCalibrationScaling_;
	/// Holds the energy axis scaling reference point
	double energyCalibrationReference_;

	/// Determines of the data should be normalized as a transmission signal (Beer's law ln(Norm/Data)
	bool isTransmission_;
	/// Determines if the data should be renormalized to an absorption edge jump
	bool toEdgeJump_;
	/// Chooses which data value to use for pre-edge shift
	int preEdgePoint_;
	/// Chooses which data value to use for post-edge scaling
	int postEdgePoint_;


	/// The name of the data source that should be analyzed.
	QString dataName_;
	/// The name of the data source that be used for Normalization.
	QString normalizationName_;
	/// Flag holding whether or not the data source can be analyzed.
	bool canAnalyze_;

	/// Flag for knowing whether we need to compute the values.
	mutable bool cacheUpdateRequired_;
	/// A list of indices of values that need to be updated.  These are meant for when scans are running and data is coming in a pixel at a time.
	mutable QList<AMnDIndex> dirtyIndices_;
	/// The vector holding the data.
	mutable QVector<double> cachedData_;
	/// Holds the cached data range.
	mutable AMRange cachedDataRange_;
};

#endif // AM1DCalibrationAB_H
