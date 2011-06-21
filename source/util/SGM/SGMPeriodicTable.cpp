#include "SGMPeriodicTable.h"

//Singleton instance
SGMPeriodicTable* SGMPeriodicTable::instance_ = 0;

SGMPeriodicTable::SGMPeriodicTable(QObject *parent) :
		QObject(parent)
{
	SGMElementInfo *tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("C"),
							    SGMEnergyPosition(280.0, -397720, -145629, 286.63),
							    SGMEnergyPosition(295.0, -377497, -140470, 200.46),
							    SGMEnergyPosition(320.0, -348005, -132290, 100.54),
							    this);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Carbon", 5.0, 280.0, 295.0, 320.0, 24000, 24000, 24000, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 4000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("N"),
					    SGMEnergyPosition(400.0, -278404, -109034, 1.37942),
					    SGMEnergyPosition(415.0, -268341, -105051, 5.99793),
					    SGMEnergyPosition(430.0, -258981, -101191, 15.5151),
					    this);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Nitrogen", 5.0, 400.0, 415.0, 430.0, 10000, 10000, 10000, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1500, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Nitrogen", 20.0, 400.0, 415.0, 430.0, 1800, 1800, 1800, 20.0, 800, tmpElementInfo->standardStart().undulatorStepSetpoint(), 400, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("O"),
					    SGMEnergyPosition(530.0, -210116, -77335, 166.586),
					    SGMEnergyPosition(545.0, -204333, -73988, 198.591),
					    SGMEnergyPosition(560.0, -198860, -70692, 232.356),
					    this);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Oxygen", 5.0, 530.0, 545.0, 560.0, 10000, 10000, 10000, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1600, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Oxygen", 20.0, 530.0, 545.0, 560.0, 1100, 1100, 1100, 20.0, 800, tmpElementInfo->standardStart().undulatorStepSetpoint(), 330, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Cu"),
					    SGMEnergyPosition(925.0, -220063, 1414, 218.584),
					    SGMEnergyPosition(935.0, -217710, 3318, 230.292),
					    SGMEnergyPosition(945.0, -215406, 5232, 242.299),
					    this);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Copper", 5.0, 925.0, 935.0, 945.0, 3000, 3000, 3000, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Copper", 20.0, 925.0, 935.0, 945.0, 450, 450, 450, 20.0, 800, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Zn"),
					    SGMEnergyPosition(1010.0, -201543, 17819, 326.886),
					    SGMEnergyPosition(1035.0, -196675, 22723, 362.103),
					    SGMEnergyPosition(1060.0, -192036, 27686, 398.628),
					    this);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Zinc 3/2", 5.0, 1010.0, 1025.0, 1040.0, 3200, 3200, 3200, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Zinc 1/2", 5.0, 1025.0, 1040.0, 1055.0, 3350, 3350, 3350, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Zinc L", 5.0, 1010.0, 1035.0, 1060.0, 6000, 6000, 6000, 5.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters("Zinc L", 20.0, 1010.0, 1025.0, 1040.0, 860, 860, 860, 20.0, 200, tmpElementInfo->standardStart().undulatorStepSetpoint(), 1000, tmpElementInfo->standardEnd().undulatorStepSetpoint()-tmpElementInfo->standardStart().undulatorStepSetpoint(), this));
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
			retVal << tmpElementInfo->availableFastScanParameters().at(y)->element() + " " + tmpStr.setNum(tmpElementInfo->availableFastScanParameters().at(y)->runSeconds());
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
