#ifndef VESPERSVALVEGROUPCONTROL_H
#define VESPERSVALVEGROUPCONTROL_H

#include <QObject>
#include <QList>

#include "beamline/VESPERS/AMValveControl.h"

/*!
	This class handles some of the group behaviour for the valve controls.  It has the ability to open and shut all the
	valves simulaneously, but also individual control.  It returns the overall status of the valves, which is is closed
	even if only one valve is closed.
  */

class VESPERSValveGroupControl : public QObject
{
	Q_OBJECT
public:
	/// Constructor.  Takes in all the valve controls and puts them in list and sets up all necessary connections.
	explicit VESPERSValveGroupControl(QList<AMValveControl *> *list, QObject *parent = 0);

	/// Status: whether all the valves are open or not.
	bool allValvesOpen();

	/// Returns the index of the valve with a given name.  Returns -1 if invalid.
	int indexOf(QString name);
	/// Returns the name of the valve at a given index.  Returns a null string if not a valid index.
	QString nameAtIndex(int index) { if (index >= 0 && index < list_->size()) return list_->at(index)->name(); return QString(); }

	/// Opens valve at a given index.
	void openValve(int index) { if (index >= 0 && index < list_->size() && list_->at(index)->stateString().compare("Closed") == 0) list_->at(index)->openValve(); }
	/// Closes valve at a given index.
	void closeValve(int index) { if (index >= 0 && index < list_->size() && list_->at(index)->stateString().compare("Open") == 0) list_->at(index)->closeValve(); }

signals:
	/// Notifies when the status has changed.
	void statusChanged(bool);

public slots:
	/// Class that opens all the valves in sequence.
	void openAllValves();
	/// Class that closes all the valves in sequence.
	void closeAllValves();

protected slots:
	/// Helper slot that handles opening the next valve.
	void openAllValvesHelper();
	/// Helper slot that handles closing the next valve.
	void closeAllValvesHelper();
	/// Determines whether all the valves are open or not.
	void onValveStateChanged() { emit statusChanged(allValvesOpen()); }

protected:
	/// The list.
	QList<AMValveControl *> *list_;

	/// Index used to keep track of which index we're on while opening or closing all the valves.  Always -1 except when opening or closing valves.
	int index_;

};

#endif // VESPERSVALVEGROUPCONTROL_H
