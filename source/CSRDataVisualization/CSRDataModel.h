#ifndef CSRDATAMODEL_H
#define CSRDATAMODEL_H

#include <QObject>

#include <QVector>
#include <QFile>
#include <QPointF>

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
	/// Returns the maximum points as a list of (x,y) pairs.
	const QList<QPointF> &maximaPoints() const { return maximaPoints_; }
	/// Returns the peak differences.
	const QVector<double> &mainToWakefieldDifferences() const { return mainToWakeFieldDifferences_; }
	/// Returns the main pain differences.
	const QVector<double> &mainPeakSeparation() const { return mainToMainDifferences_; }
	/// Returns the maximum main peak values.
	const QVector<double> &mainPeakMaxima() const { return mainPeakMaximum_; }
	/// Returns the maximum wakefield values.
	const QVector<double> &wakefieldPeakMaxima() const { return wakefieldPeakMaximum_; }
	/// Returns the integral of the data set where each point corresponds to a rotation.
	const QVector<double> &integral() const { return integral_; }

signals:

public slots:
	/// Loads a file from the target directory.
	bool loadDataFile(const QString &filePath);
	/// Compute some paramenters.
	void computeMeanAndStandardDeviation();
	/// Finds all points that are 5 standard deviations and puts them in a list within the given values.
	void findPeaks(int start, int end);
	/// Some maths.
	void compute();
	/// Compute the integral.
	void computeIntegral();

protected:
	/// The mean.
	double mean_;
	/// The standard deviation.
	double standardDeviation_;
	/// The data file.  So we don't have to hold the data inside the program.
	mutable QFile *dataFile_;
	/// The peak positions.
	QList<double> peakPositions_;
	/// list of maximum values.
	QList<double> maximumValues_;
	/// Peak differences
	QVector<double> mainToWakeFieldDifferences_;
	/// Peak differences
	QVector<double> mainToMainDifferences_;
	/// Maximum main peak values.
	QVector<double> mainPeakMaximum_;
	/// Maximum wakefield peaks.
	QVector<double> wakefieldPeakMaximum_;
	/// A list of points with maxima as (x,y) pair.
	QList<QPointF> maximaPoints_;
	/// The integral where each point is the integral over a single revolution.
	QVector<double> integral_;
};

#endif // CSRDATAMODEL_H
