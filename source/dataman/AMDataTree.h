#ifndef AMDATATREE_H
#define AMDATATREE_H

#include "AMErrorMonitor.h"

/// This is a lightweight template class that can contain either a single data value (of type T), or point to an entire subtree of higher-dimensional data.
/*! Two constructors are provided: one that creates the single-value representation, and the other that represents a linke to another tree.
	
	isTree() can be used to determine whether there is higher-dimensional data.
	
	value() returns the single datapoint. If the instance is actually representing higher-dimensional data, it returns a default-constructed value and reports an Alert message.
	*/

/// Forward declaration of AMDataTree<T>
template <class T>
class AMDataTree;

template <class T>
		class AMDataPoint {
	
public:
	/// Creates a datapoint representing a single value
	AMDataPoint(const T& sourceValue) {
		tree_ = 0;
		value_ = sourceValue;
	}
	
	/// Creates a datapoint that is actually a link to an AMDataTree of higher-dimensional data.
	AMDataPoint(AMDataTree<T>* sourceTree) {
		tree_ = sourceTree;
	}
	
	/// Indicates if this instance represents a link to an AMDataTree<T> of higher-dimensional data.
	bool isTree() const {
		return (tree_ != 0);
	}
	
	/// Returns the single datapoint. If the instance is actually representing higher-dimensional data, it returns a default-constructed value and reports an Alert message.
	T value() const { 
		// if we represent a sub-tree of data, 
		if(isTree())
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "Multi-dimensional data accessed as a single data point. Returning a default value. It's probably incorrect."));
	
		return value_; 
	}
	
	/// Returns the subtree
	AMDataTree<T>* tree() const { return tree_; }
	
protected:
	T value_;
	AMDataTree<T>* tree_;
	
};


/// This class is an attempt at supporting arbitrary-dimensionality data for AMScan objects, while maintaining simplicity.
/*! Data must have a principal column (usually the "x" axis or main independent variable), and the values stored in this column must be true data values (of type T).
	The dataset can have an arbitrary number of additional columns (ex: "tey_raw", "tfy_raw", and "sddSpectrums").  These columns can contain single datapoints OR links to separate AMDataTables containing higher-dimensional data.

	For example, in the case of typical absorption scan data taken on the SGM beamline, "tey_raw" and "tfy_raw" would be columns of numbers, but all of the datapoints in the "sddSpectrums" column would actually be links to other AMDataTables.  (In the case of the SDD (silicon drift detector), the linked tables would contain an x column (pixel value or energy value) and a y column of intensities.)

	The number of datapoints (whether actual values or AMDataTable links) in any column must be count().

	One problem: while this is a template class, it's currently impossible to have a column of floats and a column of ints in the same parent AMDataTree.  Unfortunately all data types must be the same.
	*/

template <class T>
		class AMDataTree {

public:
    explicit AMDataTree();
	
	unsigned count() const { return count_; }

signals:

public slots:

protected:
	/// The number of datapoints (in any column) of this dataset
	int count_;
	
	/// The x values: values in the principal column of the dataset. Must be bare numbers (type T).
	QVector<T> x_;
	/// The name of the principal column (the independent variable that you scanned). ex: "eV" or "temp"
	QString xName_;
	
	/// The y values: values in the additional columns of the dataset. The first list is over different columns ("tey", "tfy", "etc"). Then, each vector contains count() AMDataPoints, which can either be a single number (type T) or a whole other AMDataTable.
	QList<QVector<AMDataPoint<T> > > ys_;
	/// The names of the additional columns
	QList<QString> yNames_;
	
};

#endif // AMDATATREE_H
