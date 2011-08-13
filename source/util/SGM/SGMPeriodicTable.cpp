/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "SGMPeriodicTable.h"

#include <QDebug>

//Singleton instance
SGMPeriodicTable* SGMPeriodicTable::instance_ = 0;

SGMPeriodicTable::SGMPeriodicTable(QObject *parent) :
		QObject(parent)
{
	//CARBON INFO
	SGMElementInfo *tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("C"), this);
	SGMEdgeInfo carbonK(AMPeriodicTable::table()->elementBySymbol("C")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(280.0, -397720, -145629, 286.63, 0),
												      SGMEnergyPosition(295.0, -377497, -140470, 200.46, 0),
												      SGMEnergyPosition(320.0, -348005, -132290, 100.54, 0)));
	tmpElementInfo->addEdgeInfo(carbonK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 24000, 5.0, 200, 4000 ));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 4800, 20.0, 200, 820));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//NITROGEN INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("N"), this);
	SGMEdgeInfo nitrogenK(AMPeriodicTable::table()->elementBySymbol("N")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(400.0, -278404, -109034, 1.37942, 0),
												      SGMEnergyPosition(415.0, -268341, -105051, 5.99793, 0),
												      SGMEnergyPosition(430.0, -258981, -101191, 15.5151, 0)));
	tmpElementInfo->addEdgeInfo(nitrogenK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 10000, 5.0, 200, 1500));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 1800, 20.0, 800, 400));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//OXYGEN INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("O"), this);
	SGMEdgeInfo oxygenK(AMPeriodicTable::table()->elementBySymbol("O")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(530.0, -210116, -77335, 166.586, 0),
												      SGMEnergyPosition(545.0, -204333, -73988, 198.591, 0),
												      SGMEnergyPosition(560.0, -198860, -70692, 232.356, 0)));
	tmpElementInfo->addEdgeInfo(oxygenK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 10000, 5.0, 200, 1600));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 1100, 20.0, 800, 330));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//COPPER INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Cu"), this);
	SGMEdgeInfo copperL3(AMPeriodicTable::table()->elementBySymbol("Cu")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(925.0, -220063, 1414, 218.584, 1),
													 SGMEnergyPosition(935.0, -217710, 3318, 230.292, 1),
													 SGMEnergyPosition(945.0, -215406, 5232, 242.299, 1)));
	tmpElementInfo->addEdgeInfo(copperL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 3000, 5.0, 200, 1000));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 450, 20.0, 800, 235));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//ZINC INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Zn"), this);
	SGMEdgeInfo zincL3(AMPeriodicTable::table()->elementBySymbol("Zn")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(1010.0, -201543, 17819, 326.886, 1),
													 SGMEnergyPosition(1025.0, -198594, 20749, 347.853, 1),
													 SGMEnergyPosition(1040.0, -195729, 23697, 369.307, 1)));
	tmpElementInfo->addEdgeInfo(zincL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 3500, 5.0, 200, 1800));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 550, 20.0, 800, 300));
	SGMEdgeInfo zincL2(AMPeriodicTable::table()->elementBySymbol("Zn")->L2Edge(), SGMScanRangeInfo(SGMEnergyPosition(1025.0, -198594, 20749, 347.853, 1),
													 SGMEnergyPosition(1040.0, -195729, 23697, 369.307, 1),
													 SGMEnergyPosition(1060.0, -192036, 27686, 398.628, 1)));
	tmpElementInfo->addEdgeInfo(zincL2);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									5.0, 3800, 5.0, 200, 1400));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									20.0, 600, 20.0, 800, 350));

	SGMStandardScanInfo zincL("L Edge", SGMScanRangeInfo(SGMEnergyPosition(1010.0, -201543, 17796, 326.886),
											SGMEnergyPosition(1035.0, -196675, 22723, 362.103, 1),
											SGMEnergyPosition(1060.0, -192036, 27678, 398.628, 1)));
	tmpElementInfo->addStandardScanInfo(zincL);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									5.0, 6000, 5.0, 200, 3500));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									20.0, 850, 20.0, 800, 600));

	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());
}

SGMElementInfo* SGMPeriodicTable::elementInfoByName(const QString &elementName) const{
	if(sgmPeriodicTableInfo_.contains(AMPeriodicTable::table()->elementByName(elementName)))
		return sgmPeriodicTableInfo_.at(sgmPeriodicTableInfo_.indexOfKey(AMPeriodicTable::table()->elementByName(elementName)));
	else
		return 0; //NULL
}

QStringList SGMPeriodicTable::fastScanPresetsStrings() const{
	QStringList retVal;
	QString tmpStr;
	SGMElementInfo *tmpElementInfo;
	for(int x = 0; x < sgmPeriodicTableInfo_.count(); x++){
		tmpElementInfo = sgmPeriodicTableInfo_.at(x);
		for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
			retVal << tmpElementInfo->availableFastScanParameters().at(y)->element() + " " + tmpElementInfo->availableFastScanParameters().at(y)->edge() + " " + tmpStr.setNum(tmpElementInfo->availableFastScanParameters().at(y)->runSeconds());
	}
	return retVal;
}

QList<SGMFastScanParameters*> SGMPeriodicTable::fastScanPresets() const{
	QList<SGMFastScanParameters*> retVal;
	SGMElementInfo *tmpElementInfo;
	for(int x = 0; x < sgmPeriodicTableInfo_.count(); x++){
		tmpElementInfo = sgmPeriodicTableInfo_.at(x);
		for(int y = 0; y < tmpElementInfo->availableFastScanParameters().count(); y++)
			retVal << tmpElementInfo->availableFastScanParameters().at(y);
	}
	return retVal;
}
