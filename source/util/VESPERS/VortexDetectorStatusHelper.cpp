#include "VortexDetectorStatusHelper.h"

VortexDetectorStatusHelper::VortexDetectorStatusHelper(QObject *parent) :
	QObject(parent)
{
	singleMCAto_ = new AMProcessVariable("IOC1607-004:mca1Read.SCAN", true, this);
	singleMCAfrom_ = new AMProcessVariable("IOC1607-004:mca1.SCAN", true, this);
	connect(singleMCAfrom_, SIGNAL(valueChanged(int)), singleMCAto_, SLOT(setValue(int)));
	connect(singleMCAto_, SIGNAL(valueChanged(int)), this, SLOT(setSingleMCA(int)));

	singleDXPto_ = new AMProcessVariable("IOC1607-004:dxp1:ReadParams.SCAN", true, this);
	singleDXPfrom_ = new AMProcessVariable("IOC1607-004:dxp1.SCAN", true, this);
	connect(singleDXPfrom_, SIGNAL(valueChanged(int)), singleDXPto_, SLOT(setValue(int)));
	connect(singleDXPto_, SIGNAL(valueChanged(int)), this, SLOT(setSingleDXP(int)));

	fourMCAto_ = new AMProcessVariable("dxp1607-B21-04:ReadAll.SCAN", true, this);
	fourMCAfrom_ = new AMProcessVariable("dxp1607-B21-04:mca1.SCAN", true, this);
	connect(fourMCAfrom_, SIGNAL(valueChanged(int)), fourMCAto_, SLOT(setValue(int)));
	connect(fourMCAto_, SIGNAL(valueChanged(int)), this, SLOT(setFourMCA(int)));

	fourDXPto_ = new AMProcessVariable("dxp1607-B21-04:ReadDXPs.SCAN", true, this);
	fourDXPfrom_ = new AMProcessVariable("dxp1607-B21-04:dxp1.SCAN", true, this);
	connect(fourDXPfrom_, SIGNAL(valueChanged(int)), fourDXPto_, SLOT(setValue(int)));
	connect(fourDXPto_, SIGNAL(valueChanged(int)), this, SLOT(setFourDXP(int)));
}
