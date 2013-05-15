#include "VESPERSPilatusCCDDetector.h"

#include <QProcess>
#include <QDebug>
VESPERSPilatusCCDDetector::VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, "PAD1607-B21-05:cam1", AMnDIndex(981, 1043), parent)
{
	connect(this, SIGNAL(connected(bool)), this, SLOT(updateAuroraSize()));
}

void VESPERSPilatusCCDDetector::updateAuroraSize()
{
	QProcess proc;
	proc.start("df", QStringList() << "/nas/vespers");

	if (!proc.waitForFinished())
		return;

	QStringList output = QString(proc.readAll()).split(" ", QString::SkipEmptyParts);
	totalAuroraSize_ = output.at(7).toDouble()/1e9;
	occupiedAuroraSize_ = output.at(8).toDouble()/1e9;
	emit fileSystemInfoUpdated();
}
