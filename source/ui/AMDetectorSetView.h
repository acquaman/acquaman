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

	int count() const{
		return viewSet_->count();
	}

	AMDetectorSet* detectorSet(){
		return viewSet_;
	}

	AMDetectorView* boxByName(const QString &name){
		return detectorBoxes_.at(viewSet_->indexOfKey(name));
	}

	AMDetectorView const * const boxAt(int row) const{
		return detectorBoxes_.at(row);
	}

	AMDetectorView* detailByName(const QString &name){
		return detectorDetails_.at(viewSet_->indexOf(name));
	}

	AMDetectorView const * const detailAt(int row) const{
		return detectorDetails_.at(row);
	}

	/// Returns the current values of the detector set
	AMDetectorInfoSet currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMDetectorInfoSet configValues();

	friend QDebug operator<<(QDebug d, const AMDetectorSetView& dsv);

signals:
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged();
	/// Emitted when any configuration value changes (but ONLY when the view is configure only)
	void configValuesChanged();

public slots:

protected slots:
	void onDetectorAddedToSet(int index);
	void onDetectorSetSettingsChanged();
	void onDetectorSetConfigurationRequested();

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMDetectorSet *viewSet_;
	bool configureOnly_;
	QList<AMDetectorView*> detectorBoxes_;
	QList<AMDetectorView*> detectorDetails_;
	QGridLayout *gl_;
};


#endif // AMDETECTORSETVIEW_H
