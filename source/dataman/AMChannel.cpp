#include "AMChannel.h"

// Forward declaration of Scan
#include "AMScan.h"
#include "AMErrorMonitor.h"


AMChannel::AMChannel(AMScan* scan, const QString& name, const QString& expression) : QObject(scan)
{
    name_ = name;
	scan_ = scan;

	parser_.DefineNameChars("0123456789_:"
						   "abcdefghijklmnopqrstuvwxyz"
						   "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	//parser_.DefineOprtChars("abcdefghijklmnopqrstuvwxyz"
	//					   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		//				   "+-*^/?<>=#!$%&|~'_");
	//parser_.DefineInfixOprtChars("/+-*^?<>=#!$%&|~'_");

	setExpression(expression);
}

bool AMChannel::setExpression(const QString& expression) {

	/// \todo only needed when the data tree columns change.
	if(!setVariablesFromDataColumns())
		return false;


	try {
		parser_.SetExpr(expression.toStdString());
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel: %1: '%2'.  I found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, e.GetCode(), explanation));
		return false;
	}

	return true;
}

double AMChannel::value(unsigned p) const {

	AMDataTree* t = dataTree();

	/// \todo optimization: only copy the required variables, using parser_.GetUsedVar()
	varStorage_[0] = t->x(p);
	for(int i=0; unsigned(i) < t->numYColumns(); i++)
		varStorage_[i+1] = t->value(i, p);

	double rv;
	try {
		rv = parser_.Eval();
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel: %1: '%2'.  I found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, e.GetCode(), explanation));
		return 0.0;
	}

	return rv;
}


bool AMChannel::setVariablesFromDataColumns() {

	parser_.ClearVar();

	// add all of the raw data column names as allowed variables in the expression parser
		// the values from the columns will be buffered in varStorage_ for evaluation.
	QStringList ycols = dataTree()->yColumnNames();
	varStorage_.resize(ycols.count()+1);

	try {
		parser_.DefineVar(dataTree()->xName().toStdString(), varStorage_.data() );
		for(int i=0; i<ycols.count(); i++)
			parser_.DefineVar(ycols.at(i).toStdString(), varStorage_.data()+1+i);
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel: %1: '%2'.  I found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg()), QString::fromStdString(e.GetExpr()), QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Alert, e.GetCode(), explanation));
		return false;
	}

	return true;
}


AMDataTree* AMChannel::dataTree() const {
	return &(scan()->d_);
}
