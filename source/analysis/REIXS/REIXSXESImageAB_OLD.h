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


#ifndef REIXSXESIMAGEAB_H
#define REIXSXESIMAGEAB_H

#include "analysis/AMStandardAnalysisBlock.h"
#include "util/AMDeferredFunctionCall.h"

#define HAVE_INLINE	// makes GSL operations faster by inlining.
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>

/// Interface to define categories of curve fitting
class REIXSFunctionFitter {
public:
	/// Function to fit a curve to a set of points, and return new points on the curve.
	virtual QVector<int> smooth(const QVector<int>& input, const QVector<double>& weights) = 0;
};

/// Class to implement quadratic curve fitting
class REIXSQuadraticFitter : public REIXSFunctionFitter {
public:
	REIXSQuadraticFitter();
	virtual ~REIXSQuadraticFitter();

	/// Function to fit a quadratic curve to a set of points, and return new points on the curve.
	virtual QVector<int> smooth(const QVector<int>& input, const QVector<double>& weights);

protected:
	// Structures used for shift curve fitting:
	//////////////////////////////
	double numRows_;

	gsl_multifit_linear_workspace* fitWorkSpace_; ///< Fit: GSL curve fitting context.
	gsl_matrix* fitXX_;		///< Fit: The matrix of predictor variables XX_ij = x_i^j;  In our case, x is the image row index (y!)
	gsl_vector* fitY_;		///< Fit: The vector of observations;  In our case, fitY_j is the shift value at j.
	gsl_vector* fitWeight_;	///< Fit: The weight of the observation; In our case 1 for sumRangeMin_ <= y <= sumRangeMax_; 0 otherwise.
	gsl_vector* fitC_;		///< Fit: The vector of coefficients; In our case the 3 quadratic fit constants.
	gsl_matrix* fitCov_;		///< Fit: covariance matrix
	double fitChisq_;		///< Fit: Chi-squared result

	/// Helper function to allocate the fitting structures for a given input size \c numRows. Used if the request to fit does not match the current allocated size.
	void allocateFittingStructures(int numRows);
	/// Helper function to free the fitting structures
	void releaseFittingStructures();
};

/// Class to implement cubic curve fitting
class REIXSCubicFitter : public REIXSFunctionFitter {
public:
	REIXSCubicFitter();
	virtual ~REIXSCubicFitter();

	/// Function to fit a Cubic curve to a set of points, and return new points on the curve.
	virtual QVector<int> smooth(const QVector<int>& input, const QVector<double>& weights);

protected:
	// Structures used for shift curve fitting:
	//////////////////////////////
	double numRows_;

	gsl_multifit_linear_workspace* fitWorkSpace_; ///< Fit: GSL curve fitting context.
	gsl_matrix* fitXX_;		///< Fit: The matrix of predictor variables XX_ij = x_i^j;
	gsl_vector* fitY_;		///< Fit: The vector of observations;
	gsl_vector* fitWeight_;	///< Fit: The weight of the observation;
	gsl_vector* fitC_;		///< Fit: The vector of coefficients; In our case the 4 Cubic fit constants.
	gsl_matrix* fitCov_;		///< Fit: covariance matrix
	double fitChisq_;		///< Fit: Chi-squared result

	/// Helper function to allocate the fitting structures for a given input size \c numRows. Used if the request to fit does not match the current allocated size.
	void allocateFittingStructures(int numRows);
	/// Helper function to free the fitting structures
	void releaseFittingStructures();
};

/// Class to implement quartic (4th-power) curve fitting
class REIXSQuarticFitter : public REIXSFunctionFitter {
public:
	REIXSQuarticFitter();
	virtual ~REIXSQuarticFitter();

	/// Function to fit a Quartic curve to a set of points, and return new points on the curve.
	virtual QVector<int> smooth(const QVector<int>& input, const QVector<double>& weights);

protected:
	// Structures used for shift curve fitting:
	//////////////////////////////
	double numRows_;

	gsl_multifit_linear_workspace* fitWorkSpace_; ///< Fit: GSL curve fitting context.
	gsl_matrix* fitXX_;		///< Fit: The matrix of predictor variables XX_ij = x_i^j;
	gsl_vector* fitY_;		///< Fit: The vector of observations;
	gsl_vector* fitWeight_;	///< Fit: The weight of the observation;
	gsl_vector* fitC_;		///< Fit: The vector of coefficients; In our case the 4 Quartic fit constants.
	gsl_matrix* fitCov_;		///< Fit: covariance matrix
	double fitChisq_;		///< Fit: Chi-squared result

	/// Helper function to allocate the fitting structures for a given input size \c numRows. Used if the request to fit does not match the current allocated size.
	void allocateFittingStructures(int numRows);
	/// Helper function to free the fitting structures
	void releaseFittingStructures();
};






/// Custom analysis block to shift and sum an XES detector image into a 1D spectrum, taking into account the possible curvature of the image. This version shifts each row by an arbitrary amount when summing columns.  It also has the capability to guess/determine the curvature using a correlation algorithm, where each row is correlated with the center row to determine the optimal shift.
class REIXSXESImageAB : public AMStandardAnalysisBlock
{
	Q_OBJECT
	Q_PROPERTY(int sumRangeMin READ sumRangeMin WRITE setSumRangeMin)
	Q_PROPERTY(int sumRangeMax READ sumRangeMax WRITE setSumRangeMax)
	Q_PROPERTY(AMIntList shiftValues READ shiftValues WRITE setShiftValues)
	Q_PROPERTY(int correlationCenterPixel READ correlationCenterPixel WRITE setCorrelationCenterPixel)
	Q_PROPERTY(int correlationHalfWidth READ correlationHalfWidth WRITE setCorrelationHalfWidth)
	Q_PROPERTY(int correlationSmoothing READ correlationSmoothing WRITE setCorrelationSmoothing)
	Q_PROPERTY(bool liveCorrelation READ liveCorrelation WRITE enableLiveCorrelation)
	Q_PROPERTY(double energyCalibrationOffset READ energyCalibrationOffset WRITE setEnergyCalibrationOffset)
	Q_PROPERTY(double tiltCalibrationOffset READ tiltCalibrationOffset WRITE setTiltCalibrationOffset)

	Q_CLASSINFO("AMDbObject_Attributes", "description=REIXS XES detector image analysis to 1D spectrum")
	Q_CLASSINFO("correlationSmoothing", "upgradeDefault=1")

public:
	/// Enum describing the options for smoothing the auto-correlated shift curve.
	enum ShiftCurveSmoothing { NoSmoothing, QuadraticSmoothing, CubicSmoothing, QuarticSmoothing };

	/// Constructor. \c outputName is the name() for the output data source.
	REIXSXESImageAB(const QString& outputName, QObject* parent = 0);
	/// This constructor is used to reload analysis blocks directly out of the database
	Q_INVOKABLE REIXSXESImageAB(AMDatabase* db, int id);

	/// Destructor
	virtual ~REIXSXESImageAB();

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
	/*! - there must be a single input source
  - the rank() of that input source must be 2 (two-dimensional)
  */
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

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

	// Analysis parameters
	///////////////////////////
	/// The minimum row to include in the sum
	int sumRangeMin() const { return sumRangeMin_; }
	/// The maximum row to include (inclusive) in the sum
	int sumRangeMax() const { return sumRangeMax_; }
	/// Returns the shift values used to offset each row before summing. This will have the same size as the height of the image.
	AMIntList shiftValues() const { return shiftValues_; }

	/// The central pixel value to use when running an auto-correlation routine
	int correlationCenterPixel() const { return correlationCenterPx_; }
	/// The full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	int correlationHalfWidth() const{ return correlationHalfWidth_; }
	/// The type of smoothing to apply to the shift curve generated using correlation. One of ShiftCurveSmoothing.
	int correlationSmoothing() const { return correlationSmoothing_; }
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
	void setSumRangeMin(int sumRangeMin);
	/// Set the maximum index in the region of interest. If the sum range is beyond the size of the summed axis, the output goes invalid. However, the value remains as set.
	void setSumRangeMax(int sumRangeMax);
	/// Sets the shift values for each row. This should have the same size as the height of the image.
	void setShiftValues(const AMIntList& shiftValues);

	/// Sets the central pixel value to use when running an auto-correlation routine
	void setCorrelationCenterPixel(int centerPx);
	/// Sets the full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	void setCorrelationHalfWidth(int width);
	/// Sets the smoothing used on the shift curve after correlation. \c type must be one of ShiftCurveSmoothing.
	void setCorrelationSmoothing(int type);
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

	/// Caches the shifted and summed values.  Access only if cacheInvalid_ is false.
	mutable QVector<double> cachedValues_;
	/// True if the cachedValues_ needs to be re-calculated.
	mutable bool cacheInvalid_;

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
	int sumRangeMin_, sumRangeMax_;

	/// The central pixel value to use when running an auto-correlation routine
	int correlationCenterPx_;
	/// The full-width of the region around correlationCenterPixel() to compute when running an auto-correlation routine.
	int correlationHalfWidth_;
	/// Describes the smoothing that should be applied to the shift curve resulting from the correlation routine.  One of ShiftCurveSmoothing.
	int correlationSmoothing_;
	/// True if the correlation routine should be run every time the data changes.
	bool liveCorrelation_;
	/// The shift values used to offset each row of the image when summing
	AMIntList shiftValues_;

	/// Calibration tweaks: an artificial (user-supplied) offset for the detector energy, in eV
	double energyCalibrationOffset_;
	/// Calibration tweaks: an artificial (user-supplied) offset for the detector tilt, in deg.
	double tiltCalibrationOffset_;


	/// An AMDeferredFunctionCall to call the correlation function a maximum of once per event loop.
	AMDeferredFunctionCall callCorrelation_;


	/// Smoothing object used for the shift curve after correlation
	REIXSFunctionFitter* curveSmoother_;

	// Helper Functions
	///////////////////////

	/// helper function to compute and fill cachedValues_.
	void computeCachedValues() const;
	/// Helper to compute energy axis scale, and fill cachedAxisValues_.
	void computeCachedAxisValues() const;

	/// Helper function to look at our overall situation and determine what the output state should be.
	void reviewState();

};

#endif // REIXSXESIMAGEAB_H
