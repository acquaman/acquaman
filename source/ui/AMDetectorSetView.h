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

	/*
	/// Returns the current values of the control set
	AMControlInfoList currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMControlInfoList configValues();
	//QMap<QString, QVariant> configValues() { return configValues_;}
	//bool boxTrigger() const { return boxTrigger_;}
	*/

signals:
	/*
	//void configValuesChanged();
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged(AMControlInfoList);
	/// Emitted when any configuration value changes (but ONLY when the view is configure only)
	void configValuesChanged(AMControlInfoList);
	*/

public slots:
	//void setConfigValues(QMap<QString, QVariant> configValues);
	//void resetBoxTrigger(){boxTrigger_ = false;}

protected slots:
	void onDetectorAddedToSet(int index);
	/*
	void onControlSetValuesChanged(AMControlInfoList infoList);
	void onConfigurationValueChanged();
	//void onBoxUpdate(const QString& value);
	*/

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMDetectorSet *viewSet_;
	bool configureOnly_;
	//QMap<QString, QVariant> configValues_;
	QList<AMDetectorView*> detectorBoxes_;
	//QList<bool> dirty_;
	//bool boxTrigger_;
	QVBoxLayout *vl_;
	QHBoxLayout *hl_;
};


#endif // AMDETECTORSETVIEW_H
