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


#ifndef AMACTIONINFO3_H
#define AMACTIONINFO3_H

#include "dataman/database/AMDbObject.h"

/// AMActionInfo defines the interface for subclasses that describe actions (AMAction). The Info subclasses are a way to separate the description/content of actions from their implementation. This allows storing and viewing future/past actions inside programs which shouldn't have the capability to run them
/*! (For example: viewing a history of a user's SGM beamline actions inside their take-home Dataman program, or when on another beamline. Theoretically, this approach could even let someone plan and queue up beamline actions at home to run once they get to the beamline.)

  Normally there is a one-to-one mapping between AMAction and corresponding AMActionInfo classes.  The AMAction class is instantiated by passing an AMActionInfo into its constructor; while the action exists there is a "has-a" relationship between actions and infos.  The AMActionInfo should contain the parameters that specify the content of the action, but should not be coupled to the implementation; the AMAction class adds any necessary implementation objects.

  After actions are run, it is their AMActionInfo that can be stored persistently in the user's database [and hence shared across other beamlines/programs, which is why the infos must be decoupled from the implementation]. If you want to be able to re-create and run live AMActions based on these stored AMActionInfos, you should register your Action/Info pair with AMActionRegistry::registerActionForInfo().

All actions NEED to have a valid action info.  However, if you want to be lazy and dispense with the hassle of custom AMActionInfo subclasses, you can use an instance of the generic AMActionInfo inside your actions (for ex: where the action parameters have been hard-coded into the actions themselves). The information in this base class is sufficient to provide users with general feedback on the state of the action while it is queued or running.  However, in this case it will not be possible to recreate a copy of any particular action based on the stored AMActionInfo.

  A custom AMActionInfo is also necessary if you want to install a custom view or editor for queued or stored actions; this can be done using the AMActionRegistry functions registerDelegateForInfo(), registerEditorForInfo() and registerWindowEditorForInfo(). All of the views/editors operate on the info rather than the action, so that they can be used in programs that shouldn't/can't access the action's implementation.
*/
class AMActionInfo3 : public AMDbObject
{
	Q_OBJECT

	Q_PROPERTY(QString shortDescription READ shortDescription WRITE setShortDescription)
	Q_PROPERTY(QString longDescription READ longDescription WRITE setLongDescription)
	Q_PROPERTY(QString iconFileName READ iconFileName WRITE setIconFileName)
	Q_PROPERTY(double expectedDuration READ expectedDuration WRITE setExpectedDuration)
	Q_PROPERTY(bool canCopy READ canCopy WRITE setCanCopy)

	Q_CLASSINFO("AMDbObject_Attributes", "description=Generic Action Info3")

public:
	/// Constructor
	Q_INVOKABLE AMActionInfo3(const QString& shortDescription = "Generic Action", const QString& longDescription = QString(), const QString& iconFileName = QString(), QObject *parent = 0);
	/// Copy Constructor
	AMActionInfo3(const AMActionInfo3& other);

	/// This function is used as a virtual copy constructor, so we can make detailed copies of info instances without knowing their detailed type. It must be re-implemented by each subclass, and we strongly suggest implementing and using the copy constructor, ie: createCopy() { return new MyAMActionInfo(*this); }
	virtual AMActionInfo3* createCopy() const { return new AMActionInfo3(*this); }

	/// Destructor: nothing required in the base class
	virtual ~AMActionInfo3() {}


	/// This should describe the type of the action, for example, "REIXS Sample Move Action"; it should be re-implemented by subclasses.
	virtual QString typeDescription() const { return "Generic Action"; }


	/// This description is presented to users in default views, for example: "Move to Sample Fe2O3". Unlike typeDescription(), it is intended to be usually unique to an action instance.
	/*! This is actually just a synonym for AMDbObject::name(). */
	QString shortDescription() const { return shortDescription_; }
	/// This description is presented to users in default views, for example: "<b>Moving to Sample Fe2O3:</b>\n X=30.4mm,\n Y=12.1mm,\n Z=16.5mm,\n R=45.0deg"
	QString longDescription() const { return longDescription_; }
	/// This returns the path to an icon used to represent the action in default views. If none is provided, it returns a generic icon. PNG images are recommended.
	QString iconFileName() const { if(iconFileName_.isEmpty()) return ":/64x64/generic-action.png"; return iconFileName_; }

	/// Returns the total expected duration in seconds. If the time to completion is unknown, this will return -1. This is usually set by the action implementation while running, but if you have an estimate of how long it will take before the action runs, it's fine to pre-load this.
	double expectedDuration() const { return expectedDuration_; }

	/// Returns whether this action can be copied.  There may be instances (ie: sub actions within a list) that you don't want to provide individual access to.  This way even if you see this action in the history, this flag will determine whether or not you can copy it or not.
	bool canCopy() const { return canCopy_; }

	/// Returns a string with any warnings that occured during the load from database phase. Can be overridden by subclasses. Empty string implies no warnings.
	virtual QString dbLoadWarnings() const { return QString(); }

public slots:

	/// Set a short description of of the action, for ex: "Moving to Sample Fe2O3"
	void setShortDescription(const QString& shortDescription) { shortDescription_ = shortDescription; setModified(true); emit infoChanged(); }
	/// Set a long description of the action, for example: "<b>Moving to Sample Fe2O3:</b>\n X=30.4mm,\n Y=12.1mm,\n Z=16.5mm,\n R=45.0deg"
	void setLongDescription(const QString& longDescription) { longDescription_ = longDescription; setModified(true); emit infoChanged(); }
	/// Set the path to an icon used to represent the action in default views. PNG images are recommended.
	void setIconFileName(const QString& iconFilename) { iconFileName_ = iconFilename; setModified(true); emit infoChanged(); }

	/// Sets the expected duration (total run time) of the action in seconds. It's good for action implementations to update this while running if they can figure it out.
	void setExpectedDuration(double expectedDurationSeconds) { expectedDuration_ = expectedDurationSeconds; setModified(true); emit infoChanged(); emit expectedDurationChanged(expectedDuration_); }

	/// Sets whether or not this action can be copied inside the workflow.
	void setCanCopy(bool copy) { canCopy_ = copy; setModified(true); emit infoChanged(); }

public:
	// Database thumbnails: None for now... Use in AMActionLogItem later?
	/////////////////////
	//	/// If we have a non-empty iconFileName_ has been set, we return 1 thumbnail.
	//	virtual int thumbnailCount() const { return iconFileName_.isEmpty() ? 0 : 1; }
	//	/// If we have a non-empty iconFileName_ has been set, we return 1 thumbnail with that icon.
	//	virtual AMDbThumbnail thumbnail(int index) const;

signals:
	/// Emitted when any basic information changes: shortDescription(), longDescription(), iconFileName(), canCopy().
	void infoChanged();
	/// Emitted when the expected duration changes.
	void expectedDurationChanged(double seconds);



protected:
	QString shortDescription_;
	QString longDescription_;
	QString iconFileName_;
	double expectedDuration_;
	bool canCopy_;
};

#endif // AMACTIONINFO3_H
