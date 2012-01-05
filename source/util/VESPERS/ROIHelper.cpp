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
		raw->disablePutCallbackMode(true);
		sum->disablePutCallbackMode(true);
		corr->disablePutCallbackMode(true);
		connect(raw, SIGNAL(valueChanged(QString)), sum, SLOT(setValue(QString)));
		connect(raw, SIGNAL(valueChanged(QString)), corr, SLOT(setValue(QString)));
	}

	for (int i = 0; i < 32; i++){

		raw = new AMProcessVariable("dxp1607-B21-04:mca1.R"+QString::number(i)+"LO", true, this);
		sum = new AMProcessVariable("dxp1607-B21-04:mcaSum.R"+QString::number(i)+"LO", true, this);
		corr = new AMProcessVariable("dxp1607-B21-04:mcaCorrected.R"+QString::number(i)+"LO", true, this);
		raw->disablePutCallbackMode(true);
		sum->disablePutCallbackMode(true);
		corr->disablePutCallbackMode(true);
		connect(raw, SIGNAL(valueChanged(int)), sum, SLOT(setValue(int)));
		connect(raw, SIGNAL(valueChanged(int)), corr, SLOT(setValue(int)));
	}

	for (int i = 0; i < 32; i++){

		raw = new AMProcessVariable("dxp1607-B21-04:mca1.R"+QString::number(i)+"HI", true, this);
		sum = new AMProcessVariable("dxp1607-B21-04:mcaSum.R"+QString::number(i)+"HI", true, this);
		corr = new AMProcessVariable("dxp1607-B21-04:mcaCorrected.R"+QString::number(i)+"HI", true, this);
		raw->disablePutCallbackMode(true);
		sum->disablePutCallbackMode(true);
		corr->disablePutCallbackMode(true);
		connect(raw, SIGNAL(valueChanged(int)), sum, SLOT(setValue(int)));
		connect(raw, SIGNAL(valueChanged(int)), corr, SLOT(setValue(int)));
	}
}
