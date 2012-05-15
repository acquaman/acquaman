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


#include "VESPERSMapSetup.h"

#include <QProcess>

VESPERSMapSetup::VESPERSMapSetup(QObject *parent) :
	QObject(parent)
{
	is2D_ = true;
	usingSingleEl_ = true;
	usingCCD_ = false;
	usingMultipleImages_ = false;
}

void VESPERSMapSetup::launchNDMapper()
{
	if (is2D() && !usingCCD()){

		if (usingSingleElement())
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 1 --useCCD=n --multiImage=n");
		else
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 2 --useCCD=n --multiImage=n");
	}
	else if (is2D() && usingCCD()){

		if (usingSingleElement()){

			if (usingMultipleImages())
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 1 --useCCD=y --multiImage=y");
			else
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 1 --useCCD=y --multiImage=n");
		}
		else{

			if (usingMultipleImages())
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 2 --useCCD=y --multiImage=y");
			else
				QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 2 2 --useCCD=y --multiImage=n");
		}
	}
	else if (is3D() && usingSingleElement()){

		if (usingMultipleImages())
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 1 --useCCD=y --multiImage=y");
		else
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 1 --useCCD=y --multiImage=n");
	}
	else if (is3D() && usingFourElement()){

		if (usingMultipleImages())
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 2 --useCCD=y --multiImage=y");
		else
			QProcess::startDetached("konsole -vt_sz 5x5 -e /home/vespers/bin/runNDMapper 3 2 --useCCD=y --multiImage=n");
	}
}
