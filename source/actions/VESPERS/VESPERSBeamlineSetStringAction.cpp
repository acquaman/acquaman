#include "VESPERSBeamlineSetStringAction.h"

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
	StringtoAMPV(pv_, string_);
}

void VESPERSBeamlineSetStringAction::onValueChanged()
{
	if (AMPVtoString(pv_) == string_){

		AMErrorMon::debug(this, VESPERSBEAMLINESETSTRINGACTION_SUCCEEDED, QString("[SUCCEEDED] Set string.  Desired: %1 Result: %2.").arg(string_).arg(AMPVtoString(pv_)));
		setSucceeded(true);
	}

	else{

		AMErrorMon::debug(this, VESPERSBEAMLINESETSTRINGACTION_FAILED, QString("[FAILED] Set string.  Desired: %1 Result: %2.").arg(string_).arg(AMPVtoString(pv_)));
		setFailed(true);
	}
}

void VESPERSBeamlineSetStringAction::cleanup()
{

}

QString VESPERSBeamlineSetStringAction::AMPVtoString(AMProcessVariable *pv) const
{
	int current;
	QString name;

	for (unsigned i = 0; i < pv->count(); i++){

		current = pv->getInt(i);
		if (current == 0)
			break;

		name += QString::fromAscii((const char *) &current);
	}

	return name;
}

void VESPERSBeamlineSetStringAction::StringtoAMPV(AMProcessVariable *pv, QString toConvert)
{
	int converted[256];

	QByteArray toConvertBA = toConvert.toAscii();
	for (int i = 0; i < 256; i++){

		if (i < toConvertBA.size())
			converted[i] = toConvertBA.at(i);
		else
			converted[i] = 0;
	}

	pv->setValues(converted, 256);
}
