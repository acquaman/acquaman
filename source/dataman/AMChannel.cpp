#include "AMChannel.h"

// Forward declaration of Scan
#include "AMScan.h"
#include "AMErrorMonitor.h"

#include <QDebug>

AMChannel::AMChannel(AMScan* scan, const QString& name, const QString& expression, const QString& xExpression) : QObject(scan), AMObserver(), MPlotAbstractSeriesData()
{
	name_ = name;
	scan_ = scan;

	min_ = max_ = minX_ = maxX_ = -1;

	defaultX_ = true;

	dataTree()->observable()->addObserver(this);

	parser_.DefineNameChars("0123456789_:"
						   "abcdefghijklmnopqrstuvwxyz"
						   "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	parserX_.DefineNameChars("0123456789_:"
						   "abcdefghijklmnopqrstuvwxyz"
						   "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	//parser_.DefineOprtChars("abcdefghijklmnopqrstuvwxyz"
	//					   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		//				   "+-*^/?<>=#!$%&|~'_");
	//parser_.DefineInfixOprtChars("/+-*^?<>=#!$%&|~'_");

	setExpression(expression);
	setXExpression(xExpression);
}

bool AMChannel::setExpression(const QString& expression) {

	isValid_ = false;
	min_ = max_ = -1;

	/// \todo only needed when the data tree columns change.
	if(!setVariablesFromDataColumns()) {
		return false;
	}


	try {
		parser_.SetExpr(expression.toStdString());
		// check that the expression is valid (ie: variables are present, can be parsed, etc.)
		parser_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel (setting expression): %1: '%2'.  We found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
		return false;
	}

	// done.

	// optimization section: We need to copy the raw data columns into varStorage_ when evaluating. Let's figure out exactly which columns we need to copy, by seeing which variables are used.

	// Get the map with the variables
	mu::varmap_type usedvar = parser_.GetUsedVar();
	usedColumnIndices_.clear();

	mu::varmap_type::const_iterator item = usedvar.begin();

	// Query the used variables.  item->second is a pointer to the variable, so (item->second - varStorage_.data()) is the index within varStorage_.
	for (; item!=usedvar.end(); ++item)
	{
		int varStorageIndex = (int)( item->second - varStorage_.data() );
		// Since varStorage[0] is the x-column, and varStorage[i] is the (i-1)'th y-column, subtracting 1 will give us (-1) for the x-column, or the actual column index of the y-data column.
		usedColumnIndices_ << (varStorageIndex-1);
	}

	isValid_ = true;
	return true;
}

bool AMChannel::setXExpression(const QString& xExpression) {

	minX_ = maxX_ = -1;

	if(xExpression.isEmpty()) {
		defaultX_ = true;
		isValidX_ = true;
		usedColumnIndicesX_.clear();
		return true;
	}

	defaultX_ = false;
	isValidX_ = false;

	/// \todo only needed when the data tree columns change.
	if(!setVariablesFromDataColumns())
		return false;


	try {
		parserX_.SetExpr(xExpression.toStdString());
		parserX_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel (setting expression): %1: '%2'.  We found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
		return false;
	}

	// optimization section: We need to copy the raw data columns into varStorage_ when evaluating. Let's figure out exactly which columns we need to copy, by seeing which variables are used.

	// Get the map with the variables
	mu::varmap_type usedvar = parserX_.GetUsedVar();
	usedColumnIndicesX_.clear();

	mu::varmap_type::const_iterator item = usedvar.begin();

	// Query the used variables.  item->second is a pointer to the variable, so (item->second - varStorage_.data()) is the index within varStorage_.
	for (; item!=usedvar.end(); ++item)
	{
		int varStorageIndex = (int)( item->second - varStorage_.data() );
		// Since varStorage[0] is the x-column, and varStorage[i] is the (i-1)'th y-column, subtracting 1 will give us (-1) for the x-column, or the actual column index of the y-data column.
		usedColumnIndicesX_ << (varStorageIndex-1);
	}

	isValidX_ = true;
	return true;
}

double AMChannel::value(unsigned p) const {

	AMDataTree* t = dataTree();

	// Copy the raw data column values into varStorage_.
	// optimization: only copy the columns we actually use, based on the indices in usedColumnIndices_
	foreach(int usedCol, usedColumnIndices_) {
		if(usedCol == -1)
			varStorage_[0] = t->x(p);
		else
			varStorage_[usedCol+1] = t->value(usedCol, p);
	}

	/* Prior to optimization:
	varStorage_[0] = t->x(p);
	for(int i=0; unsigned(i) < t->numYColumns(); i++)
		varStorage_[i+1] = t->value(i, p);
	*/

	double rv;
	try {
		rv = parser_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel (evaluating value): %1: '%2'.  We found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, e.GetCode(), explanation));
		return 0.0;
	}

	return rv;
}

double AMChannel::x(unsigned p) const {

	AMDataTree* t = dataTree();

	// default x: just return the x column value
	if(defaultX_)
		return t->x(p);

	// evaluate value using the x-expression parser

	// Copy the raw data column values into varStorage_.
	// optimization: only copy the columns we actually use, based on the indices in usedColumnIndices_
	foreach(int usedCol, usedColumnIndicesX_) {
		if(usedCol == -1)
			varStorage_[0] = t->x(p);
		else
			varStorage_[usedCol+1] = t->value(usedCol, p);
	}

	double rv;
	try {
		rv = parserX_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel (evaluating x value): %1: '%2'.  We found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, e.GetCode(), explanation));
		return 0.0;
	}

	return rv;
}



bool AMChannel::setVariablesFromDataColumns() {

	parser_.ClearVar();
	parserX_.ClearVar();

	// add all of the raw data column names as allowed variables in the expression parser
		// the values from the columns will be buffered in varStorage_ for evaluation.
	QStringList ycols = dataTree()->yColumnNames();
	varStorage_.resize(ycols.count()+1);

	try {
		parser_.DefineVar(dataTree()->xName().toStdString(), varStorage_.data() );
		parserX_.DefineVar(dataTree()->xName().toStdString(), varStorage_.data() );
		for(int i=0; i<ycols.count(); i++) {
			parser_.DefineVar(ycols.at(i).toStdString(), varStorage_.data()+1+i);
			parserX_.DefineVar(ycols.at(i).toStdString(), varStorage_.data()+1+i);
		}
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel (setting variables): %1.").arg(QString::fromStdString(e.GetMsg()));
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
		return false;
	}

	return true;
}


AMDataTree* AMChannel::dataTree() const {
	return (scan()->d_);
}

QString AMChannel::errorMsg() const {
	QString rv;
	try {
		parser_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		rv = QString("%1").arg(QString::fromStdString(e.GetMsg()));
	}

	try {
		if(!defaultX_)
			parserX_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		rv.append(QString("\n %1").arg(QString::fromStdString(e.GetMsg())));
	}

	return rv;
}

QString AMChannel::longErrorMsg() const {
	QString rv;
	try {
		parser_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		rv = QString("%1 (expression: '%2').  We found '%3' at position %4.  Error code: %5.").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()),	QString::fromStdString(e.GetToken())).arg(e.GetPos()).arg(e.GetCode());
	}

	try {
		if(!defaultX_)
			parserX_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		rv.append(QString("\n %1 (X-axis expression: '%2').  We found '%3' at position %4.  Error code: %5.").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()),	QString::fromStdString(e.GetToken())).arg(e.GetPos()).arg(e.GetCode()));
	}

	return rv;
}

void AMChannel::searchMin() const {
	min_ = 0;
	for(int i=1; (unsigned)i<count(); i++)
		if(value(i) < value(min_))
			min_ = i;
}
void AMChannel::searchMax() const {
	max_ = 0;
	for(int i=1; (unsigned)i<count(); i++)
		if(value(i)>value(max_))
			max_ = i;
}
void AMChannel::searchMinX() const {
	minX_ = 0;
	for(int i=1; (unsigned)i<count(); i++)
		if(x(i)<x(minX_))
			minX_ = i;
}
void AMChannel::searchMaxX() const {
	maxX_ = 0;
	for(int i=1; (unsigned)i<count(); i++)
		if(x(i)>x(maxX_))
			maxX_ = i;
}

/// We observe AMDataTrees.  They Emit(3, "columnChanged", columnIndex) whenever values in a column change.
void AMChannel::onObservableChanged(AMObservable* source, int code, const char* msg, int colIndex) {
	Q_UNUSED(source)
	Q_UNUSED(msg)

	if( code != 3 || QString(msg) != QString("columnChanged") )
		return;

	// colIndex will be -1 if the x-data column changed, or the y-column index.
	if(usedColumnIndices_.contains(colIndex)) {
		min_ = max_ = -1;
		emit updated();
		Emit(0, "dataChanged");
	}
	if(usedColumnIndicesX_.contains(colIndex) || (defaultX_ && (colIndex == -1))) {
		minX_ = maxX_ = -1;
		emit updated();
		Emit(0, "dataChanged");
	}
}
