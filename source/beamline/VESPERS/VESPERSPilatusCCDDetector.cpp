#include "VESPERSPilatusCCDDetector.h"

#include <QDebug>
VESPERSPilatusCCDDetector::VESPERSPilatusCCDDetector(const QString &name, const QString &description, QObject *parent)
	: VESPERSCCDDetector(name, description, "PAD1607-B21-05:cam1", AMnDIndex(981, 1043), parent)
{
	dfProcess_ = new QProcess;
	connect(dfProcess_, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(readOutProcess(int,QProcess::ExitStatus)));
	connect(this, SIGNAL(connected(bool)), this, SLOT(updateAuroraSize()));
}

void VESPERSPilatusCCDDetector::updateAuroraSize()
{
	dfProcess_->start("df", QStringList() << "/nas/vespers");
}

void VESPERSPilatusCCDDetector::readOutProcess(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (exitCode == 0 && exitStatus == QProcess::NormalExit){

		QStringList output = QString(dfProcess_->readAll()).split(" ", QString::SkipEmptyParts);
		totalAuroraSize_ = output.at(7).toDouble()/1e9;
		occupiedAuroraSize_ = output.at(8).toDouble()/1e9;
		emit fileSystemInfoUpdated();
	}
}
