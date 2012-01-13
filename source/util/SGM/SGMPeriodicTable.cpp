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
	SGMEdgeInfo carbonK(AMPeriodicTable::table()->elementBySymbol("C")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(270.0, -397720, -149991, 286.63, 0),
												      SGMEnergyPosition(295.0, -377497, -140470, 200.46, 0),
												      SGMEnergyPosition(320.0, -348005, -133061, 100.54, 0)));
	tmpElementInfo->addEdgeInfo(carbonK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 40000, 5.0, 200, 6000 ));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 6200, 20.0, 200, 970));
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

	//CALCIUM INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Ca"), this);
	SGMEdgeInfo calciumL3(AMPeriodicTable::table()->elementBySymbol("Ca")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(345.0, -322785, -125202, 1.0, 1),
													 SGMEnergyPosition(350.0, -318716, -123716, 1.0, 1),
													 SGMEnergyPosition(355.0, -313695, -122224, 1.0, 1)));
	tmpElementInfo->addEdgeInfo(calciumL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 5500, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 950, 20.0, 800, 170));
	SGMEdgeInfo calciumL2(AMPeriodicTable::table()->elementBySymbol("Ca")->L2Edge(), SGMScanRangeInfo(SGMEnergyPosition(355.0, -313695, -122224, 1.0, 1),
													 SGMEnergyPosition(360.0, -309338, -120771, 1.0, 1),
													 SGMEnergyPosition(365.0, -305100, -119343, 1.0, 1)));
	tmpElementInfo->addEdgeInfo(calciumL2);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									5.0, 6000, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									20.0, 950, 20.0, 800, 170));

	SGMStandardScanInfo calciumL("L Edge", SGMScanRangeInfo(SGMEnergyPosition(345.0, -322785, -125202, 1.0, 1),
											SGMEnergyPosition(355.0, -313695, -122224, 1.0, 1),
											SGMEnergyPosition(365.0, -305100, -119343, 1.0, 1)));
	tmpElementInfo->addStandardScanInfo(calciumL);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									5.0, 8500, 5.0, 200, 1800));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									20.0, 1800, 20.0, 800, 300));

	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//TITANIUM INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Ti"), this);
	SGMEdgeInfo titaniumL3(AMPeriodicTable::table()->elementBySymbol("Ti")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(450.0, -247469, -96777, 1.0, 0),
													 SGMEnergyPosition(457.5, -243412, -94931, 1.0, 0),
													 SGMEnergyPosition(465.0, -239490, -93072, 1.0, 0)));
	tmpElementInfo->addEdgeInfo(titaniumL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 5500, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 950, 20.0, 800, 170));
	SGMEdgeInfo titaniumL2(AMPeriodicTable::table()->elementBySymbol("Ti")->L2Edge(), SGMScanRangeInfo(SGMEnergyPosition(465.0, -239490, -93072, 1.0, 0),
													 SGMEnergyPosition(472.5, -235684, -91249, 9.944, 0),
													 SGMEnergyPosition(480.0, -232004, -89479, 21.226, 0)));
	tmpElementInfo->addEdgeInfo(titaniumL2);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									5.0, 6000, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									20.0, 950, 20.0, 800, 170));

	SGMStandardScanInfo titaniumL("L Edge", SGMScanRangeInfo(SGMEnergyPosition(450.0, -247469, -96777, 1.0, 0),
											SGMEnergyPosition(465.0, -239490, -93072, 1.0, 0),
											SGMEnergyPosition(480.0, -232004, -89479, 21.226, 0)));
	tmpElementInfo->addStandardScanInfo(titaniumL);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									5.0, 8500, 5.0, 200, 1800));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									20.0, 1800, 20.0, 800, 300));

	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//CHROMIUM INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Cr"), this);
	SGMEdgeInfo chromiumL3(AMPeriodicTable::table()->elementBySymbol("Cr")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(565.0, -197101, -70227, 189.973, 0),
													 SGMEnergyPosition(575.0, -193671, -68067, 213.716, 0),
													 SGMEnergyPosition(585.0, -190363, -65922, 238.105, 0)));
	tmpElementInfo->addEdgeInfo(chromiumL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 5500, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 950, 20.0, 800, 170));
	SGMEdgeInfo chromiumL2(AMPeriodicTable::table()->elementBySymbol("Cr")->L2Edge(), SGMScanRangeInfo(SGMEnergyPosition(585.0, -190363, -65922, 238.105, 0),
													 SGMEnergyPosition(595.0, -187152, -63810, 263.101, 0),
													 SGMEnergyPosition(605.0, -184068, -61689, 288.669, 0)));
	tmpElementInfo->addEdgeInfo(chromiumL2);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									5.0, 6000, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									20.0, 950, 20.0, 800, 170));

	SGMStandardScanInfo chromiumL("L Edge", SGMScanRangeInfo(SGMEnergyPosition(565.0, -197101, -70227, 189.973, 0),
											SGMEnergyPosition(585.0, -190363, -65922, 238.105, 0),
											SGMEnergyPosition(605.0, -184068, -61689, 288.669, 0)));
	tmpElementInfo->addStandardScanInfo(chromiumL);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									5.0, 8500, 5.0, 200, 1800));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									20.0, 1800, 20.0, 800, 300));

	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//IRON INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Fe"), this);
	SGMEdgeInfo ironL3(AMPeriodicTable::table()->elementBySymbol("Fe")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(700.0, -159088, -42422, 554.784, 0),
													 SGMEnergyPosition(710.0, -156847, -40435, 584.795, 0),
													 SGMEnergyPosition(720.0, -154667, -38483, 615.12, 0)));
	tmpElementInfo->addEdgeInfo(ironL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 5500, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 950, 20.0, 800, 170));
	SGMEdgeInfo ironL2(AMPeriodicTable::table()->elementBySymbol("Fe")->L2Edge(), SGMScanRangeInfo(SGMEnergyPosition(720.0, -154667, -38483, 615.12, 0),
													 SGMEnergyPosition(730.0, -152550, -36523, 645.744, 0),
													 SGMEnergyPosition(740.0, -150491, -34593, 676.653, 0)));
	tmpElementInfo->addEdgeInfo(ironL2);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									5.0, 6000, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									20.0, 950, 20.0, 800, 170));

	SGMStandardScanInfo ironL("L Edge", SGMScanRangeInfo(SGMEnergyPosition(700.0, -159088, -42422, 554.784, 0),
											SGMEnergyPosition(720.0, -154667, -38483, 615.12, 0),
											SGMEnergyPosition(740.0, -150491, -34593, 676.653, 0)));
	tmpElementInfo->addStandardScanInfo(ironL);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									5.0, 8500, 5.0, 200, 1800));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									20.0, 1800, 20.0, 800, 300));

	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//NICKEL INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Ni"), this);
	SGMEdgeInfo nickelL3(AMPeriodicTable::table()->elementBySymbol("Ni")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(840.0, -242330, -15364, 78.917, 1),
													 SGMEnergyPosition(852.5, -238779, -13006, 89.776, 1),
													 SGMEnergyPosition(865.0, -235326, -10614, 101.285, 1)));
	tmpElementInfo->addEdgeInfo(nickelL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 5500, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 950, 20.0, 800, 170));
	SGMEdgeInfo nickelL2(AMPeriodicTable::table()->elementBySymbol("Ni")->L2Edge(), SGMScanRangeInfo(SGMEnergyPosition(865.0, -235326, -10614, 101.285, 1),
													 SGMEnergyPosition(877.5, -231977, -8246, 113.412, 1),
													 SGMEnergyPosition(890.0, -228716, -5858, 126.123, 1)));
	tmpElementInfo->addEdgeInfo(nickelL2);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									5.0, 6000, 5.0, 200, 750));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L2")),
									20.0, 950, 20.0, 800, 170));

	SGMStandardScanInfo nickelL("L Edge", SGMScanRangeInfo(SGMEnergyPosition(840.0, -242330, -15364, 78.917, 1),
											SGMEnergyPosition(865.0, -235326, -10614, 101.285, 1),
											SGMEnergyPosition(890.0, -228716, -5858, 126.123, 1)));
	tmpElementInfo->addStandardScanInfo(nickelL);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									5.0, 8500, 5.0, 200, 1800));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmStandardScanInfos().at(tmpElementInfo->sgmStandardScanInfos().indexOfKey("L Edge")),
									20.0, 1800, 20.0, 800, 300));

	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//COPPER INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Cu"), this);
	SGMEdgeInfo copperL3(AMPeriodicTable::table()->elementBySymbol("Cu")->L3Edge(), SGMScanRangeInfo(SGMEnergyPosition(920.0, -221259, 1300, 218.584, 1),
													 SGMEnergyPosition(950.0, -214272, 3318, 230.292, 1),
													 SGMEnergyPosition(970.0, -209854, 9900, 242.299, 1)));
	tmpElementInfo->addEdgeInfo(copperL3);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									5.0, 3000, 5.0, 200, 1000));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("L3")),
									20.0, 1100, 20.0, 800, 510));
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

	//SODIUM INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Na"), this);
	SGMEdgeInfo sodiumK(AMPeriodicTable::table()->elementBySymbol("Na")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(1050.0, -193868, 25085, 329.87, 1),
												      SGMEnergyPosition(1080.0, -188479, 31109, 374.72, 1),
												      SGMEnergyPosition(1110.0, -183388, 37235, 421.201, 1)));
	tmpElementInfo->addEdgeInfo(sodiumK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 24000, 5.0, 200, 4000 ));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 4800, 20.0, 200, 820));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//MAGNESIUM INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Mg"), this);
	SGMEdgeInfo magnesiumK(AMPeriodicTable::table()->elementBySymbol("Mg")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(1290.0, -249736, -101803, 91.21, 2),
												      SGMEnergyPosition(1315.0, -239101, -99682, 105.38, 2),
												      SGMEnergyPosition(1340.0, -234638, -97582, 120.65, 2)));
	tmpElementInfo->addEdgeInfo(magnesiumK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 24000, 5.0, 200, 4000 ));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 4800, 20.0, 200, 820));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//ALUMINUM INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Al"), this);
	SGMEdgeInfo aluminumK(AMPeriodicTable::table()->elementBySymbol("Al")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(1550.0, -202852, -80942, 283.82, 2),
												      SGMEnergyPosition(1580.0, -198998, -78694, 311.23, 2),
												      SGMEnergyPosition(1610.0, -195288, -76434, 339.47, 2)));
	tmpElementInfo->addEdgeInfo(aluminumK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 24000, 5.0, 200, 4000 ));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 4800, 20.0, 200, 820));
	sgmPeriodicTableInfo_.append(tmpElementInfo, tmpElementInfo->element());

	//SILICON INFO
	tmpElementInfo = new SGMElementInfo(AMPeriodicTable::table()->elementBySymbol("Si"), this);
	SGMEdgeInfo siliconK(AMPeriodicTable::table()->elementBySymbol("Si")->KEdge(), SGMScanRangeInfo(SGMEnergyPosition(1830.0, -171811, -60656, 567.57, 2),
												      SGMEnergyPosition(1860.0, -169041, -58574, 601.08, 2),
												      SGMEnergyPosition(1890.0, -166359, -56493, 635.07, 2)));
	tmpElementInfo->addEdgeInfo(siliconK);
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									5.0, 24000, 5.0, 200, 4000 ));
	tmpElementInfo->addFastScanParameters(new SGMFastScanParameters(tmpElementInfo->element()->name(),
									tmpElementInfo->sgmEdgeInfos().at(tmpElementInfo->sgmEdgeInfos().indexOfKey("K")),
									20.0, 4800, 20.0, 200, 820));
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
