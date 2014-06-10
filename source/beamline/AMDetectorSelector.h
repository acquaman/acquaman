#ifndef AMDETECTORSELECTOR_H
#define AMDETECTORSELECTOR_H

#include <QObject>
#include <QMap>
#include <QStringList>

#include "dataman/info/AMDetectorInfoSet.h"

class AMDetectorGroup;
class AMDetector;

class AMDetectorSelector : public QObject
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMDetectorSelector();
	AMDetectorSelector(AMDetectorGroup *detectorGroup, QObject *parent = 0);

	/// Returns whether or not a detector is selected (enabled) by checking against the name
	bool detectorIsSelectedByName(const QString &name) const;
	/// Convenience version of above
	bool detectorIsSelected(AMDetector *detector) const;
	AMDetectorInfoSet selectedDetectorInfos() const;
	AMDetectorInfoSet unselectedDetectorInfos() const;

	/// Returns whether or not a detector is set to default by checking against the name
	bool detectorIsDefaultByName(const QString &name) const;
	/// Convenience version of above
	bool detectorIsDefault(AMDetector *detector) const;

	/// Returns the detector group
	AMDetectorGroup* detectorGroup();

	/// Returns the number of detectors in the detector group. Convenience call.
	int count() const;

	/// Returns the preferential ordering list (an ordered list of detector names)
	QStringList preferentialOrdering() const;

public slots:
	/// Sets the detector as selected (or not selected) by passing the detector name
	void setDetectorSelectedByName(const QString &name, bool isSelected);
	/// Convenience version of above
	void setDetectorSelected(AMDetector *detector, bool isSelected);
	void setDefaultsSelected();

	/// Sets the detector as default (or not default) by passing the detector name
	void setDetectorDefaultByName(const QString &name, bool isDefault);
	/// Convenience version of above
	void setDetectorDefault(AMDetector *detector, bool isDefault);

	/// Sets the preferential ordering list
	void setPreferentialOrdering(const QStringList &preferentialOrdering);

signals:
	/// Emitted when the selected state for a detector changes
	void selectedChanged(AMDetector *detector);
	/// Emitted when the default state for a detector changes
	void defaultChanged(AMDetector *detector);

	/// Forwards signal from the detector group
	void detectorBecameUnconnected(AMDetector *detector);
	/// Forwards signal from the detector group
	void detectorBecameConnected(AMDetector *detector);
	/// Forwards signal from the detector group
	void allAreConnectedChanged(bool allAreConnected);

protected slots:
	/// Handles removing detectors from the mappings
	void onDetectorRemovedFromGroup(AMDetector *detector);
	/// Handles adding detectors to the mappings
	void onDetectorAddedToGroup(AMDetector *detector);

protected:
	/// The detector group holding the detectors of interest, whether connected or unconnected
	AMDetectorGroup *detectorGroup_;
	/// A mapping of selected (enabled) detectors by detector name
	QMap<QString, bool> selectedDetectors_;
	/// A mapping of default detectors by detector name
	QMap<QString, bool> defaultDetectors_;
	/// A preferential ordering of the detectors
	QStringList preferentialOrdering_;
};

#endif // AMDETECTORSELECTOR_H
