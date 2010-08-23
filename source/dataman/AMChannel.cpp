#include "AMChannel.h"

// Forward declaration of Scan
#include "AMScan.h"
#include "AMErrorMonitor.h"

#include <QDebug>

double* AddVariable(const char *a_szName, void *a_pUserData)
{
	QString fullVariable(a_szName);
	QStringList variableBreakdownBlanks = fullVariable.split('.');
	QStringList variableBreakdown = fullVariable.split('.', QString::SkipEmptyParts);
	if( (variableBreakdownBlanks.count() != variableBreakdown.count()) || ( (variableBreakdown.count() != 1) && (variableBreakdown.count() != 2) ) ){
		throw mu::Parser::exception_type(_T("not a valid variable."));
		return NULL;
	}
	AMChannel *ch = ((AMChannel*)a_pUserData);
	AMParVar *tmpVar = new AMParVar();
	if(variableBreakdown.count() == 2){
		QRegExp rx("\\[(\\d+)\\]$");
		bool firstHas = variableBreakdown.at(0).contains(rx) || variableBreakdown.at(0).contains("[");
		bool secondHas = variableBreakdown.at(1).contains(rx) || variableBreakdown.at(1).contains("[");
		if( (firstHas && secondHas) || (!firstHas && !secondHas) ){
			throw mu::Parser::exception_type(_T("not a valid variable."));
			return NULL;
		}
		tmpVar->indexer = rx.cap(1).toInt();
		QString tmpCol = variableBreakdown.at(1);
		QString tmpST = variableBreakdown.at(0);
		if(firstHas){
			tmpVar->level = 0;
			tmpST.remove(rx);
		}
		else{
			tmpVar->level = 1;
			tmpCol.remove(rx);
		}
		tmpVar->colName = tmpCol;
		tmpVar->stName = tmpST;
	}
	else
		tmpVar->colName = variableBreakdown.at(0);
	double *varSpace = ch->addVariable(tmpVar);
	if(varSpace == NULL){
		throw mu::Parser::exception_type(_T("not a valid variable."));
		return NULL;
	}
	return varSpace;
}

double* AddVariableX(const char *a_szName, void *a_pUserData)
{
	QString fullVariable(a_szName);
	QStringList variableBreakdownBlanks = fullVariable.split('.');
	QStringList variableBreakdown = fullVariable.split('.', QString::SkipEmptyParts);
	if( (variableBreakdownBlanks.count() != variableBreakdown.count()) || ( (variableBreakdown.count() != 1) && (variableBreakdown.count() != 2) ) ){
		throw mu::Parser::exception_type(_T("not a valid variable."));
		return NULL;
	}
	AMChannel *ch = ((AMChannel*)a_pUserData);
	AMParVar *tmpVar = new AMParVar();
	tmpVar->isXExpression = true;
	if(variableBreakdown.count() == 2){
		QRegExp rx("\\[(\\d+)\\]$");
		bool firstHas = variableBreakdown.at(0).contains(rx) || variableBreakdown.at(0).contains("[");
		bool secondHas = variableBreakdown.at(1).contains(rx) || variableBreakdown.at(1).contains("[");
		if( (firstHas && secondHas) || (!firstHas && !secondHas) ){
			throw mu::Parser::exception_type(_T("not a valid variable."));
			return NULL;
		}
		tmpVar->indexer = rx.cap(1).toInt();
		QString tmpCol = variableBreakdown.at(1);
		QString tmpST = variableBreakdown.at(0);
		if(firstHas){
			tmpVar->level = 0;
			tmpST.remove(rx);
		}
		else{
			tmpVar->level = 1;
			tmpCol.remove(rx);
		}
		tmpVar->colName = tmpCol;
		tmpVar->stName = tmpST;
	}
	else
		tmpVar->colName = variableBreakdown.at(0);
	double *varSpace = ch->addVariable(tmpVar);
	if(varSpace == NULL){
		throw mu::Parser::exception_type(_T("not a valid variable."));
		return NULL;
	}
	return varSpace;
}

AMChannel::AMChannel(AMScan* scan, const QString& name, const QString& expression, const QString& xExpression) : QObject(), AMObserver(), MPlotAbstractSeriesData()
{
	name_ = name;
	scan_ = scan;

	min_ = max_ = minX_ = maxX_ = -1;

	defaultX_ = true;

	dataTree()->observable()->addObserver(this);

	parser_.DefineNameChars("0123456789_:.[]"
						   "abcdefghijklmnopqrstuvwxyz"
						   "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	parserX_.DefineNameChars("0123456789_:.[]"
						   "abcdefghijklmnopqrstuvwxyz"
						   "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	//parser_.DefineOprtChars("abcdefghijklmnopqrstuvwxyz"
	//					   "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		//				   "+-*^/?<>=#!$%&|~'_");
	//parser_.DefineInfixOprtChars("/+-*^?<>=#!$%&|~'_");
	varStorage_.reserve(25);
	parser_.SetVarFactory(AddVariable, (void*)(this));
	parserX_.SetVarFactory(AddVariableX, (void*)(this));

	setExpression(expression);
	setXExpression(xExpression);
}

double* AMChannel::addVariable(AMParVar *parVar){
	if(parVar->stName == ""){
		if( !((dataTree()->xName() == parVar->colName) || dataTree()->yColumnNames().contains(parVar->colName) ) )
			return NULL;
		if(parVar->isXExpression){
			if( dataTree()->xName() == parVar->colName ){
				usedColumnIndicesX_ << -1;
				parVar->isX = true;
			}
			else
				usedColumnIndicesX_ << dataTree()->yColumnNames().indexOf(parVar->colName);
		}
		else{
			if( dataTree()->xName() == parVar->colName ){
				usedColumnIndices_ << -1;
				parVar->isX = true;
			}
			else
				usedColumnIndices_ << dataTree()->yColumnNames().indexOf(parVar->colName);
		}
	}
	else{
		if( !dataTree()->ySubtreeNames().contains(parVar->stName) || !( (dataTree()->prototype(parVar->stName)->xName() == parVar->colName) || dataTree()->prototype(parVar->stName)->yColumnNames().contains(parVar->colName) ) )
			return NULL;
		if( dataTree()->prototype(parVar->stName)->xName() == parVar->colName )
			parVar->isX = true;
	}
	if(varStorage_.capacity() == varLookUps_.count())
		varStorage_.resize((int)(1.5*varStorage_.capacity()));
	parVar->vectorIndex = varLookUps_.count();
	varLookUps_.append(parVar);
	varStorage_.append(0);
	return varStorage_.data()+parVar->vectorIndex;
}

QRectF AMChannel::boundingRect() const {
	if(count() == 0)
		return QRectF();
	else {
		double mins = min();
		double maxs = max();
		double minXs = minX();
		double maxXs = maxX();
		return QRectF(QPointF(minXs, mins), QSizeF(maxXs-minXs, maxs-mins));
	}
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
		QString explanation = QString("AMChannel (setting expression): %1: '%2'.  We found '%3' at position %4.  TODO what happens now?").arg(QString::fromStdString(e.GetMsg())).arg(QString::fromStdString(e.GetExpr())).arg(QString::fromStdString(e.GetToken())).arg(e.GetPos());
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
		return false;
	}

	// done.

	// optimization section: We need to copy the raw data columns into varStorage_ when evaluating. Let's figure out exactly which columns we need to copy, by seeing which variables are used.

	/* Doing in addVariable
	// Get the map with the variables
	mu::varmap_type usedvar = parser_.GetUsedVar();
	usedColumnIndices_.clear();

	mu::varmap_type::const_iterator item = usedvar.begin();

	std::cout << "\n\nVARIABLES\n";
	fflush(stdout);
	// Query the used variables.  item->second is a pointer to the variable, so (item->second - varStorage_.data()) is the index within varStorage_.
	for (; item!=usedvar.end(); ++item)
	{
		std::cout << "Used variable: " << item->first;
		fflush(stdout);
		int varStorageIndex = (int)( item->second - varStorage_.data() );
		// Since varStorage[0] is the x-column, and varStorage[i] is the (i-1)'th y-column, subtracting 1 will give us (-1) for the x-column, or the actual column index of the y-data column.
		usedColumnIndices_ << (varStorageIndex-1);
	}
	std::cout << "\n";
	fflush(stdout);
	*/

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

	/* Doing in addVariable
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
	*/

	isValidX_ = true;
	return true;
}

double AMChannel::value(unsigned p) const {

	AMDataTree* t = dataTree();

	AMParVar *tmpVar;
	for(int i=0; i<varLookUps_.count(); i++){
		tmpVar = varLookUps_.at(i);
		if( (tmpVar->indexer == -1) && (tmpVar->level == -1) ){
			if(tmpVar->isX)
			{varStorage_[tmpVar->vectorIndex] = t->x(p);}//qDebug() << "At time0: " << t->x(p);}
			else
			{varStorage_[tmpVar->vectorIndex] = t->value(tmpVar->colName, p);}//qDebug() << "At time1: " << t->value(tmpVar->colName, p);}
		}
		else{
			if(tmpVar->isX){
				if(tmpVar->level == 0)
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, tmpVar->indexer)->x(p);}//qDebug() << "At time2: " << t->deeper(tmpVar->stName, tmpVar->indexer)->x(p);}
				else
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, p)->x(tmpVar->indexer);}//qDebug() << "At time3: " << t->deeper(tmpVar->stName, p)->x(tmpVar->indexer);}
			}
			else{
				if(tmpVar->level == 0)
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, tmpVar->indexer)->value(tmpVar->colName, p);}//qDebug() << "At time4: " << t->deeper(tmpVar->stName, tmpVar->indexer)->value(tmpVar->colName, p);}
				else
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, p)->value(tmpVar->colName, tmpVar->indexer);}//qDebug() << "At time5: " << t->deeper(tmpVar->stName, p)->value(tmpVar->colName, tmpVar->indexer);}
			}
		}
		//qDebug() << "tmpVar: " << tmpVar->stName << tmpVar->colName << tmpVar->isX << tmpVar->vectorIndex << tmpVar->indexer << tmpVar->level;
		//qDebug() << "Tried writing index: " << tmpVar->vectorIndex << " value: " << varStorage_[tmpVar->vectorIndex];
	}
	/*
	// Copy the raw data column values into varStorage_.
	// optimization: only copy the columns we actually use, based on the indices in usedColumnIndices_
	foreach(int usedCol, usedColumnIndices_) {
		if(usedCol == -1)
			varStorage_[0] = t->x(p);
		else{
			if(usedCol < t->numYColumns())
				varStorage_[usedCol+1] = t->value(usedCol, p);
			else{
				if(usedCol == t->numYColumns())
					varStorage_[usedCol+1] = t->deeper(0, p)->x(199);
				else
					varStorage_[usedCol+1] = t->deeper(0, p)->value(0, 199);
			}
		}
	}
	*/

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
	if(defaultX_){
		//qDebug() << "Just returning default X";
		return t->x(p);
	}

	AMParVar *tmpVar;
	for(int i=0; i<varLookUps_.count(); i++){
		tmpVar = varLookUps_.at(i);
		if( (tmpVar->indexer == -1) && (tmpVar->level == -1) ){
			if(tmpVar->isX)
			{varStorage_[tmpVar->vectorIndex] = t->x(p);}//qDebug() << "At time0: " << t->x(p);}
			else
			{varStorage_[tmpVar->vectorIndex] = t->value(tmpVar->colName, p);}//qDebug() << "At time1: " << t->value(tmpVar->colName, p);}
		}
		else{
			if(tmpVar->isX){
				if(tmpVar->level == 0)
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, tmpVar->indexer)->x(p);}//qDebug() << "At time2: " << t->deeper(tmpVar->stName, tmpVar->indexer)->x(p);}
				else
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, p)->x(tmpVar->indexer);}//qDebug() << "At time3: " << t->deeper(tmpVar->stName, p)->x(tmpVar->indexer);}
			}
			else{
				if(tmpVar->level == 0)
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, tmpVar->indexer)->value(tmpVar->colName, p);}//qDebug() << "At time4: " << t->deeper(tmpVar->stName, tmpVar->indexer)->value(tmpVar->colName, p);}
				else
				{varStorage_[tmpVar->vectorIndex] = t->deeper(tmpVar->stName, p)->value(tmpVar->colName, tmpVar->indexer);}//qDebug() << "At time5: " << t->deeper(tmpVar->stName, p)->value(tmpVar->colName, tmpVar->indexer);}
			}
		}
		//qDebug() << "tmpVar: " << tmpVar->stName << tmpVar->colName << tmpVar->isX << tmpVar->vectorIndex << tmpVar->indexer << tmpVar->level;
		//qDebug() << "Tried writing index: " << tmpVar->vectorIndex << " value: " << varStorage_[tmpVar->vectorIndex];
	}


	//qDebug() << "Not returning default X";
	// evaluate value using the x-expression parser

	/*
	// Copy the raw data column values into varStorage_.
	// optimization: only copy the columns we actually use, based on the indices in usedColumnIndices_
	foreach(int usedCol, usedColumnIndicesX_) {
		if(usedCol == -1)
			varStorage_[0] = t->x(p);
		else
			varStorage_[usedCol+1] = t->value(usedCol, p);
	}
	*/

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
/*
	parser_.ClearVar();
	parserX_.ClearVar();

	// add all of the raw data column names as allowed variables in the expression parser
		// the values from the columns will be buffered in varStorage_ for evaluation.
	QStringList ycols = dataTree()->yColumnNames();
	QStringList ySubs;
	QString baseName;
	for(int i=0; i<dataTree()->ySubtreeNames().count(); i++){
		baseName = dataTree()->ySubtreeNames().at(i) + ".";
		ySubs << baseName + dataTree()->prototype(i)->xName();
		for(int j=0; j<dataTree()->prototype(i)->yColumnNames().count(); j++)
			ySubs << baseName + dataTree()->prototype(i)->yColumnNames().at(j);
	}
	ycols.append(ySubs);
	varStorage_.resize(ycols.count()+1);

	try {
		std::cout << "Allowable variables are: \n";
		parser_.DefineVar(dataTree()->xName().toStdString(), varStorage_.data() );
		std::cout << dataTree()->xName().toStdString() << "\n";
		parserX_.DefineVar(dataTree()->xName().toStdString(), varStorage_.data() );
		for(int i=0; i<ycols.count(); i++) {
			parser_.DefineVar(ycols.at(i).toStdString(), varStorage_.data()+1+i);
			std::cout << ycols.at(i).toStdString() << "\n";
			parserX_.DefineVar(ycols.at(i).toStdString(), varStorage_.data()+1+i);
		}
		std::cout << "\n";
		fflush(stdout);
	}
	catch(mu::Parser::exception_type &e) {
		QString explanation = QString("AMChannel (setting variables): %1.").arg(QString::fromStdString(e.GetMsg()));
		AMErrorMon::report(AMErrorReport(this, AMErrorReport::Debug, e.GetCode(), explanation));
		return false;
	}
*/
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
//	qDebug() << "\nStart searching min";
	for(int i=1; (unsigned)i<count(); i++)
		if(value(i) < value(min_))
			min_ = i;
//	qDebug() << "Done searching min\n";
}
void AMChannel::searchMax() const {
	max_ = 0;
//	qDebug() << "\nStart searching max";
	for(int i=1; (unsigned)i<count(); i++)
		if(value(i)>value(max_))
			max_ = i;
//	qDebug() << "Done searching max\n";
}
void AMChannel::searchMinX() const {
	minX_ = 0;
//	qDebug() << "\nStart searching minX";
	for(int i=1; (unsigned)i<count(); i++)
		if(x(i)<x(minX_))
			minX_ = i;
//	qDebug() << "Done searching minX\n";
}
void AMChannel::searchMaxX() const {
	maxX_ = 0;
//	qDebug() << "\nStart searching maxX";
	for(int i=1; (unsigned)i<count(); i++)
		if(x(i)>x(maxX_))
			maxX_ = i;
//	qDebug() << "Done searching maxX\n";
}

/// We observe AMDataTrees.  They Emit(3, "columnChanged", columnIndex) whenever values in a column change.
void AMChannel::onObservableChanged(AMObservable* source, int code, const char* msg, int colIndex) {
	Q_UNUSED(source)
	Q_UNUSED(msg)

//	qDebug() << "dataTree emits " << code << QString(msg) << colIndex << " for " << usedColumnIndices_ << " and defaultX " << defaultX_;
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
