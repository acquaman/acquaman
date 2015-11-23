/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef AM_AMCONTROLSET_H
#define AM_AMCONTROLSET_H

#define AMCONTROLSET_CONTROL_TIMEOUT_MS 4000

#include "beamline/AMControl.h"
#include "dataman/info/AMControlInfoList.h"

#include "util/AMOrderedSet.h"

/// An AMControlSet is designed to hold a logical group of controls.
/*!
  Internally, AMControl provides a way of organizing controls hierarchichally, using its childrenControls().  This relationship implies ownership and exclusivity. On the other hand, AMControlSet provides a way of organizing controls that are logically linked together.
  For example, beamline energy might be related to several other controls (say mono angle, undulator gap, and exit slit position).
  These are heirarchically linked, so should be children of an energy AMControl.
  This way you could monitor the status of each child to get the status of the control as a whole.
  Conversely, the expected flux of the beamline at any given energy may be several parameters (say the mono grating choice, the exit slit gap, and the harmonic).
  These are logically linked, and should be in an AMControlSet. The convenience is that a large control object can list its logical groups, which can be displayed together or operated on as a group.
  As well, heirarchically linked controls should not likely wind up as a child of more than one control; however, logically grouped controls have no real problem overlapping.

  AMControlSet provides a container for a logical group of controls.  The container does NOT take ownership of the controls that are placed in it, nor delete these controls when removing from the container or deleting the container.
  */
class AMControlSet : public QObject, public AMOrderedSet<QString, AMControl*>
{
Q_OBJECT
public:
	/// Constructor
 	virtual ~AMControlSet();
	explicit AMControlSet(QObject *parent = 0);

	/// Returns the name defined for the control set.
	QString name() const;

	/// Converts all the controls to their simplified AMControlInfo form, and returns a list like this
	AMControlInfoList toInfoList() const;

	/// Returns true if ALL the controls in this set are connected.
	bool isConnected() const;

	/// Returns the names of the subset of controls which are not currently connected
	QStringList unconnected() const;

	/// Return the index of a given \c control in the set. You can then access the control using at() or operator[].  (Returns -1 if not found in the set.)
	int indexOf(AMControl* control);
	/// Return the index of the control named \c controlName. (Returns -1 if not found in the set.)
	int indexOf(const QString& controlName);
	/// Returns the control named \c controlName, or 0 if not found in the set.
	AMControl* controlNamed(const QString& controlName) const;

	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addControl(AMControl* newControl);

	/// Removes an AMControl \c control from the set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeControl(AMControl* control);


	/// Checks to see if this control set can be set from the given AMControlInfoList (they refer to the same controls)
	bool validInfoList(const AMControlInfoList& info);

	/// Sets positions of the controls in the set from the available info in the AMControlInfoList \c infoList. Returns the number of controls that were set (matched by name)
	int setFromInfoList(const AMControlInfoList& info);

signals:
	/// This signal is emitted whenever isConnected() changes
	void connected(bool groupConnected);
	void controlConnectedChanged(bool isConnected, AMControl *control);

	/// This signal is emitted if any of the controls in the set have timed out.
	void controlSetTimedOut();

	/// This signal is emitted whenever one of the controls has a new value.
	void controlSetValuesChanged();

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name) { name_ = name; }




protected slots:
	/// Handles when any of the controls become connected or disconnected
	void onConnected(bool ctrlConnected);
	void onConnectionsTimedOut();

	/// Handles when any of the controls has a new value
	void onControlValueChanged();

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;


	/// Caches whether all the controls in this set were previously connected.
	bool wasConnected_;
};




#endif // AM_AMCONTROLSET_H
