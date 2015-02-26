#ifndef CSRDATAMODEL_H
#define CSRDATAMODEL_H

#include <QObject>

#include <QVector>
#include <QFile>

class CSRDataModel : public QObject
{
	Q_OBJECT
public:
	explicit CSRDataModel(QObject *parent = 0);
	~CSRDataModel();

	/// Get the time data.
	void timeData(int start, int end, double *newData) const;
	/// Get the data values.
	void data(int start, int end, double *newData) const;

	/// Returns the mean.
	double mean() const { return mean_; }
	/// Returns the standard deviation.
	double standardDeviation() const { return standardDeviation_; }
	/// Returns the peak positions.
	QList<double> peakPositions() const { return peakPositions_; }

signals:

public slots:
	/// Loads a file from the target directory.
	bool loadDataFile(const QString &filePath);
	/// Compute some paramenters.
	void computeMeanAndStandardDeviation();
	/// Finds all points that are 5 standard deviations and puts them in a list within the given values.
	void findPeaks(int start, int end);

protected:
	/// The mean.
	double mean_;
	/// The standard deviation.
	double standardDeviation_;
	/// The data file.  So we don't have to hold the data inside the program.
	mutable QFile *dataFile_;
	/// The peak positions.
	QList<double> peakPositions_;
};

#endif // CSRDATAMODEL_H
