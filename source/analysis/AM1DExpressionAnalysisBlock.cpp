#include "AM1DExpressionAnalysisBlock.h"

AM1DExpressionAB::AM1DExpressionAnalysisBlock(const QString& outputName, QObject* parent)
	: AMAnalysisBlock(outputName, parent)
{
	state_ = AMDataSource::InvalidState;
}

/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid.
bool AM1DExpressionAB::areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

/// Set the data source inputs
void AM1DExpressionAB::setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

/// Return the current state of the outputs.
/*! Valid: all inputs are in the Valid state, and the expression is also valid.
	Processing: one or more of the inputs are in the Processing state, and the expression is valid.
	Invalid: one or more of the inputs are in the Invalid state, OR the expression is invalid, OR the list of inputs is empty.
		*/
AMDataSource::State AM1DExpressionAB::state() const {

}

// Access to input data sources
//////////////////////////

/// Access the data sources which are currently providing input to this block
QList<AMDataSource*> AM1DExpressionAB::inputDataSources() const {

}
/// Number of current input sources
int AM1DExpressionAB::inputDataSourceCount() const {

}
/// Access input data source by index.  If \c index >= inputDataCount(), returns null pointer.
AMDataSource* AM1DExpressionAB::inputDataSourceAt(int index) const {

}
/// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name; if they do, this returns the first one.) Returns -1 if no input source found with this name.
/*! This might be involve a slow lookup; users should not call repeatedly.*/
int AM1DExpressionAB::indexOfInputSource(const QString& dataSourceName) const {

}
/// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
int AM1DExpressionAB::indexOfInputSource(const AMDataSource* source) const {

}

// Creating editors for editing parameters
////////////////////////////////////
/// Create, connect, and return a widget suitable for displaying/editing this analysis block's custom parameters.  If you don't want to provide an editor widget, return 0.
QWidget* AM1DExpressionAB::createEditorWidget();



void AM1DExpressionAB::onInputSourceValuesChanged() {

}

void AM1DExpressionAB::onInputSourceSizeChanged() {

}

void AM1DExpressionAB::onInputSourceStateChanged() {

}

/// Check if a given expression string is valid (for the current set of inputs)
bool AM1DExpressionAB::checkExpressionValidity(const QString& testExpression);

/// Set the current expression used to evaluate the result. If the expression is not valid, the state of the output goes to Invalid, and this returns false.
bool AM1DExpressionAB::setExpression(const QString& newExpression);

/// Retrieve the current expression (It may or may not be valid -- whatever was last set with setExpression)
QString AM1DExpressionAB::expression() const;

/// Check if the current expression is valid
bool AM1DExpressionAB::isExpressionValid() const;
