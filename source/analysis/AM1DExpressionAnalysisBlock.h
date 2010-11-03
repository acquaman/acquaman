#ifndef AM1DEXPRESSIONANALYSISBLOCK_H
#define AM1DEXPRESSIONANALYSISBLOCK_H

#include "dataman/AMAnalysisBlock.h"

/// This class implements an analysis block which takes a set of 1-dimensional input data sources, and creates a 1D result of the same size by applying a formula on a point-by-point basis. The formula can be any algebraic expression where the variable names come from the name()s of the input data sources.
/*! Current restrictions, which are flagged \todo to be removed:

  - Input data sources (in addition to having a rank() of 1) must have the same size().  This could be removed by adding a "subset" notation (ex: tey[10:30] / I0[11:31] )
  - Variables for the input data sources come from the name() of each input data source. You can also use "[name()].x" to refer to the input source's independent axisValue().  In the case where there are two inputs with the same name, we should eventually introduce some kind of notation to resolve this ambiguity. Currently, a variable refers to the first data source in the input list with a matching name.
  */
class AM1DExpressionAB : public AMAnalysisBlock
{
	Q_OBJECT
public:
	/// Constructor. \c outputName is the name() for the output data source.
	AM1DExpressionAB(const QString& outputName, QObject* parent = 0);

	/// Check if a set of inputs is valid. The empty list (no inputs) must always be valid.
	virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

	/// Set the data source inputs
	virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

	/// Return the current state of the outputs.
	/*! Valid: all inputs are in the Valid state, and the expression is also valid.
		Processing: one or more of the inputs are in the Processing state, and the expression is valid.
		Invalid: one or more of the inputs are in the Invalid state, OR the expression is invalid, OR the list of inputs is empty.
			*/
	virtual AMDataSource::State state() const;

	// Access to input data sources
	//////////////////////////

	/// Access the data sources which are currently providing input to this block
	virtual QList<AMDataSource*> inputDataSources() const;
	/// Number of current input sources
	virtual int inputDataSourceCount() const;
	/// Access input data source by index.  If \c index >= inputDataCount(), returns null pointer.
	virtual AMDataSource* inputDataSourceAt(int index) const;
	/// Retrieve index of an input data source by name. (Hopefully no two data sources have the same name; if they do, this returns the first one.) Returns -1 if no input source found with this name.
	/*! This might be involve a slow lookup; users should not call repeatedly.*/
	virtual int indexOfInputSource(const QString& dataSourceName) const;
	/// Retrieve index of an input data source by pointer. If it doesn't exist, returns -1.
	virtual int indexOfInputSource(const AMDataSource* source) const;

	// Creating editors for editing parameters
	////////////////////////////////////
	/// Create, connect, and return a widget suitable for displaying/editing this analysis block's custom parameters.  If you don't want to provide an editor widget, return 0.
	virtual QWidget* createEditorWidget();


	// AMDbObject interface
	////////////////////////////////

	/// Specialization of AMDbObject::typeDescription(). This can be re-implemented.
	virtual QString typeDescription() const {
		return "1D Expression Evaluator";
	}

	// Expression Setting for Y values
	//////////////////////////////
	/// Check if a given expression string is valid (for the current set of inputs)
	bool checkExpressionValidity(const QString& testExpression);

	/// Set the current expression used to evaluate the result. Any algebraic expression is valid; the allowed variables are the name()s of the input data sources, or '[name()].x' to refer to the independent variable of an input source.  If the expression is not valid, the state of the output goes to Invalid, and this returns false.
	bool setExpression(const QString& newExpression);

	/// Retrieve the current expression (It may or may not be valid -- whatever was last set with setExpression)
	QString expression() const {return QString::fromStdString(parser_.GetExpr()).trimmed(); }

	/// Check if the current expression is valid
	bool isExpressionValid() const { return isExpressionValid_; }

	// X-values (or axis values)
	///////////////////////////////
	/// Use this input source for the independent axis. (axisValue()).
	void setIndependentAxis


protected slots:
	void onInputSourceValuesChanged();
	void onInputSourceSizeChanged();
	void onInputSourceStateChanged();

protected:
	/// caches the state of the output data
	AMDataSource::State state_;
	/// caches whether the current expression is valid
	bool isExpressionValid_;

	/// Optimization: if the expression is just a single input source name, we can optimize to return its value directly.
	bool expressionDirect_;
	/// The index of the input source to use for the optimized expression




};

#endif // AM1DEXPRESSIONANALYSISBLOCK_H
