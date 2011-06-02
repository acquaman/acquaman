#include "GeneralUtilities.h"

GeneralUtilities::GeneralUtilities()
{
}

QString GeneralUtilities::removeGreek(QString name)
{
	if (name.contains(QString::fromUtf8("α")))
		return name.replace(QString::fromUtf8("α"), "a");

	else if (name.contains(QString::fromUtf8("β")))
		return name.replace(QString::fromUtf8("β"), "b");

	else if (name.contains(QString::fromUtf8("γ")))
		return name.replace(QString::fromUtf8("γ"), "g");

	return name;
}

QString GeneralUtilities::addGreek(QString name)
{
	if (name.contains("a"))
		return name.replace("a", QString::fromUtf8("α"));

	else if (name.contains("b"))
		return name.replace("b", QString::fromUtf8("β"));

	else if (name.contains("g"))
		return name.replace("g", QString::fromUtf8("γ"));

	return name;
}
