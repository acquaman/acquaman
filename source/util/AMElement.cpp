/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "AMElement.h"

#include <QStringBuilder>

AMElement::AMElement(QString name, QString symbol, QString atomicNumber, QStringList edgeList, QStringList emissionLineList, QObject *parent) :
	QObject(parent)
{
	name_ = name;
	symbol_ = symbol;
	atomicNumber_ = atomicNumber.toInt();
	edgeSize_ = edgeList.size();
	emissionLineSize_ = emissionLineList.size();

	switch(edgeSize_){
	case 24: edges_.prepend(AMAbsorptionEdge(symbol % " P3", edgeList.at(23)));
	case 23: edges_.prepend(AMAbsorptionEdge(symbol % " P2", edgeList.at(22)));
	case 22: edges_.prepend(AMAbsorptionEdge(symbol % " P1", edgeList.at(21)));
	case 21: edges_.prepend(AMAbsorptionEdge(symbol % " O5", edgeList.at(20)));
	case 20: edges_.prepend(AMAbsorptionEdge(symbol % " O4", edgeList.at(19)));
	case 19: edges_.prepend(AMAbsorptionEdge(symbol % " O3", edgeList.at(18)));
	case 18: edges_.prepend(AMAbsorptionEdge(symbol % " O2", edgeList.at(17)));
	case 17: edges_.prepend(AMAbsorptionEdge(symbol % " O1", edgeList.at(16)));
	case 16: edges_.prepend(AMAbsorptionEdge(symbol % " N7", edgeList.at(15)));
	case 15: edges_.prepend(AMAbsorptionEdge(symbol % " N6", edgeList.at(14)));
	case 14: edges_.prepend(AMAbsorptionEdge(symbol % " N5", edgeList.at(13)));
	case 13: edges_.prepend(AMAbsorptionEdge(symbol % " N4", edgeList.at(12)));
	case 12: edges_.prepend(AMAbsorptionEdge(symbol % " N3", edgeList.at(11)));
	case 11: edges_.prepend(AMAbsorptionEdge(symbol % " N2", edgeList.at(10)));
	case 10: edges_.prepend(AMAbsorptionEdge(symbol % " N1", edgeList.at(9)));
	case 9: edges_.prepend(AMAbsorptionEdge(symbol % " M5", edgeList.at(8)));
	case 8: edges_.prepend(AMAbsorptionEdge(symbol % " M4", edgeList.at(7)));
	case 7: edges_.prepend(AMAbsorptionEdge(symbol % " M3", edgeList.at(6)));
	case 6: edges_.prepend(AMAbsorptionEdge(symbol % " M2", edgeList.at(5)));
	case 5: edges_.prepend(AMAbsorptionEdge(symbol % " M1", edgeList.at(4)));
	case 4: edges_.prepend(AMAbsorptionEdge(symbol % " L3", edgeList.at(3)));
	case 3: edges_.prepend(AMAbsorptionEdge(symbol % " L2", edgeList.at(2)));
	case 2: edges_.prepend(AMAbsorptionEdge(symbol % " L1", edgeList.at(1)));
	case 1: edges_.prepend(AMAbsorptionEdge(symbol % " K", edgeList.at(0)));
	}

	switch(emissionLineSize_){
	case 9: emissionLines_.prepend(qMakePair(QString::fromUtf8("Mα1"), emissionLineList.at(8)));
	case 8: emissionLines_.prepend(qMakePair(QString::fromUtf8("Lγ1"), emissionLineList.at(7)));
	case 7: emissionLines_.prepend(qMakePair(QString::fromUtf8("Lβ2"), emissionLineList.at(6)));
	case 6: emissionLines_.prepend(qMakePair(QString::fromUtf8("Lβ1"), emissionLineList.at(5)));
	case 5: emissionLines_.prepend(qMakePair(QString::fromUtf8("Lα2"), emissionLineList.at(4)));
	case 4: emissionLines_.prepend(qMakePair(QString::fromUtf8("Lα1"), emissionLineList.at(3)));
	case 3: emissionLines_.prepend(qMakePair(QString::fromUtf8("Kβ1"), emissionLineList.at(2)));
	case 2: emissionLines_.prepend(qMakePair(QString::fromUtf8("Kα2"), emissionLineList.at(1)));
	case 1: emissionLines_.prepend(qMakePair(QString::fromUtf8("Kα1"), emissionLineList.at(0)));
	}
}
