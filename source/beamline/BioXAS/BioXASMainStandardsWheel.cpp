#include "BioXASMainStandardsWheel.h"

BioXASMainStandardsWheel::BioXASMainStandardsWheel(QObject *parent) :
	CLSStandardsWheel("MainStandardsWheel", "SMTR1607-7-I21-23", parent)
{
	setName(0, "Cr");
	setName(1, "Mn");
	setName(2, "Fe");
	setName(3, "Co");
	setName(4, "Ni");
	setName(5, "Cu");
	setName(6, "Zn");
	setName(7, "As");
	setName(8, "Se");
	setName(9, "Hg");
	setName(10, "Mo");
	setName(11, "None");
}

BioXASMainStandardsWheel::~BioXASMainStandardsWheel()
{

}
