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


#ifndef REIXSXESImageInterpolationAB_H
#define REIXSXESImageInterpolationAB_H

#include "analysis/AMStandardAnalysisBlock.h"
#include "util/AMDeferredFunctionCall.h"
#include "REIXSXESImageAB.h"

#define HAVE_INLINE	// makes GSL operations faster by inlining.
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>

#include <QPair>

#include <QDebug>



/// Custom analysis block to shift and sum an XES detector image into a 1D spectrum, taking into account the possible curvature of the image. This version shifts each row by an arbitrary amount when summing columns.  It also has the capability to guess/determine the curvature using a correlation algorithm, where each row is correlated with the center row to determine the optimal shift.
class REIXSXESImageInterpolationAB : public AMStandardAnalysisBlock
{
	Q_OBJECT
	Q_PROPERTY(int sumRangeMinY READ sumRangeMinY WRITE setSumRangeMinY)
	Q_PROPERTY(int sumRangeMaxY READ sumRangeMaxY WRITE setSumRangeMaxY)
	Q_PROPERTY(int sumRangeMinX READ sumRangeMinX WRITE setSumRangeMinX)
	Q_PROPERTY(int sumRangeMaxX READ sumRangeMaxX WRITE setSumRangeMaxX)
	Q_PROPERTY(double rangeRound READ rangeRound WRITE setRangeRound)

	Q_PROPERTY(AMIntList shiftValues1 READ shiftValues1 WRITE setShiftValues1)
	Q_PROPERTY(AMIntList shiftValues2 READ shiftValues2 WRITE setShiftValues2)

	Q_PROPERTY(bool curve2Disabled READ curve2Disabled WRITE setCurve2Disabled)

	Q_PROPERTY(int correlation1CenterPixel READ correlation1CenterPixel WRITE setCorrelation1CenterPixel)
	Q_PROPERTY(int correlation1HalfWidth READ correlation1HalfWidth WRITE setCorrelation1HalfWidth)
	Q_PROPERTY(int correlation2CenterPixel READ correlation2CenterPixel WRITE setCorrelation2CenterPixel)
	Q_PROPERTY(int correlation2HalfWidth READ correlation2HalfWidth WRITE setCorrelation2HalfWidth)

	Q_PROPERTY(int correlation1SmoothingType READ correlation1SmoothingType WRITE setCorrelation1SmoothingType)
	Q_PROPERTY(int correlation1SmoothingMode READ correlation1SmoothingMode WRITE setCorrelation1SmoothingMode)
	Q_PROPERTY(int correlation2SmoothingType READ correlation2SmoothingType WRITE setCorrelation2SmoothingType)
	Q_PROPERTY(int correlation2SmoothingMode READ correlation2SmoothingMode WRITE setCorrelation2SmoothingMode)

	Q_PROPERTY(int binningLevel READ binningLevel WRITE setBinningLevel)

	Q_PROPERTY(bool liveCorrelation READ liveCorrelation WRITE enableLiveCorrelation)
	Q_PROPERTY(double energyCalibrationOffset READ energyCalibrationOffset WRITE setEnergyCalibrationOffset)
	Q_PROPERTY(double tiltCalibrationOffset READ tiltCalibrationOffset WRITE setTiltCalibrationOffset)

	Q_CLASSINFO("AMDbObject_Attributes", "description=REIXS XES detector image analysis to 1D spectrum")
	Q_CLASSINFO("correlationSmoothing", "upgradeDefault=1")
	Q_CLASSINFO("binningLevel", "upgradeDefault=2")

public:
	/// Enum describing the options for smoothing the auto-correlated shift curve.
	enum ShiftCurveSmoothing { None, Poly, Median, Average};

	/// Constructor. \c outputName is the name() for the output data source.
	Q_INVOKABLE REIXSXESImageInterpolationAB(const QString& outputName = "InvalidInput", QObject* parent = 0);

	/// Destructor
	virtual ~REIXSXESImageInterpolationAB();

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
  - the rank() of that input source must be 2 (two-dimensional)
  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 2; }

protected:
	/// Set the data source inputs.
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

public:
	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing the expressions.
	virtual QWidget* createEditorWidget();


	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or (if AM_ENABLE_BOUNDS_CHECKING is defined, any are out of range), or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;
	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
	/*! The base-class implementation simply calls value() repeatedly, so you should absolutely re-implement this for better performance.

	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

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

	// Analysis parameters
	///////////////////////////
	/// The minimum row to include in the sum
	int sumRangeMinY() const { return sumRangeMinY_; }
	/// The maximum row to include (inclusive) in the sum
	int sumRangeMaxY() const { return sumRangeMaxY_; }
	/// The minimum column to include in the sum
	int sumRangeMinX() const { return sumRangeMinX_; }
	/// The maximum column to include (inclusive) in the sum
	int sumRangeMaxX() const { return sumRangeMaxX_; }

	//The "roundness" of the mask (0 for rectangular, 1 for ellipse)
	double rangeRound() const { return rangeRound_; }

	/// Returns the last comupted shift values that are available for assignment to shiftValues1 or shiftValues2
	AMIntList shiftValues1() const { return shiftValues1_; }
	/// Returns the second shift values used interpolated offset of each row before summing. This will have the same size as the height of the image.
	AMIntList shiftValues2() const { return shiftValues2_; }
	/// Returns the last calculate shift values for a given column
	AMDoubleList shiftValuesAt(int i);

	/// The central pixel value to use when running an auto-correlation routine
	int curve2Disabled() const { return curve2Disabled_; }
	/// The central pixel value to use when running an auto-correlation routine
	int correlation1CenterPixel() const { return correlation1CenterPx_; }
	/// The central pixel value to use when running an auto-correlation routine
	int correlation2CenterPixel() const { return correlation2CenterPx_; }
	/// The central pixel value to use when running an auto-correlation routine
	int binningLevel() const { return binningLevel_; }

	/// The full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	int correlation1HalfWidth() const{ return correlation1HalfWidth_; }
	/// The type of smoothing to apply to the shift curve generated using correlation. One of ShiftCurveSmoothing.
	/// The full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	int correlation2HalfWidth() const{ return correlation2HalfWidth_; }
	/// The type of smoothing to apply to the shift curve generated using correlation. One of ShiftCurveSmoothing.

	QPair<int,int> correlation1Smoothing() const {return correlation1Smoothing_;}
	int correlation1SmoothingType() const { return correlation1Smoothing_.first; }
	int correlation1SmoothingMode() const { return correlation1Smoothing_.second;}
	QPair<int,int> correlation2Smoothing() const {return correlation2Smoothing_;}
	int correlation2SmoothingType() const { return correlation2Smoothing_.first; }
	int correlation2SmoothingMode() const { return correlation2Smoothing_.second;}

	/// True if the correlation routine should be run every time the data changes.
	bool liveCorrelation() const { return liveCorrelation_; }


	/// Calibration tweaks: an artificial (user-supplied) offset for the detector energy, in eV
	double energyCalibrationOffset() const { return energyCalibrationOffset_; }
	/// Calibration tweaks: an artificial (user-supplied) offset for the detector tilt, in deg.
	double tiltCalibrationOffset() const { return tiltCalibrationOffset_; }


	// Setting parameters
	///////////////////////////


public slots:

	/// Set the minimum index in the region of interest.  If the sum range is beyond the size of the summed axis, the output goes invalid. The value remains as set, however.
	void setSumRangeMinY(int sumRangeMinY);
	/// Set the maximum index in the region of interest. If the sum range is beyond the size of the summed axis, the output goes invalid. However, the value remains as set.
	void setSumRangeMaxY(int sumRangeMaxY);
	/// Set the minimum index in the region of interest.  If the sum range is beyond the size of the summed axis, the output goes invalid. The value remains as set, however.
	void setSumRangeMinX(int sumRangeMinX);
	/// Set the maximum index in the region of interest. If the sum range is beyond the size of the summed axis, the output goes invalid. However, the value remains as set.
	void setSumRangeMaxX(int sumRangeMaxX);
	/// Sets the comupted shift values
	void setShiftValues1(const AMIntList& shiftValues1);
	/// Sets the second shift values for each row. This should have the same size as the height of the image.
	void setShiftValues2(const AMIntList& shiftValues1);

	void setRangeRound(double rangeRound);

	/// enables or disables curve interpolation
	void setCurve2Disabled(bool disabled);
	/// Sets the central pixel value to use when running an auto-correlation routine
	void setCorrelation1CenterPixel(int centerPx);
	/// Sets the full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	void setCorrelation1HalfWidth(int width);
	/// Sets the smoothing used on the shift curve after correlation. \c type must be one of ShiftCurveSmoothing.
	void setCorrelation1Smoothing(QPair<int,int> cSmooth);

	/// Sets the central pixel value to use when running an auto-correlation routine
	void setCorrelation2CenterPixel(int centerPx);
	/// Sets the full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	void setCorrelation2HalfWidth(int width);
	/// Sets the smoothing used on the shift curve after correlation. \c type must be one of ShiftCurveSmoothing.
	void setCorrelation2Smoothing(QPair<int,int> cSmooth);

	/// Handles changing the binning level and trigging a recompute
	void setBinningLevel(int binningLevel);

	void setCorrelation1SmoothingType(int type);
	void setCorrelation1SmoothingMode(int mode);

	void setCorrelation2SmoothingType(int type);
	void setCorrelation2SmoothingMode(int mode);


	/// Enable (or disable) automatically running the correlation routine every time the data changes
	void enableLiveCorrelation(bool enabled);


	/// Set an artificial (user-supplied) calibration offset for the detector energy, in eV
	void setEnergyCalibrationOffset(double energyCalibrationOffset);
	/// Set an artificial (user-supplied) calibration offset for the detector tilt, in deg.
	void setTiltCalibrationOffset(double tiltCalibrationOffset);




signals:

	// additional signals:

	/// Emitted when the shift values are changed. (Retrieve the values with shiftValues() if necessary.)
	void shiftValuesChanged();


protected slots:
	/// Connected to be called when the values of the input data source change
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of the input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();


	/// Call to run the correlation routine to compute the shift values
	void correlateNow();


protected:
	// Helper Functions
	///////////////////////

	/// helper function to compute and fill cachedValues_.
	void computeCachedValues() const;
	/// Helper fucntion to compute interpolated shift map
	void computeShiftMap(int iSize, int jSize, double *shiftValues) const;
	/// Helper to compute energy axis scale, and fill cachedAxisValues_.
	void computeCachedAxisValues() const;
	///helper function to determine if a pixel is with the mask area (uses _rangeRound)
	bool isWithinMaskEllipse(double xVal, double yVal, double width, double height) const;

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

	// Member variables
	/// Caches the shifted and summed values.  Access only if cacheInvalid_ is false.
	mutable QVector<double> cachedData_;
	/// True if the cachedValues_ needs to be re-calculated.
	mutable bool cacheUpdateRequired_;
	/// Holds the cached data range.
	mutable AMRange cachedDataRange_;

	/// Cache of axis values.
	mutable QVector<double> cachedAxisValues_;
	/// True if the cachedAxisValues_ needs to be re-calculated.
	mutable bool axisValueCacheInvalid_;
	/// True if the axis values could not be calculated [no access to the scan, required positions, etc.]
	mutable bool axisValuesInvalid_;

	AMDataSource* inputSource_;	// our single input source, or 0 if we don't have one.

	// Parameters:
	/////////////////////

	/// The upper and lower row limits to include in the sum
	int sumRangeMinY_, sumRangeMaxY_;
	int sumRangeMinX_, sumRangeMaxX_; //left and right boundaries, so we can have full control over ellipse
	double rangeRound_; //0 to 1, 0 for rectangular mask, 1 for elliptical

	/// bool to disable interpolation and use a single curve shift
	bool curve2Disabled_;
	/// The central pixel value to use when running an auto-correlation routine
	int correlation1CenterPx_;
	/// The full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	int correlation1HalfWidth_;
	/// Describes the smoothing that should be applied to the shift curve resulting from the correlation routine.  One of ShiftCurveSmoothing.
	QPair<int, int> correlation1Smoothing_;

	/// The central pixel value to use when running an auto-correlation routine
	int correlation2CenterPx_;
	/// The full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	int correlation2HalfWidth_;
	/// Describes the smoothing that should be applied to the shift curve resulting from the correlation routine.  One of ShiftCurveSmoothing.
	QPair<int, int> correlation2Smoothing_;

	/// True if the correlation routine should be run every time the data changes.
	bool liveCorrelation_;
	/// The first set of shift values used to offset each row of the image when summing
	AMIntList shiftValues1_;
	/// The second set of shift values used to offset each row of the image when summing
	AMIntList shiftValues2_;
	/// The level of interpolation, hard-coded for now
	int interpolationLevel_;
	/// The level of binning
	int binningLevel_;
	/// The last computed shift value map
	mutable QVector<double> lastShiftValueMap_;

	/// Calibration tweaks: an artificial (user-supplied) offset for the detector energy, in eV
	double energyCalibrationOffset_;
	/// Calibration tweaks: an artificial (user-supplied) offset for the detector tilt, in deg.
	double tiltCalibrationOffset_;


	/// An AMDeferredFunctionCall to call the correlation function a maximum of once per event loop.
	AMDeferredFunctionCall callCorrelation_;


	/// Smoothing object used for the shift curve after correlation
	REIXSFunctionFitter* curve1Smoother_, * curve2Smoother_;

};

#endif // REIXSXESImageInterpolationAB_H
