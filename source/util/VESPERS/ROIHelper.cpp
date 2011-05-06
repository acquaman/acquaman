#include "ROIHelper.h"
#include "beamline/AMProcessVariable.h"

ROIHelper::ROIHelper(QObject *parent) :
	QObject(parent)
{
	AMProcessVariable *raw;
	AMProcessVariable *sum;
	AMProcessVariable *corr;

	for (int i = 0; i < 32; i++){

		raw = new AMProcessVariable("dxp1607-B21-04:mca1.R"+QString::number(i)+"NM", true, this);
		sum = new AMProcessVariable("dxp1607-B21-04:mcaSum.R"+QString::number(i)+"NM", true, this);
		corr = new AMProcessVariable("dxp1607-B21-04:mcaCorrected.R"+QString::number(i)+"NM", true, this);
		connect(raw, SIGNAL(valueChanged(QString)), sum, SLOT(setValue(QString)));
		connect(raw, SIGNAL(valueChanged(QString)), corr, SLOT(setValue(QString)));
	}

	for (int i = 0; i < 32; i++){

		raw = new AMProcessVariable("dxp1607-B21-04:mca1.R"+QString::number(i)+"LO", true, this);
		sum = new AMProcessVariable("dxp1607-B21-04:mcaSum.R"+QString::number(i)+"LO", true, this);
		corr = new AMProcessVariable("dxp1607-B21-04:mcaCorrected.R"+QString::number(i)+"LO", true, this);
		connect(raw, SIGNAL(valueChanged(int)), sum, SLOT(setValue(int)));
		connect(raw, SIGNAL(valueChanged(int)), corr, SLOT(setValue(int)));
	}

	for (int i = 0; i < 32; i++){

		raw = new AMProcessVariable("dxp1607-B21-04:mca1.R"+QString::number(i)+"HI", true, this);
		sum = new AMProcessVariable("dxp1607-B21-04:mcaSum.R"+QString::number(i)+"HI", true, this);
		corr = new AMProcessVariable("dxp1607-B21-04:mcaCorrected.R"+QString::number(i)+"HI", true, this);
		connect(raw, SIGNAL(valueChanged(int)), sum, SLOT(setValue(int)));
		connect(raw, SIGNAL(valueChanged(int)), corr, SLOT(setValue(int)));
	}
}
