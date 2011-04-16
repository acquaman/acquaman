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

	int count() const;
	AMDetectorSet* detectorSet();
	AMDetectorView* boxByName(const QString &name);
	AMDetectorView const * const boxAt(int row) const;
	AMDetectorView* detailByName(const QString &name);
	AMDetectorView const * const detailAt(int row) const;
	bool checkedAt(int row) const;

	/// Returns the current values of the detector set
	AMDetectorInfoSet currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMDetectorInfoSet configValues();

	friend QDebug operator<<(QDebug d, const AMDetectorSetView& dsv);

signals:
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged();
	/// Emitted when any configuration value changes or when a detector is enabled/disabled (but ONLY when the view is configure only)
	void configValuesChanged();

public slots:

protected slots:
	void onDetectorAddedToSet(int index);
	void onDetectorRemovedFromSet(int index);
	void onDetectorSetSettingsChanged();
	void onDetectorSetConfigurationRequested();

protected:
	/// Pointer to the AMControlSet which is the subject of this view.
	AMDetectorSet *viewSet_;
	bool configureOnly_;
	QList<AMDetectorView*> detectorBoxes_;
	QList<AMDetectorView*> detectorDetails_;
	QList<QCheckBox*> checkBoxes_;
	QGridLayout *gl_;
};


#endif // AMDETECTORSETVIEW_H
