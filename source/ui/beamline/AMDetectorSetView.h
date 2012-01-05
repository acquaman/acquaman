/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
	// Darren:  I'm changing this from AMDetectorView const * const because it only needs one.  Also, to be consistent with other parts of our code, I'm also changing the ordering to const AMDetectorView *.
	const AMDetectorView *boxAt(int row) const;
	AMDetectorView* detailByName(const QString &name);
	// Darren:  I'm changing this from AMDetectorView const * const because it only needs one.  Also, to be consistent with other parts of our code, I'm also changing the ordering to const AMDetectorView *.
	const AMDetectorView *detailAt(int row) const;
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
	void setDisabled(bool disabled);

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
