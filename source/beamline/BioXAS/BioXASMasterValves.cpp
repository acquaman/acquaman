#include "BioXASMasterValves.h"
#include "beamline/AMControlSet.h"

BioXASMasterValves::BioXASMasterValves(QObject *parent) :
	BioXASValves("BioXASMasterValves", parent)
{

}

BioXASMasterValves::~BioXASMasterValves()
{

}

void BioXASMasterValves::addValves(BioXASValves *newValves)
{
	if (newValves) {

		QList<AMControl*> valvesList = newValves->valvesList();

		foreach (AMControl *valve, valvesList)
			addValve(valve);
	}
}

void BioXASMasterValves::removeValves(BioXASValves *valves)
{
	if (valves) {

		QList<AMControl*> valves = valves->valvesList();

		foreach (AMControl *valve, valves)
			removeValve(valve);
	}
}

