#include "AMExporterOptionXDIFormat.h"

AMExporterOptionXDIFormat::AMExporterOptionXDIFormat(QObject *parent)
	: AMExporterOptionGeneralAscii(parent)
{
	elementSymbol_ = "";
	elementEdge_ = "";
}

AMExporterOptionXDIFormat::AMExporterOptionXDIFormat(const AMExporterOptionXDIFormat &original)
	: AMExporterOptionGeneralAscii(original)
{
	elementSymbol_ = original.elementSymbol();
	elementEdge_ = original.elementEdge();
}

AMExporterOptionXDIFormat::~AMExporterOptionXDIFormat()
{

}

AMExporterOption * AMExporterOptionXDIFormat::createCopy() const
{
	AMExporterOption *option = new AMExporterOptionXDIFormat(*this);
	option->dissociateFromDb(true);
	return option;
}

void AMExporterOptionXDIFormat::setElementSymbol(const QString &symbol)
{
	if (elementSymbol_ != symbol){

		elementSymbol_ = symbol;
		setModified(true);
	}
}

void AMExporterOptionXDIFormat::setElementEdge(const QString &edge)
{
	if (elementEdge_ != edge){

		elementEdge_ = edge;
		setModified(true);
	}
}
