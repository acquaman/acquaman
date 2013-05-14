#include "VESPERSPilatusCCDDetector.h"

#include <QProcess>
#include <QDebug>
VESPERSPilatusCCDDetector::VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, "PAD1607-B21-05:cam1", AMnDIndex(981, 1043), parent)
{
}

void VESPERSPilatusCCDDetector::updateAuroraSize()
{
	QProcess proc;
	proc.start("df", QStringList() << "/nas/vespers");

	if (!proc.waitForFinished())
		return;

	QString test = QString(proc.readAll());
	QStringList output = test.split(" ", QString::SkipEmptyParts);
	totalAuroraSize_ = output.at(8).toDouble()/1e9;
	occupiedAuroraSize_ = output.at(9).toDouble()/1e9;
	qDebug() << test << output << occupiedAuroraSize_ << totalAuroraSize_;
}
