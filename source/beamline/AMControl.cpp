#include "Control.h"

/// Set the control object's children (and grandchildren, etc) based on a QMap of QString and double pairs
/// QString is the name of the child (as in child's objectName) and value is the desired move position
/// errorLevel specifies what constitutes an error (shouldn't move it, can't move it, can't find it)
/// any error causes NO MOVEMENTS to occur
bool AMControl::setStateList(const QMap<QString, double> controlList, unsigned int errorLevel){

	if(errorLevel & 0x4)
		qDebug() << "Fail on shouldn't";
	if(errorLevel & 0x2)
		qDebug() << "Fail on can't";
	if(errorLevel & 0x1)
		qDebug() << "Fail on unfound";

	// Copy of the QMap to pass around, remove found instances from it
	QMap<QString, double> *tmpList = new QMap<QString, double>(controlList);
	// New list of name and pointer to Control to add found instances to
	QMap<QString, AMControl*> *executeList = new QMap<QString, AMControl*>();
	// Start search
	if(!searchSetChildren(tmpList, executeList, errorLevel))
		return FALSE;
	// Check if there are any unfound instances and flag as error if necessary
	if(tmpList->count() > 0 && (errorLevel & 0x1) )
		return FALSE;
	// Run through original list. If it's not in "unfounds" list and if it's outside the tolerance, move it to the specified position
	QMap<QString, double>::const_iterator i = controlList.constBegin();
	 while (i != controlList.constEnd()) {
		 if(!tmpList->contains(i.key()) && fabs(i.value() - executeList->value(i.key())->value()) > executeList->value(i.key())->tolerance() )
			 executeList->value(i.key())->move(i.value());
//             qDebug() << "Name is " << i.key() << " value should be " << i.value() << " value is " << executeList->value(i.key())->value();
		 ++i;
	 }
	return TRUE;
}

/// Used internally by setStateList, called recursively.
bool AMControl::searchSetChildren(QMap<QString, double> *controlList, QMap<QString, AMControl*> *executeList, unsigned int errorLevel){
	AMControl* tmpCtrl = NULL;
	// Run through all the children, check for shouldn't move and can't move errors
	// Insert in the list of executable instances, remove from the "unfounds" list
	for(int x = 0; x < numChildren(); x++){
		tmpCtrl = child(x);
		qDebug() << "Inspecting " << tmpCtrl->objectName();
		if(controlList->contains(tmpCtrl->objectName())){
			qDebug() << "Checking against " << tmpCtrl->objectName() << " should " << tmpCtrl->shouldMove() << " can " << tmpCtrl->canMove();
			if(!tmpCtrl->shouldMove() && (errorLevel & 0x4) )
				return false;
			if(!tmpCtrl->canMove() && tmpCtrl->shouldMove() && (errorLevel & 0x2) )
				return false;
			if(tmpCtrl->shouldMove() && tmpCtrl->canMove()){
				executeList->insert(tmpCtrl->objectName(), tmpCtrl);
				controlList->remove(tmpCtrl->objectName());
			}
		}
		// Call recursively on all grandchildren (CHECK TO SEE IF NO CHILDREN?, SAVE A CALL)
 //       for(int y = 0; y < tmpCtrl->numChildren(); y++){
		if(tmpCtrl->numChildren() > 0)
			if(!tmpCtrl->searchSetChildren(controlList, executeList, errorLevel))
				return false;
 //       }
	}
	return true;
}

AMReadOnlyPVControl::AMReadOnlyPVControl(const QString& name, const QString& readPVname, double tolerance, QObject* parent) : AMControl(name, "?", tolerance, parent),  {

	readPV_ = new DoubleProcessVariable(readPVname, true, this);

	connect(readPV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(readPV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SIGNAL(readConnectionTimeoutOccurred()));
	connect(readPV_, SIGNAL(error(int)), this, SLOT(onPVError(int)));
	connect(readPV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	connect(readPV_, SIGNAL(initialized()), this, SLOT(onReadPVInitialized()));

	/* reuse for wStatus:
	if(movingPVname != "") {
		movingPV_ = new IntProcessVariable(movingPVname, true, this);
		connect(movingPV_, SIGNAL(valueChanged(int)), this, SLOT(onMovingChanged(int)));
	}
	else
		movingPV_ = 0;
		*/

}

// This is written out fully (rather than simply connected SIGNALs to SIGNALs
// So that we can override it in ReadWritePVControl.
void AMReadOnlyPVControl::onPVConnected(bool) {

	// For the read-only control, just having the readPV is enough to be connected or disconnected.
	emit connected( isConnected() );

	// don't need to do anything else here.  isConnected() and canRead() come dynamically from the PV state canRead().
}

void AMReadOnlyPVControl::onPVError(int error) {
	// TODO: figure out how to handle this best.
	qDebug() << QString("Process Variable error %1: %2.").arg(readPV_->pvName()).arg(error);
}

/* reuse for wStatus:
void AMReadOnlyPVControl::onMovingChanged(int isMoving) {
	emit moving(isMoving);

	if(wasMoving_ && !isMoving)
		onMoveEnded();
	if(!wasMoving_ && isMoving)
		onMoveStarted();

	wasMoving_ = isMoving;
}*/

void AMReadOnlyPVControl::onReadPVInitialized() {
	setUnits(readPV_->units());	// copy over the new unit string
	qDebug() << QString("ReadOnlyPVControl: read PV initialized correctly.");
}


AMPVControl::AMPVControl(const QString& name, const QString& readPVname, const QString& writePVname, double tolerance, double completionTimeoutSeconds, QObject* parent)
	: AMReadOnlyPVControl(name, readPVname, tolerance, parent)
{
	//not moving yet:
	mip_ = false;

	// not connected:
	wasConnected_ = false;

	// setpoint is initialized:
	setpoint_ = 0;

	// this is what to use for our timeout:
	completionTimeout_ = completionTimeoutSeconds;

	// connect the timer to the timeout handler:
	connect(&completionTimer_, SIGNAL(timeout()), this, SLOT(onCompletionTimeout()));

	// process variable:
	writePV_ = new DoubleProcessVariable(movePVName, true, this);
	connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onPVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeoutOccurred()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	// We now need to monitor the feedback position ourselves, to see if we get where we want to go:
	connect(readPV_, SIGNAL(valueChanged(double)), this, SLOT(onNewFeedbackValue(double)));
}

/// Start a move to the value setpoint:
// todo: figure out if dave and tom want handling for already-moving... (practical example: HV supply)
virtual void AMPVControl::move(double setpoint) {

	// new move target:
	setpoint_ = setpoint;

	// kill any old countdowns:
	completionTimer_.stop();

	if( canMove() ) {
		// Issue the move
		writePV_->setValue(moveTarget_);
		qDebug() << QString("Moving %1 to %2").arg(writePV_->pvName()).arg(moveTarget_);

		// We're now moving! Let's hope this hoofedinkus makes it...
		mip_ = true;

		// emit the signal that we started:
		emit this->moveStarted();

		// start the countdown to see if we get there in time or stall out: (completionTimeout_ is in seconds)
		completionTimer_.start(completionTimeout_*1000.0);
	}
	else {
		qDebug() << QString("Could not move %1 to %2").arg(writePV_->pvName()).arg(value);

		// Notify the failure right away:
		emit moveFailed(AMControl::NotConnectedFailure);
		// And we're definitely not moving.
		mip_ = false;
	}

}

/// This is used to check every new value, to see if we entered tolerance:
void AMPVControl::onNewFeedbackValue(double val) {

	// If we're not in the middle of a move, don't really care about changing values.
	if(!mip_)
		return;

	// Did we make it?
	if( inPosition() ) {
		// move is now done:
		mip_ = false;
		// disable the timer, so it doesn't trigger an error later
		completionTimer_.stop();
		// let everyone know we succeeded:
		emit moveSucceeded();

	}
}

/// This is used to handle the timeout of a move:
void AMPVControl::onCompletionTimeout() {

	// if we weren't moving, this shouldn't have happened. someone forgot to shutoff the timer?
	// todo: this is only included for state testing debugging... can remove if never happens
	if(!mip_) {
		qDebug() << "AMPVControl:: timer timeout while move not in progress.  How did this happen?";
		return;
	}

	// No matter what, this move is over:
	mip_ = false;
	completionTimer_.stop;

	// Did we make it?
	if( inPosition() ) {
		emit moveSucceeded();
	}
	// Didn't make it into position:
	else {
		emit moveFailed(AMControl::ToleranceFailure);
	}
}


/// This is called when a PV channel connects or disconnects
void AMPVControl::onPVConnected(bool connected) {

	// we'll receive this when either one connects or disconnects.
	// We need both connected to count as connected. That's not hard to figure out.
	// But, we should do some change detection so that if we're already disconnected,
	// we don't re-emit disconnected. (Or, emit disconnected after the first PV connects)

	// Losing the connection:
	if(wasConnected_ && !isConnected() ) {
		emit connected(wasConnected_ = false);
	}

	// gaining the connection:
	if(!wasConnected_ && isConnected() ) {
		emit connected(wasConnected_ = true);
	}
}

/// This is called when there is a PV channel error:
void AMPVControl::onPVError(int error) {
	// TODO: handle this better
	if(sender() == readPV_)
		qDebug() << QString("ProcessVariable error %1: %2.").arg(readPV_->pvName()).arg(error);
	if(sender() == writePV_)
		qDebug() << QString("ProcessVariable error %1: %2.").arg(writePV_->pvName()).arg(error);
}

/*

PVControl::PVControl(const QString& name, const QString& readPVname, const QString& writePVname, const QString& movingPVname, double tolerance, QObject* parent) : ReadOnlyPVControl(name, readPVname, movingPVname, parent) {

	tolerance_ = tolerance;

	writePV_ = new DoubleProcessVariable(writePVname, true, this);
	// TODO: check if this failed to connect channel. Also, handle disconnects/breakdowns.
	// connect(writePV_, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)));
	connect(writePV_, SIGNAL(connected(bool)), this, SLOT(onPVConnected(bool)));
	connect(writePV_, SIGNAL(error(int)), this, SLOT(onPVError(int)));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SIGNAL(writeConnectionTimeout()));
	connect(writePV_, SIGNAL(connectionTimeout()), this, SLOT(onConnectionTimeout()));

	// TODO: actually moving; write failed-->moveFailed; sort out signals.
	// TODO: decide if monitoring the set value is desireable (notif. that you exceeded limits?)
}

void PVControl::move(double value) {

	//onMoveStarted(); TODO: flag this manually if we have no connection from the movingPV_?

	moveTarget_ = value;
	// QString stringVersion = QString("%1").arg(moveTarget_, -1, 'e', 4);

	if( canMove() ) {
		writePV_->setValue(moveTarget_);
		qDebug() << QString("Moving %1 to %2").arg(writePV_->pvName()).arg(moveTarget_);
	}
	else {
		qDebug() << QString("Could not move %1 to %2").arg(writePV_->pvName()).arg(value);
		onMoveFailed();
	}


}

void PVControl::stop() {
	/// TODOTODOTODO!
}

void PVControl::onConnectionTimeout() {
	if(sender() == readPV_) {
		emit readConnectionTimeout();
		setState( state() & ~Control::CanMeasure );	// Disable CanMeasure flag
		emit connected(false);						// Not having a readPV_ makes us disconnected.
	}
	if(sender() == writePV_) {
		emit writeConnectionTimeout();
		setState( state() & ~Control::CanMove );	// disable CanMove flag
		emit connected(false);						// Not having a writePV_ makes us disconnected.
	}
}

void PVControl::onPVConnected(bool) {
	// This will happen if either the readPV_ or writePV_ connects, or disconnects. That makes lots of cases.
	// We can distinguish based on sender() == readPV_ or sender() == writePV_. But we don't have to.

	// Let's just check using our isConnected() function, which guarantees that we can read from readPV_ and write to writePV_.
	// It will re-check for connection status on these two PVs.
	emit connected( this->isConnected() );

	int state = Control::NotConnected;
	if( readPV_->canRead() )
//            state = Control::CanMeasure;
				state |= Control::CanMeasure;
	if (writePV_->canWrite() )
//            state = Control::CanMove;
				state |= Control::CanMove;

	setState(state);
}

void PVControl::onPVError(int error) {
	// TODO
	if(sender() == readPV_)
		qDebug() << QString("ProcessVariable error %1: %2.").arg(readPV_->pvName()).arg(error);
	if(sender() == writePV_)
		qDebug() << QString("ProcessVariable error %1: %2.").arg(writePV_->pvName()).arg(error);
}
*/
