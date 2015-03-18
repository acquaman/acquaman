#ifndef CSRDATAMODEL_H
#define CSRDATAMODEL_H

#include <QObject>

#include <QVector>
#include <QFile>
#include <QStringList>

#include "CSRDataVisualization/CSRFileConfiguration.h"

class CSRDataModel : public QObject
{
	Q_OBJECT

public:
	/// Constructor for the data model.
	explicit CSRDataModel(QObject *parent = 0);
	/// Copy constructor.
	CSRDataModel(const CSRDataModel &original);
	/// Destructor.
	~CSRDataModel();

	/// Get the time data.
	void timeData(int start, int end, double *newData) const;
	/// Get the data values.
	void data(int start, int end, double *newData) const;

	/// Returns the integral of the data set where each point corresponds to a rotation.
	const QVector<double> &integral() const { return integral_; }

	/// Adds a file configuration.
	void addFileConfiguration(CSRFileConfiguration *info);
	/// Removes a file configuraiton.
	void removeFileConfiguration(CSRFileConfiguration *info);
	/// Returns a configuration.
	CSRFileConfiguration *fileConfigurationAt(int index) const;
	/// Returns the number of configurations.
	int fileConfigurationCount() const;

	/// Set the file names.
	void setFileNames(const QStringList &newFileNames);
	/// Returns the file names.
	const QStringList &fileNames() const { return fileNames_; }

signals:
	/// Notifier on the progress of all the files.
	void progress(int);
	/// Notifier that all the files have been handled.
	void finished();

public slots:
	/// This starts the model running through all its data files.
	void start();
	/// Sets up all the important parameters necessary to run the data.
	void setupFileParameters(const QString &filePath);
	/// Loads a file from the target directory.
	bool loadDataFile(const QString &filePath);
	/// Compute the integral.
	void computeIntegral();
	/// Writes the output file.
	void writeOutputFile(const QString &filePath);

protected:
	/// The data file.  So we don't have to hold the data inside the program.
	mutable QFile *dataFile_;
	/// The integral where each point is the integral over a single revolution.
	QVector<double> integral_;
	/// The size of the file.
	int fileSize_;
	/// The number of points in the file.
	int numberOfPoints_;
	/// The number of revolutions in the data.
	int numberOfRevolutions_;
	/// List of file configurations.
	QList<CSRFileConfiguration *> fileConfigurations_;
	/// List of file names that will be run through.
	QStringList fileNames_;
};

#endif // CSRDATAMODEL_H
