#ifndef AMDETECTORSETVIEW_H
#define AMDETECTORSETVIEW_H

#include "AMDetectorView.h"
#include "beamline/AMDetectorSet.h"

class AMDetectorSetView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor
	/*! \param viewSet Pointer to the AMControlSet to view.
	  \param parent Pointer to QWidget to act as parent.
	  */
	explicit AMDetectorSetView(AMDetectorSet *viewSet, bool configureOnly = false, QWidget *parent = 0);

	AMDetectorView* boxByName(const QString &name){
		return detectorBoxes_.at(viewSet_->indexOfKey(name));
	}

	AMDetectorView* boxAt(int row){
		return detectorBoxes_.at(row);
	}

	/// Returns the current values of the control set
	AMDetectorInfoSet currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMDetectorInfoSet configValues();

signals:
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged(AMDetectorInfoSet);
	/// Emitted when any configuration value changes (but ONLY when the view is configure only)
	void configValuesChanged(AMDetectorInfoSet);

public slots:

protected slots:
	void onDetectorAddedToSet(int index);
	void onControlSetValuesChanged(AMDetectorInfoSet infoList);
	void onConfigurationValueChanged();

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMDetectorSet *viewSet_;
	bool configureOnly_;
	QList<AMDetectorView*> detectorBoxes_;
	QList<AMDetectorView*> detectorDetails_;
	QGridLayout *gl_;
};


#endif // AMDETECTORSETVIEW_H
