#ifndef CSRDATAMODEL_H
#define CSRDATAMODEL_H

#include <QObject>

#include <QVector>
#include <QFile>

#include "CSRDataVisualization/CSRFileConfiguration.h"

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

	/// Returns the integral of the data set where each point corresponds to a rotation.
	const QVector<double> &integral() const { return integral_; }

	/// Adds a file configuration.
	void addFileConfiguration(CSRFileConfiguration *info);
	/// Returns a configuration.
	CSRFileConfiguration *fileConfigurationAt(int index) const;
	/// Returns the number of configurations.
	int fileConfigurationCount() const;

signals:

public slots:
	/// Loads a file from the target directory.
	bool loadDataFile(const QString &filePath);
	/// Compute the integral.
	void computeIntegral();

protected:
	/// The data file.  So we don't have to hold the data inside the program.
	mutable QFile *dataFile_;
	/// The integral where each point is the integral over a single revolution.
	QVector<double> integral_;
	/// List of file configurations.
	QList<CSRFileConfiguration *> fileConfigurations_;
};

#endif // CSRDATAMODEL_H
