#ifndef AMDETECTORINFOLIST_H
#define AMDETECTORINFOLIST_H

#include <QObject>
#include <beamline/AMDetector.h>

class AMDetectorInfoSet : public QObject
{
Q_OBJECT
public:
	/// Constructor, only needs a QObject to act as a parent.
	explicit AMDetectorInfoSet(QObject *parent = 0);
	//AMDetectorInfoSet(AMDetectorInfoSet *copy, QObject *parent = 0);

	~AMDetectorInfoSet(){
		detectors_.clear();
	}

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	int count() { return detectors_.count();}
	AMDetectorInfo* detectorAt(int index) { return detectors_.at(index);}
	int indexOf(const QString &name);
	AMDetectorInfo* detectorByName(const QString &name);
	bool isDefaultAt(int index) { return defaultDetectors_.at(index);}
	bool isDefaultByName(const QString &name);

signals:

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name;}
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addDetector(AMDetectorInfo* detector, bool defaultDetector = false);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeDetector(AMDetectorInfo* detector);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMDetectorInfo*> detectors_;
	QList<bool> defaultDetectors_;
};

#endif // AMDETECTORINFOLIST_H
