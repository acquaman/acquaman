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
