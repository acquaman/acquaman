#include "AMExporterOptionSMAK.h"

AMExporterOptionSMAK::AMExporterOptionSMAK(QObject *parent)
	: AMExporterOptionGeneralAscii(parent)
{
	regExp_ = "";
}

AMExporterOptionSMAK::AMExporterOptionSMAK(const AMExporterOptionSMAK &original)
	: AMExporterOptionGeneralAscii(original)
{
	regExp_ = original.regExpString();
}

AMExporterOptionSMAK::~AMExporterOptionSMAK()
{

}

AMExporterOption *AMExporterOptionSMAK::createCopy() const
{
	AMExporterOption *option = new AMExporterOptionSMAK(*this);
	option->dissociateFromDb(true);
	return option;
}

void AMExporterOptionSMAK::setRegExpString(const QString &regExp)
{
	regExp_ = regExp;
	setModified(true);
}
