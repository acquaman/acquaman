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


#ifndef AMACTIONCONDITION_H
#define AMACTIONCONDITION_H

#include <QObject>

/*!
  This class encapsulates a condition that an action needs to have satisfied before it can start or effect it's running status.
  Certain conditions only make sense to check before an action starts, whereas some other conditions only make sense to monitor
  while the action is running.  Finally, some conditions should be checked all of the time.

  This is an abstract class and needs to be subclassed to take into account the specifics of how a condition can be monitored and
  determine if the condition is satisfied or not.  In the event that the condition is not satisfied then an explanation is
  required to let anybody who is interested know why the action is not behaving as expected.  Finally, there condition can say
  whether or not it the action should automatically resume once the condition has been resatisfied.


  ----------- DEFERRED -----------

  After careful consideration, this piece is much more extensive than we originally thought.  In order to have actions properly
  know which condition belongs to them, we need to have proper database access across multiple databases.  The
  AMActionInfo's are in their own database, which will likely have AMActionConditionInfo's in the same database.  However,
  subclasses of AMActionConditionInfo will likely have AMControlInfo's that tell them what to actually monitor (implementation
  detail), which lie inside of the user database.

  There are a couple of things that need to happen before the meat of this model is really tackled; in order they are:

  1) Implement a system for inter-database referencing.
  2) Define the best database heirarchy for storing and reloading AMActionConditionInfo's that make reinstantiating them a very
  logical process.
  3) Build the AMActionConditionInfo class.
  4) Build the other elements; AMActionConditionWatcher, AMActionCondition, AMActionConditionWatcherDialog.  These will manage
  all of the signals and make sure that the conditions are managed properly.

  The AMActionCondition was described above.

  The AMActionConditionWatcher is a manager class that exists beside the AMActionRunner.  When a new action is added to the
  current action changes, the watcher reads in all of the conditions of that particular action. When any condition is not
  satisfied the AMActionConditionWatcherDialog pops up displaying which condition is not being satisfied.  Based on the
  net group result(defaulting to false in the event of discrepancies) in the resumeAutomaticallyOnSatisfied(), you would be able
  to define what should happen as a result of the condition becoming unsatisfied.  Options such as: Restart action, cancel,
  resume, etc.

  A couple aspects of AMActionConditionWatcher that should probably be implemented are:
  1) If a condition becomes unsatisfied, it pauses the queue so that you don't progress to the next action.  The queue should
  be resumed after whatever process happens to resolve the conditions being met, unless the action was cancelled.  If the
  action is cancelled then the likely something about the current workflow will need to be modified.
  2) If the action canPause() then it should.

  It should be noted that there are pictures of whiteboards with some of this hashed out on Darren's mac.
  */
class AMActionCondition : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMActionCondition();
	explicit AMActionCondition(QObject *parent = 0);
	
signals:
	
public slots:
	
};

#endif // AMACTIONCONDITION_H
