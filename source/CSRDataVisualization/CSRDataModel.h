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

	/// Get the time data.
	void timeData(int start, int end, double *newData) const;
	/// Get the data values.
	void data(int start, int end, double *newData) const;
	/// Get the smooth data.
	void smoothData(int start, int end, double *newData) const;

	/// Returns the mean.
	double mean() const { return mean_; }
	/// Returns the standard deviation.
	double standardDeviation() const { return standardDeviation_; }

signals:

public slots:
	/// Loads a file from the target directory.
	bool loadDataFile(const QString &filePath);
	/// Compute some paramenters.
	void compute();

protected:
	/// The mean.
	double mean_;
	/// The standard deviation.
	double standardDeviation_;
	/// Smoothed over data.
	QVector<double> smoothData_;
	/// The data file.  So we don't have to hold the data inside the program.
	mutable QFile *dataFile_;
};

#endif // CSRDATAMODEL_H
