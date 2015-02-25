#ifndef CSRDATAMODEL_H
#define CSRDATAMODEL_H

#include <QObject>

#include <QVector>

class CSRDataModel : public QObject
{
	Q_OBJECT
public:
	explicit CSRDataModel(QObject *parent = 0);

	/// Get the time data.
	void timeData(double *newData) const;
	/// Get the data values.
	void data(double *newData) const;
	/// Get the smooth data.
	void smoothData(double *newData) const;

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
	/// The time data.
	QVector<double> timeData_;
	/// The values.
	QVector<double> data_;
	/// The mean.
	double mean_;
	/// The standard deviation.
	double standardDeviation_;
	/// Smoothed over data.
	QVector<double> smoothData_;

};

#endif // CSRDATAMODEL_H
