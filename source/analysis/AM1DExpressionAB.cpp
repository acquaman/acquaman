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


#include "AM1DExpressionAB.h"
#include "util/AMErrorMonitor.h"
#include "AM1DExpressionABEditor.h"
#include <limits>
#include <QDebug>

 AM1DExpressionAB::~AM1DExpressionAB(){}
AM1DExpressionAB::AM1DExpressionAB(const QString& outputName, QObject* parent)
	: AMAnalysisBlock(outputName, parent),
	  axisInfo_(outputName + "_x", 0)
{
	currentlySettingInputSources_ = false;

	// We're not using direct evaluation at the start
	direct_ = xDirect_ = false;

	// initialize the parser:
	parser_.DefineNameChars("0123456789_:.[]"
							"abcdefghijklmnopqrstuvwxyz"
							"ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	xParser_.DefineNameChars("0123456789_:.[]"
							 "abcdefghijklmnopqrstuvwxyz"
							 "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

	// initial state is invalid: no inputs.
	setState(AMDataSource::InvalidFlag);
}

// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, the requirements are...
/* - the rank() of all the inputs is 1
 - the size() of the inputs can be anything, although our output state() will go to InvalidState whenever the sizes are not all matching.
 - anything else?
 */
bool AM1DExpressionAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const {
	for(int i=0; i<dataSources.count(); i++) {
		if(dataSources.at(i)->rank() != 1)
			return false;
	}
	return true;
}

void AM1DExpressionAB::reviewState() {
	int state = 0;

	if(inputDataSourceCount() == 0 || !allUsedSizesMatch() || !expressionValid_ || !xExpressionValid_)
		state |= AMDataSource::InvalidFlag;

	state |= combinedInputState_;

	setState(state);
}


// Access to input data sources
//////////////////////////

// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name, if they do, this returns the first one.) Returns -1 if no input source found with this name.
/* This might be involve a slow lookup; users should not call repeatedly.*/
int AM1DExpressionAB::indexOfInputSource(const QString& dataSourceName) const {
	for(int i=0; i<inputDataSourceCount(); i++)
		if(inputDataSourceAt(i)->name() == dataSourceName)
			return i;
	return -1;
}
// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
int AM1DExpressionAB::indexOfInputSource(const AMDataSource* source) const {
	for(int i=0; i<inputDataSourceCount(); i++)
		if(inputDataSourceAt(i) == source)
			return i;
	return 0;
}

// Creating editors for editing parameters
////////////////////////////////////
// Create, connect, and return a widget suitable for displaying/editing this analysis block's custom parameters.  If you don't want to provide an editor widget, return 0.
QWidget* AM1DExpressionAB::createEditorWidget() {
	return new AM1DExpressionABEditor(this);
}



// Axis Information
/////////////////////////

// Returns a bunch of information about a particular axis.
AMAxisInfo AM1DExpressionAB::axisInfoAt(int axisNumber) const {
	if(axisNumber != 0)
		return AMAxisInfo("invalid", 0);

	axisInfo_.size = size_;
	/// \todo: optimization: the axis is uniform if its a direct output of the axisValue() of a source with a uniform axis.
	axisInfo_.isUniform = false;
	return axisInfo_;
}


// Data value access
////////////////////////////

// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
AMNumber AM1DExpressionAB::value(const AMnDIndex& indexes) const {
	if(!isValid())	// will catch most invalid situations: non matching sizes, invalid inputs, invalid expressions.
		return AMNumber(AMNumber::InvalidError);

	if(indexes.rank() != 1)
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(indexes.i() < 0 || indexes.i() >= size_)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif


	// can we get it directly? Single-value expressions don't require the parser.
	if(direct_) {
		// info on which variable to use is contained in directVar_.
		if(directVar_.useAxisValue)
			return sources_.at(directVar_.sourceIndex)->axisValue(0, indexes.i());
		else
			return sources_.at(directVar_.sourceIndex)->value(indexes);
	}

	// otherwise we need the parser
	else {
		// copy the new input data values into parser storage
		for(int i=0; i<usedVariables_.count(); i++) {
			AMParserVariable* usedVar = usedVariables_.at(i);
			if(usedVar->useAxisValue)
				usedVar->value = sources_.at(usedVar->sourceIndex)->axisValue(0, indexes.i());
			else
				usedVar->value = sources_.at(usedVar->sourceIndex)->value(indexes);
		}

		// evaluate using the parser:
		double rv;
		try {
			rv = parser_.Eval();
		}
		catch(mu::Parser::exception_type& e) {
			QString explanation = QString("AM1DExpressionAB Analysis Block: error evaluating value: %1: '%2'.  We found '%3' at position %4.").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
			return AMNumber(AMNumber::InvalidError);
		}
		if (rv == std::numeric_limits<qreal>::infinity() || rv == -std::numeric_limits<qreal>::infinity() || rv == std::numeric_limits<qreal>::quiet_NaN() || std::isnan(rv))
						return 0;


		return rv;
	}
}

bool AM1DExpressionAB::values(const AMnDIndex &indexStart, const AMnDIndex &indexEnd, double *outputValues) const
{
	if(!isValid())	// will catch most invalid situations: non matching sizes, invalid inputs, invalid expressions.
		return false;

	if(indexStart.rank() != 1 || indexEnd.rank() != 1)
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if((unsigned)indexEnd.i() >= (unsigned)size_ || indexStart.i() > indexEnd.i())
			return false;
#endif

	int totalSize = indexEnd.i() - indexStart.i() + 1;
	int offset = indexStart.i();

	// can we get it directly? Single-value expressions don't require the parser.
	if(direct_) {
		// info on which variable to use is contained in directVar_.
		if(directVar_.useAxisValue) {
			for(int i=0; i<totalSize; i++)
				outputValues[i] = sources_.at(directVar_.sourceIndex)->axisValue(0, i+offset);	/// \todo Create a AMDataSource::axisValues().
			return true;
		}
		else {
			return sources_.at(directVar_.sourceIndex)->values(indexStart, indexEnd, outputValues);
		}
	}

	// otherwise we need the parser
	else {
		// block-copy all of the input data sources (that are actually used in the expression) into allVarData.
		QList<QVector<double> > allVarData;
		for(int v=0; v<usedVariables_.count(); ++v) {
			QVector<double> varData(totalSize);

			AMParserVariable* usedVar = usedVariables_.at(v);
			if(usedVar->useAxisValue) {
				for(int i=0; i<totalSize; i++)
					varData[i] = sources_.at(usedVar->sourceIndex)->axisValue(0, i+offset);
			}
			else {
				bool success = sources_.at(usedVar->sourceIndex)->values(indexStart, indexEnd, varData.data());
				if(!success)
					return false;
			}
			allVarData << varData;
		}

		// loop through and parse all values
		for(int i=0; i<totalSize; ++i) {	// loop through points

			for(int v=0,cc=usedVariables_.count(); v<cc; ++v) {
				usedVariables_.at(v)->value = allVarData.at(v).at(i);
			}

			// evaluate using the parser:
			double rv;
			try {
				rv = parser_.Eval();
			}
			catch(mu::Parser::exception_type& e) {
				QString explanation = QString("AM1DExpressionAB Analysis Block: error evaluating value: %1: '%2'.  We found '%3' at position %4.").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
				return false;
			}

			if (rv == std::numeric_limits<qreal>::infinity() || rv == -std::numeric_limits<qreal>::infinity() || rv == std::numeric_limits<qreal>::quiet_NaN() || std::isnan(rv))
				rv = 0;

			outputValues[i] = rv;
		}

		return true;
	}
}


// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
AMNumber AM1DExpressionAB::axisValue(int axisNumber, int index) const
{
	if(!isValid())	// will catch most invalid situations: non matching sizes, invalid inputs, invalid expressions.
		return AMNumber(AMNumber::InvalidError);

	if(axisNumber != 0)	// someone gave us a multi-dim index for a 1D dataset
		return AMNumber(AMNumber::DimensionError);

#ifdef AM_ENABLE_BOUNDS_CHECKING
		if(index < 0 || index >= size_)
			return AMNumber(AMNumber::OutOfBoundsError);
#endif


	// can we get it directly? Single-value expressions don't require the parser.
	if(xDirect_) {
		// info on which variable to use is contained in xDirectVar_.
		if(xDirectVar_.useAxisValue)
			return sources_.at(xDirectVar_.sourceIndex)->axisValue(0, index);
		else
			return sources_.at(xDirectVar_.sourceIndex)->value(index);
	}

	// otherwise we need the parser
	else {
		// copy the new input data values into parser storage
		for(int i=0; i<xUsedVariables_.count(); i++) {
			AMParserVariable* usedVar = xUsedVariables_.at(i);
			if(usedVar->useAxisValue)
				usedVar->value = sources_.at(usedVar->sourceIndex)->axisValue(0, index);
			else
				usedVar->value = sources_.at(usedVar->sourceIndex)->value(index);
		}

		// evaluate using the parser:
		double rv;
		try {
			rv = xParser_.Eval();
		}
		catch(mu::Parser::exception_type& e) {
			QString explanation = QString("AM1DExpressionAB Analysis Block: error evaluating value: %1: '%2'.  We found '%3' at position %4.").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
			return AMNumber(AMNumber::InvalidError);
		}
		return rv;
	}
}

bool AM1DExpressionAB::axisValues(int axisNumber, int startIndex, int endIndex, double *outputValues) const
{
	if(!isValid())	// will catch most invalid situations: non matching sizes, invalid inputs, invalid expressions.
		return false;

	if(axisNumber != 0)	// someone gave us a multi-dim index for a 1D dataset
		return false;

#ifdef AM_ENABLE_BOUNDS_CHECKING
	if(startIndex < 0 || startIndex >= size_)
		return false;

	if(endIndex < 0 || endIndex >= size_)
		return false;
#endif

	if (xDirect_){

		if (xDirectVar_.useAxisValue)
			sources_.at(xDirectVar_.sourceIndex)->axisValues(axisNumber, startIndex, endIndex, outputValues);

        else
            sources_.at(xDirectVar_.sourceIndex)->values(startIndex, endIndex, outputValues);
	}

	else {

		int totalSize = endIndex - startIndex + 1;

		// block-copy all of the input data sources (that are actually used in the expression) into allVarData.
		QList<QVector<double> > allVarData;
		for(int v=0; v<xUsedVariables_.count(); ++v) {
			QVector<double> varData(totalSize);

			AMParserVariable* usedVar = xUsedVariables_.at(v);
			if(usedVar->useAxisValue) {
				for(int i=0; i<totalSize; i++)
					varData[i] = sources_.at(usedVar->sourceIndex)->axisValue(0, i+startIndex);
			}
			else {
				bool success = sources_.at(usedVar->sourceIndex)->values(startIndex, endIndex, varData.data());
				if(!success)
					return false;
			}
			allVarData << varData;
		}

		// loop through and parse all values
		for(int i=0; i<totalSize; i++) {	// loop through points

			for(int v=0,cc=xUsedVariables_.count(); v<cc; v++) {
				xUsedVariables_.at(v)->value = allVarData.at(v).at(i);
			}

			// evaluate using the parser:
			double rv;
			try {
				rv = parser_.Eval();
			}
			catch(mu::Parser::exception_type& e) {
				QString explanation = QString("AM1DExpressionAB Analysis Block: error evaluating value: %1: '%2'.  We found '%3' at position %4.").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
				AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
				return false;
			}

			if (rv == std::numeric_limits<qreal>::infinity() || rv == -std::numeric_limits<qreal>::infinity() || rv == std::numeric_limits<qreal>::quiet_NaN() || std::isnan(rv))
				rv = 0;

            outputValues[i] = rv;
		}
	}

	return true;
}


void AM1DExpressionAB::onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end) {
	emitValuesChanged(start, end);
}

// If the inputs change size, this will affect the output state. All the inputs need to be the same size for now, otherwise the result is invalid.
void AM1DExpressionAB::onInputSourceSizeChanged() {

	if (inputDataSourceCount() > 0){

		size_ = inputDataSourceAt(0)->size(0);
		emitSizeChanged(0);
	}

	reviewState();
}

// If the inputs change state, this will affect the output state.  If any inputs are InvalidState, then the output is InvalidState.  If any of the inputs are in ProcessingState, then the output is ProcessingState. Otherwise, it's ReadyState.
void AM1DExpressionAB::onInputSourceStateChanged() {
	combinedInputState_ = 0;
	for(int i = 0; i < usedVariables_.size(); i++)
		combinedInputState_ |= inputDataSourceAt(usedVariables_.at(i)->sourceIndex)->state();

	// anything that could trigger a change in the output validity must call this
	reviewState();
}

// Set the data source inputs.
/* \note Whenever new input sources are set, if the xExpression() is blank/invalid, it is automatically initialized to the axisValue() of the first input source. Otherwise it, like expression(), is left as it was prior to setting the new inputs. Note that if the names of the new inputs are different, the old expressions will both likely become invalid. */
void AM1DExpressionAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources) {

	currentlySettingInputSources_ = true;

	QString oldExpression = expression();
	QString oldXExpression = xExpression();

	// disconnect signals from the old data sources (if any)
	for(int i=0; i<inputDataSourceCount(); i++) {
		disconnect(inputDataSourceAt(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));
		disconnect(inputDataSourceAt(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		disconnect(inputDataSourceAt(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	// done with the old sources. These are our new sources now
	sources_ = dataSources;

	// clear old parser variables
	parser_.ClearVar();
	xParser_.ClearVar();
	allVariables_.clear();
	usedVariables_.clear();

	// create registry of variables: two for each source
	for(int i=0; i<sources_.count(); i++) {
		allVariables_ << AMParserVariable(i, false);	// dependent value() variable
		allVariables_ << AMParserVariable(i, true); // independent axisValue() variable
	}

	// install all these variables in the parser. Name comes from the data source name, with ".x" added for the axisValue() version.
	for(int i=0; i<allVariables_.count(); i++) {
		QString varName = sources_.at(allVariables_.at(i).sourceIndex)->name();
		if(allVariables_.at(i).useAxisValue)
			varName.append(".x");
		try {
			parser_.DefineVar( varName.toStdString(), &(allVariables_[i].value) );
			xParser_.DefineVar( varName.toStdString(), &(allVariables_[i].value) );
		}
		catch(mu::Parser::exception_type& e) {
			QString explanation = QString("AM1DExpressionAB: Error setting up variables: %1 [%2].").arg(QString::fromStdString(e.GetMsg())).arg(varName);
			AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, e.GetCode(), explanation));
		}
	}


	// connect new input sources to signal handlers.
	for(int i=0; i<sources_.count(); i++) {
		connect(sources_.at(i)->signalSource(), SIGNAL(valuesChanged(AMnDIndex,AMnDIndex)), this, SLOT(onInputSourceValuesChanged(AMnDIndex,AMnDIndex)));	/// \todo Optimization: only connect this when this input source is actually used in the expression...
		connect(inputDataSourceAt(i)->signalSource(), SIGNAL(sizeChanged(int)), this, SLOT(onInputSourceSizeChanged()));
		connect(inputDataSourceAt(i)->signalSource(), SIGNAL(stateChanged(int)), this, SLOT(onInputSourceStateChanged()));
	}

	// initialize the combination of state flags. These will be kept updated by onInputSourceStateChanged()
	combinedInputState_ = 0;
	onInputSourceStateChanged();

	setExpression(oldExpression);
	setXExpression(oldXExpression);

	// both the setExpression() and setXExpression() calls will end by calling reviewState(), which will call setState() appropriately, given the status of the inputs, their sizes, and the expression validity.

	currentlySettingInputSources_ = false;

}

bool AM1DExpressionAB::allUsedSizesMatch() const
{
	int size = -1;
	bool sizesMatch = true;

	foreach (AMParserVariable *var, usedVariables_){

		if (size == -1)
			size = inputDataSourceAt(var->sourceIndex)->size(0);

		else if (size != inputDataSourceAt(var->sourceIndex)->size(0))
			sizesMatch = false;
	}

	return sizesMatch;
}

// Check if a given expression string is valid (for the current set of inputs)
bool AM1DExpressionAB::checkExpressionValidity(const QString& testExpression) {

	bool ok = true;
	// test methodology: actually set this as the expression in the current parser, and see if it's okay. Make sure to save and restore the actual expression.
	std::string oldExpression = parser_.GetExpr();
	try {
		parser_.SetExpr(testExpression.toStdString());
		parser_.Eval();
	}
	catch(mu::Parser::exception_type& e) {
		ok = false;
	}

	// restore:
	try {
		parser_.SetExpr(oldExpression);
	}
	catch(mu::Parser::exception_type& e) {
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, -42, "Failed to restore the old expression after testing a new one. This should never happen -- Please report this bug to the Acquaman developers."));
	}

	return ok;
}

// Set the current expression used to evaluate the result. If the expression is not valid, the state of the output goes to Invalid, and this returns false.
bool AM1DExpressionAB::setExpression(const QString& newExpression) {

	expressionValid_ = true;
	usedVariables_.clear();
	direct_ = false;

	try {
		parser_.SetExpr(newExpression.toStdString());
		parser_.Eval();

		// determine the usedVariables_:
		mu::varmap_type usedvar = parser_.GetUsedVar();
		mu::varmap_type::const_iterator item = usedvar.begin();
		// Query the used variables.  item->second is a pointer to the variable storage location. (With a better design, we might not need the inner loop.)
		for (; item!=usedvar.end(); ++item)
		{
			// search to find out if a variable is used. It is used if the pointer to the storage location matches item->second.
			for(int i=0; i<allVariables_.count(); i++)
				if( &(allVariables_.at(i).value) == item->second)
					usedVariables_ << &(allVariables_[i]);
		}

		// Optimization: is the whole expression merely a single input source name? Can we operate in direct mode?
		if(expressionValid_ && usedVariables_.count() == 1) {	// precondition: only one input variable
			directVar_ = *(usedVariables_.at(0));	// storing it here won't matter if direct_ is not set.
			if(directVar_.useAxisValue) {	// is this an axisValue name?
				if(newExpression.trimmed() == sources_.at(directVar_.sourceIndex)->name() + ".x")	// the whole expression matches just the name. We can go direct.
					direct_ = true;
			}
			else {
				if(newExpression.trimmed() == sources_.at(directVar_.sourceIndex)->name())	// whole expression matches just the name. Can go direct
					direct_ = true;
			}
		}

	}
	catch(mu::Parser::exception_type& e) {
		expressionValid_ = false;
	}

	// anything that could trigger a change in the output validity must call this
	onInputSourceStateChanged();
	onInputSourceSizeChanged();
	reviewState();
	emitValuesChanged();

	if(!currentlySettingInputSources_)
		setModified(true);

	return expressionValid_;
}


// X-values (or axis values)
///////////////////////////////
// Set the expression used for the independent variable (aka x-axis... the one returned by axisValue()).   If \c xExpression is an empty string, the expression is set back to default, ie: the independent variable of the first input data source.
bool AM1DExpressionAB::setXExpression(const QString& xExpressionIn) {

	QString xExpression = xExpressionIn;

	xExpressionValid_ = true;
	xUsedVariables_.clear();
	xDirect_ = false;

	if(xExpression.isEmpty() && !sources_.isEmpty())
		xExpression = sources_.at(0)->name() + ".x";

	try {
		xParser_.SetExpr(xExpression.toStdString());
		xParser_.Eval();

		// determine usedVariables_:
		mu::varmap_type usedvar = xParser_.GetUsedVar();
		mu::varmap_type::const_iterator item = usedvar.begin();
		// Query the used variables.  item->second is a pointer to the variable storage location. (With a better design, we might not need the inner loop.)
		for (; item!=usedvar.end(); ++item)
		{
			// search to find out if a variable is used. It is used if the pointer to the storage location matches item->second.
			for(int i=0; i<allVariables_.count(); i++)
				if( &(allVariables_.at(i).value) == item->second)
					xUsedVariables_ << &(allVariables_[i]);
		}

		// Optimization: is the whole expression merely a single input source name? Can we operate in direct mode?
		if(xExpressionValid_ && xUsedVariables_.count() == 1) {	// precondition: only one input variable
			xDirectVar_ = *(xUsedVariables_.at(0));	// storing it here won't matter if xDirect_ is not set.
			if(xDirectVar_.useAxisValue) {	// is this an axisValue name?
				if(xExpression.trimmed() == sources_.at(xDirectVar_.sourceIndex)->name() + ".x")	// the whole expression matches just the name. We can go direct.
					xDirect_ = true;
			}
			else {
				if(xExpression.trimmed() == sources_.at(xDirectVar_.sourceIndex)->name())	// whole expression matches justthe name. Can go direct
					xDirect_ = true;
			}
		}

	}
	catch(mu::Parser::exception_type& e) {
		xExpressionValid_ = false;
	}

	onInputSourceStateChanged();
	onInputSourceSizeChanged();
	// anything that could trigger a change in the output validity must call this
	reviewState();
	emitValuesChanged();	/// \todo: actually, we mean that the axis values changed. How to signal that?

	if(!currentlySettingInputSources_)
		setModified(true);

	return xExpressionValid_;
}
