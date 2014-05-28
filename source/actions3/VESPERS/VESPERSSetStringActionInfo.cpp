#include "VESPERSSetStringActionInfo.h"

VESPERSSetStringActionInfo::~VESPERSSetStringActionInfo(){}

VESPERSSetStringActionInfo::VESPERSSetStringActionInfo(const QString &newText, QObject *parent)
	: AMActionInfo3("VESPERS Set String Action", "VESPERS Set String Action", ":/system-run.png", parent)
{
	text_ = newText;
}

VESPERSSetStringActionInfo::VESPERSSetStringActionInfo(const VESPERSSetStringActionInfo &other)
	: AMActionInfo3(other)
{
	text_ = other.text();
}

void VESPERSSetStringActionInfo::setText(const QString &newText)
{
	if (newText != text_){

		text_ = newText;
		setModified(true);
	}
}
