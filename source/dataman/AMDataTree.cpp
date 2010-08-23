#include "AMDataTree.h"

/// Constructor. Creates a tree with just a single (x) column. If it has explicit x values (specify hasXValues = true), space is allocated but no initialization is done; all the x values will be 0.
AMDataTree::AMDataTree(unsigned count, const QString& xColumnName, bool hasXValues)
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
AMDataTree::AMDataTree(const AMDataTree &other)
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
AMDataTree& AMDataTree::operator=(const AMDataTree& other) {
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






// Constant (access) member functions:
////////////////////////////////////////





/// the names of the (normal, data-valued columms), NOT including the x-column. (Use xName() to get the name of the x-column)
QStringList AMDataTree::yColumnNames() const {
	QStringList rv;
	for(int i=0; i<y_.count(); i++)
		rv << y_.at(i).name();
	return rv;
}

QStringList AMDataTree::ySubtreeNames() const {
	QStringList rv;
	for(unsigned i=0; i<yDNames_.count(); i++)
		rv << yDNames_.valueR(i);
	return rv;
}


/// Access the primary column values by index
AMNumericType AMDataTree::x(unsigned i) const {

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
QString AMDataTree::xName() const {
	return x_.name();
}


/// Access the value in any column by \c columnIndex and index \c i.
AMNumericType AMDataTree::value(unsigned columnIndex, unsigned i) const {
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
AMNumericType AMDataTree::value(const QString& columnName, unsigned i) const {


	if(yNames_.contains(columnName))
		return value(yNames_.value(columnName), i);

	if(columnName == x_.name())
		return x(i);

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
const AMDataTreeColumn& AMDataTree::column(unsigned columnIndex) const {

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
const AMDataTreeColumn& AMDataTree::column(const QString& columnName) const {

	if(yNames_.contains(columnName)) {
		return column(yNames_.value(columnName));
	}

	if(columnName == x_.name())
		return x_;

	AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, "AMDataTree: accessed a non-existent data column. I'm returning the primary column. This probably isn't what you want."));
	return x_;
}





const AMDataTree* AMDataTree::prototype(unsigned columnIndex) const {
	return prototypes_.at(columnIndex);
}

const AMDataTree* AMDataTree::prototype(const QString& columnName) const {
	if(yDNames_.containsF(columnName))
		return prototypes_.at(yDNames_.valueF(columnName));
	return NULL;
}





// Non-constant (modifier) member functions:
////////////////////////////////////////

/// set x-column data element
bool AMDataTree::setX(unsigned i, AMNumericType newValue) {
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
bool AMDataTree::setValue(unsigned columnIndex, unsigned i, AMNumericType newValue) {

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
bool AMDataTree::setValue(const QString& columnName, unsigned i, AMNumericType newValue) {


	if(yNames_.contains(columnName)) {
		return setValue(yNames_.value(columnName), i, newValue);
	}

	if(columnName == x_.name()) {
		return setX(i, newValue);
	}

	if(yDNames_.containsF(columnName)) {
		AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -2, "AMDataTree: attempted modifying multi-dimensional data as a single value. Not successful(VS1)."));
		return false;
	}

	//AMErrorMon::report(AMErrorReport(0, AMErrorReport::Alert, -3, QString("AMDataTree: attempted modifying non-existent data column '%1'. Not successful(VS2).").arg(columnName)));
	return false;

}

/// Create a new column of actual data. The column will have the right size (count()), but uninitialized data.  Returns the columnIndex of the new column.
unsigned AMDataTree::createColumn(const QString& newColumnName, const QString& description, const QString& units) {
	y_.append( AMDataTreeColumn(newColumnName, description, units, count()) );
	yNames_[newColumnName] = y_.count() - 1;
	return y_.count() - 1;
}

/// Create a new subtree column. All of the subtrees will be created with the given count, and implicit copies of \c treeFiller will be used as a "prototype" to initialize the trees in the column. (as long as count() isn't 0).  It's okay to use 'new AMDataTree(...)' without worrying about freeing the memory; once added to the column, shared reference counting will start from 1, and the prototype subtree will be deleted when no longer required.
/*! \note This will allocate memory for the new trees. However, reference counting is implemented so that they will be deleted automatically when the column vector (and the QSharedDataPointers within it) are deleted.
  */
void AMDataTree::createSubtreeColumn(const QString& newSubtreeColumnName, AMDataTree* treeFiller) {

	/// create the vector of subtree pointers:
	AMDataTreeSubtreeColumn newVect(count());

	/// Old way: create the trees, and store the shared-pointer references.
	/*
	for(unsigned i=0; i<count(); i++) {
		newVect[i] = new AMDataTree(subTreeCount, xColumnName, hasXValues);
	}*/
	/// New way: create one tree to use for all datapoints in the column, which will be implicitly shared until modified.
		prototypes_.append(treeFiller);
		if(count() > 0){
			QSharedDataPointer<AMDataTree> newTree(treeFiller);
			for(unsigned i=0; i<count(); i++)
				newVect[i] = newTree;
	}
	/// insert the new vector under this column name.
	yD_.append(newVect);
	yDNames_.set(newSubtreeColumnName, yD_.count() - 1);

}


/// Appending a new datapoint to each column is a common operation (for ex: a mono move and absorption measurement has just completed in an XAS scan.)
/*! This function increases the count() by 1 and appends a datapoint to each column. It sets the value of the primary column datapoint to \c newValue. (If hasXValues() is false, the \c newValue is ignored.)

Use setValue() or setLastValue() to fill in the remaining columns, otherwise they will contain default values.

If there are columns of subtrees, the default value of the new subtree is a copy of the most recent subtree. (At least until you change it.) (Exception: if count() is 0, there are no trees to emulate, so a default subtree is created.)

\test
*/
void AMDataTree::append(const AMNumericType& newValue) {

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
				yD_[i] << QSharedDataPointer<AMDataTree>(new AMDataTree());
			}
			else{
				yD_[i] << QSharedDataPointer<AMDataTree>(prototypes_.at(i));
			}
		}
		// otherwise append a copy of the latest tree.
		else
			yD_[i] << yD_[i][count_ - 2];
	}

	observable_.Emit(3, "columnChanged", -1);

}



/// clears all of the data in the tree. The count() will become 0, but all of the columns will remain.
void AMDataTree::clear() {
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
void AMDataTree::removeAll() {

	x_.clear();
	y_.clear();
	yD_.clear();
	count_ = 0;
}

