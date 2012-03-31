#ifndef AMFETCHSPECTRUMTHREAD_H
#define AMFETCHSPECTRUMTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include "dataman/AMnDIndex.h"

/*!
  This class uses a separate thread to go into a file with spectra and return with the given index.  Currently, it assumes that the
  file that is being accessed is the in the format of a single point's worth of spectra on every line.  For simplicity, it also assumes
  that only the first spectra (if there are more than one on a single line) is the spectrum that is desired.
  */
class AMFetchSpectrumThread : public QThread
{
	Q_OBJECT

public:
	/// Constructor.
	AMFetchSpectrumThread(QObject *parent = 0);
	/// Destructor.
	~AMFetchSpectrumThread();

	/// Function that goes into a spectra file and grabs the spectrum specified by AMnDIndex.  The \param size specifies how big the spectrum is.  If 0 then grab the whole line.  If it is specified then only grab that many elements.
	void fetch(AMnDIndex index, int rowLength, const QString &filename, int size);

signals:
	/// Notifier that a new spectrum has been fetched.
	void fetchedSpectrum(const QVector<double> &);

protected:
	/// Reimplemented.  Method that is called when the thread is started.
	void run();

private:
	/// The mutex for ensuring data validity.
	QMutex mutex_;
	/// Holds the wait condition for putting the thread to sleep or not.
	QWaitCondition condition_;
	/// The index of the point that the spectrum is associated with.
	AMnDIndex index_;
	/// The length of the row for the x axis of the scan.
	int rowLength_;
	/// Name of the file that will be searched.
	QString filename_;
	/// The size of the spectrum.
	int size_;
	/// Flag holding whether or not the thread should restart with new info.
	bool restart_;
	/// Flag holding whether or not the thread should stop working.
	bool abort_;
};

#endif // AMFETCHSPECTRUMTHREAD_H
