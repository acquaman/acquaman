/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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

#include "AMOldDetectorView.h"
#include "beamline/AMOldDetectorSet.h"
#include <QScrollArea>

class AMDetectorSetViewInternal;

class AMDetectorSetView : public QGroupBox
{
Q_OBJECT
public:
	explicit AMDetectorSetView(AMOldDetectorSet *viewSet, bool configureOnly = false, QWidget *parent = 0);
	// Assumes configureOnly is true and uses infoSet as the starting point
	explicit AMDetectorSetView(AMOldDetectorSet *viewSet, const AMOldDetectorInfoSet &infoSet, QWidget *parent = 0);

	int count() const;
	AMOldDetectorSet* detectorSet();
	AMOldDetectorView* boxByName(const QString &name);
	// Darren:  I'm changing this from AMDetectorView const * const because it only needs one.  Also, to be consistent with other parts of our code, I'm also changing the ordering to const AMDetectorView *.
	const AMOldDetectorView *boxAt(int row) const;
	AMOldDetectorView* detailByName(const QString &name);
	// Darren:  I'm changing this from AMDetectorView const * const because it only needs one.  Also, to be consistent with other parts of our code, I'm also changing the ordering to const AMDetectorView *.
	const AMOldDetectorView *detailAt(int row) const;
	bool checkedAt(int row) const;

	/// Returns the current values of the detector set
	AMOldDetectorInfoSet currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMOldDetectorInfoSet configValues();

	friend QDebug operator<<(QDebug d, const AMDetectorSetView& dsv);

	//virtual QSize sizeHint() const;

signals:
	/// Emitted when any control value in the control set changes (but ONLY when the view is not configure only)
	void currentValuesChanged();
	/// Emitted when any configuration value changes or when a detector is enabled/disabled (but ONLY when the view is configure only)
	void configValuesChanged();

protected:
	/// Used for the main bulk of the constructor, as it's the same between both
	void initializeWidget(AMOldDetectorSet *viewSet);

protected:
	AMDetectorSetViewInternal *internalView_;
	QScrollArea *scrollArea_;
};

class AMDetectorSetViewInternal : public QWidget
//class AMDetectorSetView : public QGroupBox
{
Q_OBJECT
public:
	/// Constructor
	/*! \param viewSet Pointer to the AMControlSet to view.
	  \param parent Pointer to QWidget to act as parent.
	  */
	explicit AMDetectorSetViewInternal(AMOldDetectorSet *viewSet, bool configureOnly = false, QWidget *parent = 0);
	// Assumes configureOnly is true and uses infoSet as the starting point
	explicit AMDetectorSetViewInternal(AMOldDetectorSet *viewSet, const AMOldDetectorInfoSet &infoSet, QWidget *parent = 0);

	int count() const;
	AMOldDetectorSet* detectorSet();
	AMOldDetectorView* boxByName(const QString &name);
	// Darren:  I'm changing this from AMDetectorView const * const because it only needs one.  Also, to be consistent with other parts of our code, I'm also changing the ordering to const AMDetectorView *.
	const AMOldDetectorView *boxAt(int row) const;
	AMOldDetectorView* detailByName(const QString &name);
	// Darren:  I'm changing this from AMDetectorView const * const because it only needs one.  Also, to be consistent with other parts of our code, I'm also changing the ordering to const AMDetectorView *.
	const AMOldDetectorView *detailAt(int row) const;
	bool checkedAt(int row) const;

	/// Returns the current values of the detector set
	AMOldDetectorInfoSet currentValues();
	/// Returns the desired values if the view is used for configuration only (returns the current values if the view is not for configuration only)
	AMOldDetectorInfoSet configValues();

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
	AMOldDetectorSet *viewSet_;
	bool configureOnly_;
	QList<AMOldDetectorView*> detectorBoxes_;
	QList<AMOldDetectorView*> detectorDetails_;
	QList<QCheckBox*> checkBoxes_;
	QGridLayout *gl_;
};


#endif // AMDETECTORSETVIEW_H
