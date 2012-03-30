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

void AMFetchSpectrumThread::run()
{
	forever {

		// Get the current file name and index.
		mutex_.lock();
		QString filename = filename_;
		AMnDIndex index = index_;
		int rowLength = rowLength_;
		int size = size_;
		mutex_.unlock();

		// The work.
		QFile file(filename);
		QVector<double> spectrum;

		if (file.open(QIODevice::ReadOnly)){

			QTextStream in(&file);
			QStringList currentLine;
			int iterations = rowLength*index.j() + index.i() -1;
			for (int i = 0; i < iterations; i++)
				in.readLine();

			currentLine = in.readLine().split(",");
			spectrum.resize(currentLine.size());
			if (size == 0)
				size = spectrum.size();

			for (int i = 0; i < size; i++)
				spectrum[i] = currentLine.at(i).toInt();

			// Pass the spectrum along as long as a new spectrum hasn't been requested.
			if (!restart_)
				emit fetchedSpectrum(spectrum);
		}

		// The clean up.  Puts thread to sleep if it's done.
		mutex_.lock();

		if (!restart_)
			condition_.wait(&mutex_);

		restart_ = false;
		mutex_.unlock();
	}
}
