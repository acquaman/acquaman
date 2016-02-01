#ifndef CSRFILECONFIGURATION_H
#define CSRFILECONFIGURATION_H

#include <QObject>
#include <QString>

/// The file information and how many points and size relations.
class CSRFileConfiguration : public QObject
{
	Q_OBJECT

public:
	/// Constructor with everything.
	CSRFileConfiguration(int fileSize, int numberOfPoints, int numberOfRevolutions, QObject *parent = 0);
	/// Copy constructor.
	CSRFileConfiguration(const CSRFileConfiguration &original);
	/// Destructor.
	~CSRFileConfiguration(){}

	/// Returns the file size.
	int fileSize() const { return fileSize_; }
	/// Returns the number of points in the file.
	int numberOfPoints() const { return numberOfPoints_; }
	/// Returns the number of revolutions.
	int numberOfRevolutions() const { return numberOfRevolutions_; }

	/// The equivalence operator.
	bool operator ==(const CSRFileConfiguration &other);
	/// The not equals operator.
	bool operator !=(const CSRFileConfiguration &other);
	/// The equals operator.
	CSRFileConfiguration &operator =(const CSRFileConfiguration &other);

public slots:
	/// Sets the file size.
	void setFileSize(int newSize);
	/// Sets the number of points.
	void setNumberOfPoints(int newNumberOfPoints);
	/// Sets the number of revolutions.
	void setNumberOfRevolutions(int newNumberOfRevolutions);

protected:
	/// The size of the file.
	int fileSize_;
	/// The number of points in the file.
	int numberOfPoints_;
	/// The number of revolutions.
	int numberOfRevolutions_;
};

#endif // CSRFILECONFIGURATION_H
