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


#include "SGMLookupTableCoordinator.h"
#include "beamline/AMPVControl.h"
#include <QFile>

 SGMLookupTableCoordinator::~SGMLookupTableCoordinator(){}
SGMLookupTableCoordinator::SGMLookupTableCoordinator(const QString &inputControlName, const QString &outputControlName, const QString &linkageControlName, const QString &loadLookupTableControlName, QObject *parent) :
	QObject(parent)
{
	Q_UNUSED(linkageControlName)
	Q_UNUSED(loadLookupTableControlName)
	connectedOnce_ = false;
	inputControl_ = new AMPVControl("inputControl", inputControlName, inputControlName, QString() , this, 0.01);
	outputControl_ = new AMPVControl("outputControl", outputControlName, outputControlName, QString(), this, 0.01);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(inputControl_);
	allControls_->addControl(outputControl_);

	connect(inputControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputControlChanged(double)));

	/*
	linkageActive_ = false;

	lookupTableFullFileName_ = "/home/sgm/beamline/programming/acquaman/source/application/SGMLookupTableCoordinator/LookupValues.txt";

	inputControl_ = new AMReadOnlyPVControl("LookupInputControl", inputControlName, this);
	inputControl_->setTolerance(0.01);
	outputControl_ = new AMPVControl("LookupOutputControl", outputControlName, outputControlName, "", this, 0.01);
	linkageControl_ = new AMReadOnlyPVControl("LookupLinkageControl", linkageControlName, this);
	loadLookupTableControl_ = new AMPVControl("LookupLoadTableControl", loadLookupTableControlName, loadLookupTableControlName, "", this, 0.1);

	allControls_ = new AMControlSet(this);
	allControls_->addControl(inputControl_);
	allControls_->addControl(outputControl_);
	allControls_->addControl(linkageControl_);
	allControls_->addControl(loadLookupTableControl_);

	connect(inputControl_, SIGNAL(valueChanged(double)), this, SLOT(onInputControlChanged(double)));
	connect(linkageControl_, SIGNAL(valueChanged(double)), this, SLOT(onLinkageControlChanged(double)));
	connect(loadLookupTableControl_, SIGNAL(valueChanged(double)), this, SLOT(onLoadLookupTableControlChanged(double)));
	*/
	connect(allControls_, SIGNAL(connected(bool)), this, SLOT(onAllControlsConnected(bool)));
}

void SGMLookupTableCoordinator::onInputControlChanged(double inputValue){
	if(!allControls_->isConnected() || !connectedOnce_)
		return;
	if(!inputControl_->withinTolerance(outputControl_->value())){
		qDebug() << "Forward the value " << inputValue;
		outputControl_->move(inputValue);
	}
	else
		qDebug() << "Do not forward value " << inputValue << " vs " << outputControl_->value();
}


/*
void SGMLookupTableCoordinator::onInputControlChanged(double inputValue){
	if(!connectedOnce_)
		return;

	qDebug() << "New input value is " << inputValue;
	internalDoConversion();
}

void SGMLookupTableCoordinator::onLinkageControlChanged(double linkageValue){
	if(!connectedOnce_)
		return;

	qDebug() << "New linkage value is " << linkageValue;
	if(linkageControl_->withinTolerance(1.0))
		linkageActive_ = true;
	else
		linkageActive_ = false;
}

void SGMLookupTableCoordinator::onLoadLookupTableControlChanged(double loadLookupValue){
	if(!connectedOnce_)
		return;

	qDebug() << "New load lookup value is " << loadLookupValue;
	if(loadLookupTableControl_->withinTolerance(1.0)){
		internalLoadLookupTable();
		internalDoConversion();
	}
}
*/

void SGMLookupTableCoordinator::onAllControlsConnected(bool connected){
	qDebug() << "Checking all SGM Lookups";
	if(connected){
		connectedOnce_ = true;
		qDebug() << "All SGM Lookups connected";
		/*
		internalLoadLookupTable();
		onLinkageControlChanged(linkageControl_->value());
		internalDoConversion();
		*/
	}
}

/*
void SGMLookupTableCoordinator::internalDoConversion(){
	if(!linkageActive_)
		return;

	qDebug() << "Do the conversion";

	double inputValue = inputControl_->value();
	double outputValue;
	double x1, y1, x2, y2;
	double m, b;
	QMap<double, double>::const_iterator lookupUpperBound = lookupTable_.find(inputValue);
	if(lookupUpperBound != lookupTable_.constEnd()){ //Cheat first and check for a perfect match
		qDebug() << "Found an exact match as " << lookupUpperBound.key() << lookupUpperBound.value();
		outputControl_->move(lookupUpperBound.value());
		return;
	}

	lookupUpperBound = lookupTable_.upperBound(inputValue);
	if(lookupUpperBound == lookupTable_.constEnd()){//Past end of lookup table
		lookupUpperBound--;
		x2 = lookupUpperBound.key();
		y2 = lookupUpperBound.value();
		lookupUpperBound--;
		x1 = lookupUpperBound.key();
		y1 = lookupUpperBound.value();
	}
	else if(lookupUpperBound == lookupTable_.constBegin()){//Before start of lookup table
		x1 = lookupUpperBound.key();
		y1 = lookupUpperBound.value();
		lookupUpperBound++;
		x2 = lookupUpperBound.key();
		y2 = lookupUpperBound.value();
	}
	else{
		x2 = lookupUpperBound.key();
		y2 = lookupUpperBound.value();
		lookupUpperBound--;
		x1 = lookupUpperBound.key();
		y1 = lookupUpperBound.value();
	}

	qDebug() << "For input value " << inputValue << " using look ups as (" << x1 << "," << y1 << ") (" << x2 << "," << y2 << ")";
	m = (y2-y1)/(x2-x1);
	b = y1 - m*x1;

	qDebug() << "m =" << m << "b =" << b;

	outputValue = m*inputValue + b;
	qDebug() << "Lookup says " << inputValue << ":" << outputValue;

	outputControl_->move(outputValue);
}

void SGMLookupTableCoordinator::internalLoadLookupTable(){
	QFile lookupTableFile(lookupTableFullFileName_);
	if (!lookupTableFile.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	lookupTable_.clear();

	QTextStream in(&lookupTableFile);
	QString line = in.readLine();
	QStringList lineValues;
	double inputValue, outputValue;
	while (!line.isNull()) {
		lineValues = line.split(" ");
		inputValue = lineValues.at(0).toDouble();
		outputValue = lineValues.at(1).toDouble();
		lookupTable_.insert(inputValue, outputValue);
		line = in.readLine();
	}

	QMapIterator<double, double> i(lookupTable_);
	while (i.hasNext()) {
		i.next();
		qDebug() << i.key() << ": " << i.value();
	}

	loadLookupTableControl_->move(0);
}
*/
