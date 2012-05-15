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


#include "AMFetchSpectrumThread.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>

AMFetchSpectrumThread::AMFetchSpectrumThread(QObject *parent)
	: QThread(parent)
{
	restart_ = false;
	abort_ = false;
	filename_ = "";
	size_ = 0;
	index_ = AMnDIndex();
	rowLength_ = 0;
}

AMFetchSpectrumThread::~AMFetchSpectrumThread()
{
	mutex_.lock();
	abort_ = true;
	condition_.wakeOne();
	mutex_.unlock();

	wait();
}

void AMFetchSpectrumThread::fetch(AMnDIndex index, int rowLength, const QString &filename, int size)
{
	QMutexLocker locker(&mutex_);

	filename_ = filename;
	index_ = index;
	rowLength_ = rowLength;
	size_ = size;

	if (!isRunning())
		start(LowPriority);

	else {

		restart_ = true;
		condition_.wakeOne();
	}
}
#include <QDebug>
void AMFetchSpectrumThread::run()
{
	forever {

		// Get the current file name and index.
		mutex_.lock();
		QString filename = filename_;
		AMnDIndex index = index_;
		int rowLength = rowLength_;
		int size = size_;
		bool abort = abort_;
		bool restart = restart_;
		mutex_.unlock();

		// Check to see if the thread has been aborted.
		if (abort)
			return;

		// The work.
		QFile file(filename);
		QVector<double> spectrum;

		if (file.open(QIODevice::ReadOnly)){

			QTextStream in(&file);
			QStringList currentLine;
			int iterations = rowLength*index.j() + index.i();

			for (int i = 0; i < iterations; i++)
				in.readLine();

			if (!in.atEnd()){

				currentLine = in.readLine().split(",");
				spectrum.resize(currentLine.size());
				if (size == 0)
					size = spectrum.size();

				for (int i = 0; i < size; i++)
					spectrum[i] = currentLine.at(i).toInt();

				// Pass the spectrum along as long as a new spectrum hasn't been requested.
				if (!restart)
					emit fetchedSpectrum(spectrum);
			}
		}

		// The clean up.  Puts thread to sleep if it's done.
		mutex_.lock();

		if (!restart_)
			condition_.wait(&mutex_);

		restart_ = false;
		mutex_.unlock();
	}
}
