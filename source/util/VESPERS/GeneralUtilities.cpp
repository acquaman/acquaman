#include "GeneralUtilities.h"

GeneralUtilities::GeneralUtilities(QObject *parent) :
	QObject(parent)
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
