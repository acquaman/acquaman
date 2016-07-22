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


#ifndef AM1DEXPRESSIONANALYSISBLOCK_H
#define AM1DEXPRESSIONANALYSISBLOCK_H

#include "dataman/AMAnalysisBlock.h"
#include "muParser/muParser.h"

/// This struct is used by AM1DExpressionAB to group information about an expression variable.
struct AMParserVariable {
public:
	/// Convenience constructor
	AMParserVariable(int mSourceIndex = 0, bool mUseAxisValue = false) {
		sourceIndex = mSourceIndex;
		useAxisValue = mUseAxisValue;
		value = 0;
	}

	/// a place to hold a double value: used as the parser's variable storage
	/*! Why? The math expression parser gets optimized into bytecode, as long as the variable targets are not changed.  Therefore, this buffer holds the variable targets (in a constant location) for the expression parser. It's much faster to copy the input data values into these targets for every evaluation, and keep the bytecode optimization, rather than set new parser variables each time. */
	double value;
	/// index of input data source
	int sourceIndex;
	/// Whether to access the independent axisValue() instead of the dependent value() for this source
	bool useAxisValue;

};

/// This class implements an analysis block which takes a set of 1-dimensional input data sources, and creates a 1D result of the same size by applying a formula expression() on a point-by-point basis. The formula can be any algebraic expression where the variable names come from the name()s of the input data sources.
/*! A second expression xExpression() is provided to evaluate the independent variable (or axisValue()) of the result. By default, this simply takes on the axisValue() of the first input source.

Current restrictions, which are flagged \todo to be removed:

  - Input data sources (in addition to having a rank() of 1) must have the same size().  This could be removed by adding a "subset" notation (ex: tey[10:30] / I0[11:31] ). Since sizes may change, the output state() will go to invalid whenever the sizes of the inputs don't all match.
  - Variables for the input data sources come from the name() of each input data source. You can also use "[name()].x" to refer to the input source's independent axisValue().  In the case where there are two inputs with the same name, we should eventually introduce some kind of notation to resolve this ambiguity. Currently, a variable refers to the first data source in the input list with a matching name.
  */
class AM1DExpressionAB : public AMAnalysisBlock
{
	Q_OBJECT

	Q_PROPERTY(QString expression READ expression WRITE setExpression)
	Q_PROPERTY(QString xExpression READ xExpression WRITE setXExpression)

	Q_CLASSINFO("AMDbObject_Attributes", "description=1D Math Expression")

public:
	/// Constructor. \c outputName is the name() for the output data source.
	Q_INVOKABLE AM1DExpressionAB(const QString& outputName = "InvalidInput", QObject* parent = 0);
	/// Destructor.
	virtual ~AM1DExpressionAB();

	QString infoDescription() const { return QString("(%1)").arg(expression()); }

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, the requirements are...
	/*! - the rank() of all the inputs is 1
		- the size() of the inputs can be anything, although our output state() will go to Invalid whenever the sizes are not all matching.
		- anything else?
		*/
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Returns the desired rank for input sources.
	virtual int desiredInputRank() const { return 1; }

	// Access to input data sources
	//////////////////////////

	/// Access the data sources which are currently providing input to this block
	virtual QList<AMDataSource*> inputDataSources() const { return sources_; }
	/// Number of current input sources
	virtual int inputDataSourceCount() const { return sources_.count(); }
	/// Access input data source by index.  If \c index >= inputDataCount(), returns null pointer.
	virtual AMDataSource* inputDataSourceAt(int index) const { if(index<0 || index>=sources_.count()) return 0; return sources_.at(index); }
	/// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name, if they do, this returns the first one.) Returns -1 if no input source found with this name.
	/*! This might be involve a slow lookup; users should not call repeatedly.*/
	virtual int indexOfInputSource(const QString& dataSourceName) const;
	/// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
	virtual int indexOfInputSource(const AMDataSource* source) const;

	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing the expressions.
	virtual QWidget* createEditorWidget();

	// AMDataSource interface for ouput value. (rank of 1 -- single-dimensional)
	///////////////////////////////////

	/// Returns axis information for all axes
	virtual QList<AMAxisInfo> axes() const { QList<AMAxisInfo> rv; rv << axisInfoAt(0); return rv; }

	/// Returns the rank (number of dimensions) of this data set
	virtual int rank() const { return 1; }
	/// Returns the size of (ie: count along) each dimension
	virtual AMnDIndex size() const { if(isValid()) return AMnDIndex(size_); else return AMnDIndex(); }
	/// Returns the size along a single axis \c axisNumber. This should be fast.
	virtual int size(int axisNumber) const { if(axisNumber==0 && isValid()) return size_; else return 0; }
	/// Returns a bunch of information about a particular axis.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const;
	/// Returns the number of an axis, by name. (By number, we mean the index of the axis. We called it number to avoid ambiguity with indexes <i>into</i> axes.) This could be slow, so users shouldn't call it repeatedly.
	virtual int idOfAxis(const QString& axisName) const { if(axisName == axisInfo_.name) return 0; else return -1; }

	/// Set the meta-information describing the output axis.
	void setAxisInfo(const AMAxisInfo& newInfo) { axisInfo_ = newInfo; }


	// Data value access
	////////////////////////////

	/// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
	virtual AMNumber value(const AMnDIndex& indexes) const;

	/// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension.
	/*! The base-class implementation simply calls value() repeatedly, so you should absolutely re-implement this for better performance.

	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

\code
int outputSize = indexStart.totalPointsTo(indexEnd);
\endcode
*/
	virtual bool values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const;

	/// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
	virtual AMNumber axisValue(int axisNumber, int index) const;
	/// Performance optimization of axisValue():  instead of a single value, copies a block of values from \c startIndex to \c endIndex in \c outputValues.  The provided pointer must contain enough space for all the requested values.
    virtual bool axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const;

	// Expression Setting for Y values
	//////////////////////////////
	/// Check if a given expression string is valid (for the current set of inputs)
	bool checkExpressionValidity(const QString& testExpression);

	/// Set the current expression used to evaluate the value(). Any algebraic expression is valid, the allowed variables are the name()s of the input data sources, or '[name()].x' to refer to the independent variable of an input source.  If the expression is not valid, the state of the output goes to InvalidState, and this returns false. However, the expression will be set (whether currently valid or not) and may become valid if the inputs are changed.
	bool setExpression(const QString& newExpression);

	/// Retrieve the current expression (It may or may not be valid -- whatever was last set with setExpression)
	QString expression() const {return QString::fromStdString(parser_.GetExpr()).trimmed(); }

	/// Check if the current expression is valid
	bool isExpressionValid() const { return expressionValid_; }

	// X-values (or axis values)
	///////////////////////////////
	/// Set the expression used for the independent variable (aka x-axis... the one returned by axisValue()).   If \c xExpression is an empty string, the expression is set back to default, ie: the independent variable of the first input data source.  Error handling for invalid expressions is that same as for setExpression().
	bool setXExpression(const QString& xExpression = QString());

	/// Retrieve the current expression used for the axisValue(), whether valid or not
	QString xExpression() const {return QString::fromStdString(xParser_.GetExpr()).trimmed(); }

	/// Check if the current expression for the axisValue() is valid
	bool isXExpressionValid() const { return xExpressionValid_; }



protected slots:
	/// Connected to be called when the values of any of the input data sources change
	/*! \todo Optimize: only connect the used data sources to here. Don't connect all of them... */
	void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
	/// Connected to be called when the size of any input source changes
	void onInputSourceSizeChanged();
	/// Connected to be called when the state() flags of any input source change
	void onInputSourceStateChanged();

protected:
	/// Set the data source inputs.  The existing expression() and xExpression() are preserved
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);
	/// Method that checks whether all the sizes of used variables match.
	bool allUsedSizesMatch() const;
	/// caches whether the current expressions are valid. Set by setExpression(), setXExpression().
	bool expressionValid_, xExpressionValid_;

	/// Caches the size of the input/output data.
	int size_;

	/// the combined state of the inputs. Set by setInputDataSourcesImplementation and updated in onInputSourceStateChanged().
	int combinedInputState_;

	/// A list of all the variables we have available. (One for each input data source, and one for the axisValue() for each input data source.)
	QVector<AMParserVariable> allVariables_;
	/// A list of pointers to the variables used in the current expression.
	QVector<AMParserVariable*> usedVariables_, xUsedVariables_;


	/// List of input sources
	QList<AMDataSource*> sources_;

	/// Math expression parsers for evaluating output values.  parser_ is used to evaluate value(). parserX_ is used to evaluate axisValue().
	mu::Parser parser_, xParser_;

	/// Optimization: if the expression is just a single input source name, we can optimize to return its value directly, rather than use the parser.
	bool direct_, xDirect_;
	/// When using direct evaluation, these are the variables to use
	AMParserVariable directVar_, xDirectVar_;

	/// Used to flag that we are currently setting new input sources, so the setExpression() and setXExpression() calls should not cause setModified(true).
	bool currentlySettingInputSources_;

	/// Axis meta-information for our single axis.
	mutable AMAxisInfo axisInfo_;

	/// Helper function to review the conditions of everything that affects the output state, and then call setState appropriately.  Call this whenever anything might have changed to affect the output state flags.
	/*! Any of these will add the InvalidFlag to the state:

	  - No inputs
	  - Inputs of different sizes
	  - Any inputs that have the InvalidFlag set
	  - Invalid expression() or xExpression().

	  Any of these will add the ProcessingFlag:

	  - Any inputs that have the ProcessinFlag set.

	  Implementation note: it's okay to call setState() multiple times with the same state value.  For efficiency, it will only call emitStateChanged() when the new state is different from the old state.
	  */
	void reviewState();


};

#endif // AM1DEXPRESSIONANALYSISBLOCK_H
