#include "VESPERSBeamlineSetStringAction.h"
#include "application/VESPERS/VESPERS.h"

VESPERSBeamlineSetStringAction::VESPERSBeamlineSetStringAction(AMProcessVariable *pv, const QString &string, QObject *parent)
	: AMBeamlineActionItem(parent)
{
	pv_ = pv;
	string_ = string;
}

void VESPERSBeamlineSetStringAction::start()
{
	setStarted(true);
	connect(pv_, SIGNAL(valueChanged()), this, SLOT(onValueChanged()));
	VESPERS::stringToPV(pv_, string_);
}

void VESPERSBeamlineSetStringAction::onValueChanged()
{
	QString string = VESPERS::pvToString(pv_);

	if (string == string_){

		AMErrorMon::debug(this, VESPERSBEAMLINESETSTRINGACTION_SUCCEEDED, QString("[SUCCEEDED] Set string.  Desired: %1 Result: %2.").arg(string_).arg(string));
		setSucceeded(true);
	}

	else{

		AMErrorMon::debug(this, VESPERSBEAMLINESETSTRINGACTION_FAILED, QString("[FAILED] Set string.  Desired: %1 Result: %2.").arg(string_).arg(string));
		setFailed(true);
	}
}

void VESPERSBeamlineSetStringAction::cleanup()
{

}
