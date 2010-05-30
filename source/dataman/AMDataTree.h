#ifndef AMDATATREE_H
#define AMDATATREE_H

#include "AMErrorMonitor.h"

#include <QObject>
#include <QSharedData>
#include <QHash>
#include "AMBiHash.h"
#include <QStringList>

#include <QDebug>

#include "dataman/AMDataTreeColumn.h"
#include "AMObservable.h"

class AMDataTree;

/// The type of a vector of subtrees looks complicated. This is a simplification typedef.
typedef QVector<QSharedDataPointer<AMDataTree> > AMDataTreeSubtreeColumn;



/// This class is an attempt at supporting arbitrary-dimensionality data for AMScan objects, while maintaining simple (programmer-easy) and fast (high-performance) access to the data.
/*! Data must have a principal column (usually the "x" axis or main independent variable), and the values stored in this column must be true data values.
The dataset can have an arbitrary number of additional columns (ex: "tey_raw", "tfy_raw", and "sddSpectrums").  These columns can contain single datapoints (stored in \c y_) OR links to separate AMDataTables containing higher-dimensional data (stored in \c yD_).

For example, in the case of typical absorption scan data taken on the SGM beamline, "tey_raw" and "tfy_raw" would be columns of numbers stored in y_ and accessed as:
\code
AMDataTree myXasData;
/// The 6th element in the "tey" column:
printf( "(%d, %d)", myXasData.x(5), myXasData.value("tey", 5) );
\endcode

However, all of the datapoints in the "sddSpectrums" column would actually be links to other AMDataTables.  (In the case of the SDD (silicon drift detector), the linked tables would contain an x column (pixel value or energy value) and a y column of intensities.)
Externally, an actual data value would be accessed as:
\code
myXasData.deeper("sddSpectrums", 5)->value("sddIntensities", 512);
OR
(*myXasData.deeperColumn("sddSpectrums"))[5]->value("sddIntensities", 512);
\endcode

Internally...
\code
///    v----------------------------------------- A vector of AMDataTrees (number of elements = count()
///                 v---------------------------- The tree for the SDD spectrum at the 6th data point (corresponding to an energy of myXasData.x_[5])
///                            v----------------- It has one column, the sdd detector intensities (across 1024 energy pixels)
///                                         v----- The intensity of the middle pixel of the SDD spectrum. (Finally, a number!)
yD_("sddSpectrums", 5)->y_["sddIntensity"][512]
\endcode


The number of datapoints (whether actual values or AMDataTable links) in any column must be count().

\todo Ideas for cleaning API and increasing performance, while restricting non-const access to the internal vectors...
	- AMDataTreeIterator (directions: next, prev, nextColumn, prevColumn, deeper, higher.
			- setColumn(), hasValue() (actual or higher-D data?)
	- set a current column as active column; use for value() and setValue()
	- expression templates?

	AMDataTree has an observable() member that implements the AMObservable interface.  It will AMObservable::observable_.Emit() messages when a column changes, using \c code 3 and \c payload for the number of the y column (or -1 for the x column.)  (It puts "columnChanged" into the \c msg string.)  You can pickup these messages if you are an AMObserver and you call observable()->addObserver(yourself) with \c yourself.
*/

class AMDataTree : public QSharedData {


public:
	/// Constructor. Creates a tree with just a single (x) column. If it has explicit x values (specify hasXValues = true), space is allocated but no initialization is done; all the x values will be 0.
	explicit AMDataTree(unsigned count = 0, const QString& xColumnName = "x", bool hasXValues = false)
		: QSharedData(), x_(xColumnName)
	{

		count_ = count;
		hasXValues_ = hasXValues;
		if( hasXValues_ ) {
			x_.resize(count_);
		}
	}



/// Copy constructor used by QSharedDataPointer::clone() when duplicating trees. Although this is a "deep copy" at the AMDataTree level, the column vectors (QList, AMDataTreeColumn) in x_, and y_ also take advantage of implicit sharing, so they are copied quickly.
/*!	The "copy-on-write" method is used: both AMDataTrees will share the same data columns in memory until one of them needs to modify it.

Consider the scenario of myXASData with an x column (eV), one y column (tey), and higher-dimension column of sddSpectrum. The sddSpectrum  subtrees each have an x- and y-column.

\code
copyXASData = myXASData;
\endcode
(the column vectors eV and tey are shared implicitly.)
(the column of subtree pointers is shared implicitly, and with no changes. Both copyXASData and myXASData's sddSpectrum pointers are pointing to the same subtrees.)

\code
copyXASData.deeper("sddSpectrums",5)->setValue("y", 512, 49.3);
\endcode
(Now, we need to make a copy of the sddSpectrum subtree #5, with the new value 49.3 at index 512 in its 'y' column. This causes a deep copy of the 'y' column in the subtree; since the data is modified, it can't be implicitly shared anymore.)
(We also need to change the pointer in the sddSpectrums column (at index 5) to point to this new subtree. This causes a deep copy of the sddSpectrums column.)

*/
	AMDataTree(const AMDataTree &other)
		: QSharedData(other),
		count_(other.count_),
		x_(other.x_),
		hasXValues_(other.hasXValues_),
		y_(other.y_),
		yNames_(other.yNames_),
		yD_(other.yD_),	///< wrong?
		yDNames_(other.yDNames_),
		observable_(other.observable_)
	{

		/// \note: the deep copies required of the subtrees are handled by the QSharedDataPointers pointing to them inside yD_.)
	}

	/// Assignment operator
	AMDataTree& operator=(const AMDataTree& other) {
		  count_ = other.count_;
		  x_ = other.x_;
		  hasXValues_ = other.hasXValues_;
		  y_ = other.y_;
		  yNames_ = other.yNames_;
		  yDNames_ = other.yDNames_;
		  yD_ = other.yD_;
		  observable_ = other.observable_;
		  return *this;
	   }


	/// Destructor. No action required; the subtrees are automatically deleted when the QSharedDataPointers inside the column vectors are destroyed and they are not used by any other implicitly-shared objects.
	virtual ~AMDataTree() {
	}




	// Constant (access) member functions:
	////////////////////////////////////////

	/// The number of datapoints in each column
	unsigned count() const { return count_; }

	/// the number of (normal, data-valued columns), NOT including the x-column!
	unsigned numYColumns() const { return y_.count(); }

	/// the names of the (normal, data-valued columms), NOT including the x-column. (Use xName() to get the name of the x-column)
	QStringList yColumnNames() const {
		QStringList rv;
		for(int i=0; i<y_.count(); i++)
			rv << y_.at(i).name();
		return rv;
	}

	QStringList ySubtreeNames() const {
		QStringList rv;
		for(int i=0; i<yDNames_.count(); i++)
			rv << yDNames_.valueR(i);
		return rv;
	}


	/// Access the primary column values by index
	AMNumericType x(unsigned i) const {

		if(i >= count()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMDataTree: accessed value out of range. Returning an (incorrect) default value."));
			return AMNumericType();
		}

		if(hasXValues_)
			return x_.at(i);
		else
			return i;
	}

	/// The name of the primary column
	QString xName() const {
		return x_.name();
	}


	/// Access the value in any column by \c columnIndex and index \c i.
	AMNumericType value(unsigned columnIndex, unsigned i) const {
		if(i >= count()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMDataTree: accessed value out of range. Returning an (incorrect) default value."));
			return AMNumericType();
		}

		if((int)columnIndex < y_.count())
			return y_[columnIndex].at(i);

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: accessed non-existent data column. Returning an (incorrect) default value."));
		return AMNumericType();
	}

	/// Access the value in any column by \c columnName and index \c i.
	/*! Accessing by column name is slower than by column index, due to having to lookup the name.
	  */
	AMNumericType value(const QString& columnName, unsigned i) const {


		if(yNames_.contains(columnName))
			return value(yNames_.value(columnName), i);

		if(columnName == x_.name())
			return x(i);

//		if(yDNames_.contains(columnName)) {
		if(yDNames_.containsF(columnName)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, "AMDataTree: accessed multi-dimensional data as a single value. Returning an (incorrect) default value."));
			return AMNumericType();
		}

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: accessed non-existent data column. Returning an (incorrect) default value."));
		return AMNumericType();
	}

	/// Iterating through a column using value() is slower because of the column lookup at each step. This can be used to access a reference to an entire column of data (an AMDataTreeColumn).
	/*! \note If there is no column with index \c columnIndex, it logs an error message and returns the primary column.
		\todo Determine if an overloaded version allowing non-const access to the internal vector is wise. (ie: "If you want to change the data in the column, use the overloaded form of this function to return a non-const reference.")
	  */
	const AMDataTreeColumn& column(unsigned columnIndex) const {

		if((int)columnIndex < y_.count()) {
			return y_[columnIndex];
		}


		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: accessed a non-existent data column. I'm returning the primary column. This probably isn't what you want."));
		return x_;
	}


	/// Iterating through a column using value() is slower because of the column lookup at each step. This can be used to access a reference to an entire column of data (an AMDataTreeColumn).
	/*! \note If the name of the column \c columnName does not exist, it logs an error message and returns the primary column.
		\todo Determine if an overloaded version allowing non-const access to the internal vector is wise. (ie: "If you want to change the data in the column, use the overloaded form of this function to return a non-const reference.")
	  */
	const AMDataTreeColumn& column(const QString& columnName) const {

		if(yNames_.contains(columnName)) {
			return column(yNames_.value(columnName));
		}

		if(columnName == x_.name())
			return x_;

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: accessed a non-existent data column. I'm returning the primary column. This probably isn't what you want."));
		return x_;
	}





	const AMDataTree* prototype(unsigned columnIndex) const {
		return prototypes_.at(columnIndex);
	}

	const AMDataTree* prototype(const QString& columnName) const {
		if(yDNames_.containsF(columnName))
			return prototypes_.at(yDNames_.valueF(columnName));
		return NULL;
	}

	/// Access a pointer to a higher-dimensional data tree. Returns 0 if column not found or index out of range.
	/*! The tree returned is a read-only pointer.  The overloaded version of this function returns a non-const pointer you can use for modifying trees. However, it could force a deep copy of the tree, it it's shared.
		*/
	const AMDataTree* deeper(unsigned columnIndex, unsigned i) const {

		if(i >= count()) {
			return 0;
		}


		if((int)columnIndex < yD_.count()) {
			return yD_[columnIndex].at(i).data();
		}

		return 0;
	}

	/// Access a pointer to a higher-dimensional data tree. Returns 0 if column not found or index out of range.
	/*! The tree returned is a read-only pointer.  The overloaded version of this function returns a non-const pointer you can use for modifying trees. However, it could force a deep copy of the tree, it it's shared.
		*/
	const AMDataTree* deeper(const QString& columnName, unsigned i) const {

//		if(yDNames_.contains(columnName)) {
		if(yDNames_.containsF(columnName)) {
			return deeper(yDNames_.valueF(columnName), i);
//			return deeper(yDNames_.value(columnName), i);
		}

		return 0;
	}


	/// Overloaded version allowing non-const access to the subtree
	AMDataTree* deeper(unsigned columnIndex, unsigned i) {

		if(i >= count()) {
			return 0;
		}

		if((int)columnIndex < yD_.count()) {
			return yD_[columnIndex][i].data(); // For the modifiable version, the non-const call to data() ends up calling detach(), creating a deep copy of the tree.
		}

		return 0;
	}

	/// Overloaded version allowing non-const access to the subtree
	AMDataTree* deeper(const QString& columnName, unsigned i) {


//		if(yDNames_.contains(columnName)) {
		if(yDNames_.containsF(columnName)) {
//			return deeper(yDNames_.value(columnName), i); // For the modifiable version, the non-const call to data() ends up calling detach(), creating a deep copy of the tree.
			return deeper(yDNames_.valueF(columnName), i); // For the modifiable version, the non-const call to data() ends up calling detach(), creating a deep copy of the tree.
		}

		return 0;
	}


	/// Returns an entire column of subtrees
	const AMDataTreeSubtreeColumn* deeperColumn(unsigned columnIndex) const {

		if((int)columnIndex < yD_.count()) {
			return &(yD_[columnIndex]);
		}

		return 0;
	}

	/// Returns an entire column of subtrees
	const AMDataTreeSubtreeColumn* deeperColumn(const QString& columnName) const {

//		if(yDNames_.contains(columnName)) {
		if(yDNames_.containsF(columnName)) {
//			return deeperColumn(yDNames_.value(columnName));
			return deeperColumn(yDNames_.valueF(columnName));
		}

		return 0;
	}

	AMDataTreeSubtreeColumn* deeperColumn(unsigned columnIndex) {

		if((int)columnIndex < yD_.count()) {
			return &(yD_[columnIndex]);
		}

		return 0;
	}

	AMDataTreeSubtreeColumn* deeperColumn(const QString& columnName) {

//		if(yDNames_.contains(columnName)) {
		if(yDNames_.containsF(columnName)) {
//			return deeperColumn(yDNames_.value(columnName));
			return deeperColumn(yDNames_.valueF(columnName));
		}

		return 0;
	}


	AMObservable* observable() const { return &observable_; }

	// Non-constant (modifier) member functions:
	////////////////////////////////////////

	/// set x-column data element
	bool setX(unsigned i, AMNumericType newValue) {
		if(hasXValues_ && i < count()) {
			x_[i] = newValue;
			observable_.Emit(3, "columnChanged", -1);
			return true;
		}
		else {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMDataTree: attempted modifying a value out of range. Not successful(X)."));
			return false;
		}
	}


	/// set an arbitrary data value:
	bool setValue(unsigned columnIndex, unsigned i, AMNumericType newValue) {

		if(i >= count()) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -1, "AMDataTree: attempted modifying a value out of range. Not successful(VI1)."));
			return false;
		}

		if((int)columnIndex < y_.count()) {
			y_[columnIndex][i] = newValue;
			observable_.Emit(3, "columnChanged", columnIndex);
			return true;
		}


		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: attempted modifying non-existent data column. Not successful(VI2)."));
		return false;

	}

	/// set an arbitrary data value:
	bool setValue(const QString& columnName, unsigned i, AMNumericType newValue) {


		if(yNames_.contains(columnName)) {
			return setValue(yNames_.value(columnName), i, newValue);
		}

		if(columnName == x_.name()) {
			return setX(i, newValue);
		}

//		if(yDNames_.contains(columnName)) {
		if(yDNames_.containsF(columnName)) {
			AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, "AMDataTree: attempted modifying multi-dimensional data as a single value. Not successful(VS1)."));
			return false;
		}

		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: attempted modifying non-existent data column. Not successful(VS2)."));
		return false;

	}

	/// Create a new column of actual data. The column will have the right size (count()), but uninitialized data.  Returns the columnIndex of the new column.
	unsigned createColumn(const QString& newColumnName, const QString& description = QString(), const QString& units = QString()) {
		y_.append( AMDataTreeColumn(newColumnName, description, units, count()) );
		yNames_[newColumnName] = y_.count() - 1;
		return y_.count() - 1;
	}

	/// Create a new subtree column. All of the subtrees will be created with the given count, and implicit copies of \c treeFiller will be used as a "prototype" to initialize the trees in the column. (as long as count() isn't 0).  It's okay to use 'new AMDataTree(...)' without worrying about freeing the memory; once added to the column, shared reference counting will start from 1, and the prototype subtree will be deleted when no longer required.
	/*! \note This will allocate memory for the new trees. However, reference counting is implemented so that they will be deleted automatically when the column vector (and the QSharedDataPointers within it) are deleted.
	  */
	void createSubtreeColumn(const QString& newSubtreeColumnName, AMDataTree* treeFiller) {

		/// create the vector of subtree pointers:
		AMDataTreeSubtreeColumn newVect(count());

		/// Old way: create the trees, and store the shared-pointer references.
		/*
		for(unsigned i=0; i<count(); i++) {
			newVect[i] = new AMDataTree(subTreeCount, xColumnName, hasXValues);
		}*/
		/// New way: create one tree to use for all datapoints in the column, which will be implicitly shared until modified.
//		if(count() == 0){
			prototypes_.append(treeFiller);
//			qDebug() << "Master tree has no items, need to delay prototype";
//		}
//		else{
//			qDebug() << "Master tree has items, prototyping now";
			if(count() > 0){
				QSharedDataPointer<AMDataTree> newTree(treeFiller);
				for(unsigned i=0; i<count(); i++)
					newVect[i] = newTree;
		}
//		}
		/// insert the new vector under this column name.
		yD_.append(newVect);
//		yDNames_[newSubtreeColumnName] = yD_.count() - 1;
		yDNames_.set(newSubtreeColumnName, yD_.count() - 1);

	}


	/// Appending a new datapoint to each column is a common operation (for ex: a mono move and absorption measurement has just completed in an XAS scan.)
	/*! This function increases the count() by 1 and appends a datapoint to each column. It sets the value of the primary column datapoint to \c newValue. (If hasXValues() is false, the \c newValue is ignored.)

	Use setValue() or setLastValue() to fill in the remaining columns, otherwise they will contain default values.

	If there are columns of subtrees, the default value of the new subtree is a copy of the most recent subtree. (At least until you change it.) (Exception: if count() is 0, there are no trees to emulate, so a default subtree is created.)

	\test
	*/
	void append(const AMNumericType& newValue = 0) {

		// count_ is now one ahead of every column's size.
		count_++;

		// append to primary column. (As long as we hasXValues_.)
		if(hasXValues_)
			x_ << newValue;

		// append default values to all the other data columns:
		for(int i=0; i<y_.count(); i++)
			y_[i] << AMNumericType();

		// append subtree copies (or, if this is the first, a default subtree) to all the subtree columns.
		for(int i=0; i<yD_.count(); i++) {
			// are we the first? append default subtree
			if(count_ == 1){
				if(prototypes_.at(i) == NULL){
					qDebug() << "Prototype is NULL, prototype default tree";
					yD_[i] << QSharedDataPointer<AMDataTree>(new AMDataTree());
				}
				else{
					qDebug() << "Prototype exists, using " << i;
					yD_[i] << QSharedDataPointer<AMDataTree>(prototypes_.at(i));
				}
			}
			// otherwise append a copy of the latest tree.
			else
				yD_[i] << yD_[i][count_ - 2];
		}

		observable_.Emit(3, "columnChanged", -1);

	}

	/// This a convenience function, equivalent to setValue(colIndex, count()-1, newValue). It's useful when filling extra columns after append(). \test
	void setLastValue(unsigned colIndex, const AMNumericType& newValue = 0) {

		setValue(colIndex, count_ - 1, newValue);

	}

	/// This a convenience function, equivalent to setValue(colName, count()-1, newValue). It's useful when filling extra columns after append(). \test
	void setLastValue(const QString& colName, const AMNumericType& newValue = 0) {

		setValue(colName, count_ - 1, newValue);

	}

	/// You cannot set column names, because the column names are the method used to access columns. However, the primary axis channel name (set in constructor) can be changed.
	/*! \note Beware, this might mess up your buddies who are using setValue("xOld", 3, 1.234).
	  */
	void setXName(const QString& newColName) {
		x_.setName(newColName);
	}

	/// clears all of the data in the tree. The count() will become 0, but all of the columns will remain.
	void clear() {
		x_.clear();

		// iterate through all of the columns and empty them
		for(int i=0; i<y_.count(); i++) {
			y_[i].clear();
		}

		// iterate through all of the subtree columns and empty them.
		for(int i=0; i<yD_.count(); i++)
			yD_[i].clear();

		for(int i=-1; i<y_.count(); i++)
			observable_.Emit(3, "columnChanged", i);

	}

	/// clears all of the data in the tree, and removes all of the columns.
	void removeAll() {

		x_.clear();
		y_.clear();
		yD_.clear();
		count_ = 0;
	}




public slots:

protected:

	// Internal data:
	//////////////////////////////
	/// The number of datapoints (in any column) of this dataset
	int count_;

	/// The x values: values and metadata for the principal column of the dataset. Must contain bare numbers; no subtrees allowed here.
	AMDataTreeColumn x_;
	/// Sometimes (for example: evenly spaced data, or pixel data) the index is all we need for an x-axis; there's no need to store actual x-axis values in memory. In this case, hasXValues_ = false.
	bool hasXValues_;

	/// A <columnName, columnData> map of the dependent data columns. The keys are the column names (ex: "tey", "tfy", "etc"). The values are a vector containing count() datapoints. These are single data points; higher-dimensionality data is stored in yD_.
	/*! \note Implementation details: These need to be mutable so that we can access the non-const QHash::operator[] overload which returns a reference to the value, from our const member functions. (Unfortunately, because it creates a new value for  keys which don't exist, it's not const.)
	  */
	QList<AMDataTreeColumn> y_;
	/// used to lookup column indices based on column names.
	QHash<QString, int> yNames_;

	/// Higher-dimensional data is represented by pointing to another AMDataTree for each datapoint.
	QList<AMDataTreeSubtreeColumn> yD_;

	QList<AMDataTree*> prototypes_;

	/// used to lookup subtree column indices based on column names
//	QHash<QString, int> yDNames_;
	AMBiHash<QString, int> yDNames_;

	mutable AMObservable observable_;


};



#endif // AMDATATREE_H
