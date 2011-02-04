/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#ifndef ACQMAN_AMCONTROLSET_H
#define ACQMAN_AMCONTROLSET_H

#define AMCONTROLSET_CONTROL_TIMEOUT_MS 4000

#include "beamline/AMControl.h"
#include "dataman/AMControlInfoList.h"
#include "dataman/AMDetectorInfo.h"

/// An AMControlSet is designed to hold a logical group of controls.
/*!
  Controls that are heirarchically linked should be children of an AMControl. On the other hand, AMControls that are logically linked should be in an AMControlSet.
  For example, beamline energy might be related to several other controls (say mono angle, undulator gap, and exit slit position).
  These are heirarchically linked, so should be children of an energy AMControl.
  This way you could monitor the status of each child to get the status of the control as a whole.
  Conversely, the expected flux of the beamline at any given energy may be several parameters (say the mono grating choice, the exit slit gap, and the harmonic).
  These are logically linked, and should be in an AMControlSet. The convenience is that a large control object can list its logical groups, which can be displayed together or operated on as a group.
  As well, heirarchically linked controls should not likely wind up as a child of more than one control; however, logically grouped controls have no real problem overlapping.

  All that is required to make an AMControlSet is to give the set a name and to add controls by passing a pointer to the AMControl. A function to remove controls is also offered for convenience.
  */
class AMControlSet : public QObject
{
Q_OBJECT
public:
	/// Constructor, only needs a QObject to act as a parent.
	explicit AMControlSet(QObject *parent = 0);

	~AMControlSet(){
		ctrls_.clear();
	}

	/// Returns the name defined for the control set.
	QString name() const { return name_;}
	int count() { return ctrls_.count();}
	AMControl* controlAt(int index) { return ctrls_.at(index);}
	int indexOf(const QString &name);
	AMControl* controlByName(const QString &name);

	AMControlInfoList* info() { return info_; }

	bool isConnected();

signals:
	void connected(bool groupConnected);

public slots:
	/// Sets the name of the control set.
	void setName(const QString &name);
	/// Adds an AMControl to the control set. Returns true if the addition was successful. Failure could result from adding the same AMControl twice.
	bool addControl(AMControl* ctrl);
	/// Removes an AMControl from the control set. Returns true if the removal was successful. Failure could result from removing an AMControl not in the set.
	bool removeControl(AMControl* ctrl);

//	void syncInfo();
	void setFromInfo(AMControlInfoList *info);

protected slots:
	void onConnected(bool ctrlConnected);
	void onConnectionsTimedOut();
	void onValueChanged(double value);

protected:
	/// Holds the name of the control set. Should be descriptive of the logical relationship.
	/// AMControlSetView will use this value as the title of the group box being displayed.
	QString name_;
	/// Local list of AMControl pointers, which represent the controls in the set.
	QList<AMControl*> ctrls_;

	AMControlInfoList *info_;

	bool wasConnected_;
};




#endif // ACQMAN_AMCONTROLSET_H
