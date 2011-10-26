/*
Copyright 2010, 2011 Mark Boots, David Chevrier, and Darren Hunter.

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


#include "GeneralUtilities.h"

QString GeneralUtilities::removeGreek(QString name)
{
	if (name.contains(QString::fromUtf8("α")))
		return name.replace(name.lastIndexOf(QString::fromUtf8("α")), 1, "a");

	else if (name.contains(QString::fromUtf8("β")))
		return name.replace(name.lastIndexOf(QString::fromUtf8("β")), 1, "b");

	else if (name.contains(QString::fromUtf8("γ")))
		return name.replace(name.lastIndexOf(QString::fromUtf8("γ")), 1, "g");

	return name;
}

QString GeneralUtilities::addGreek(QString name)
{
	if (name.contains("a"))
		return name.replace(name.lastIndexOf("a"), 1, QString::fromUtf8("α"));

	else if (name.contains("b"))
		return name.replace(name.lastIndexOf("b"), 1, QString::fromUtf8("β"));

	else if (name.contains("g"))
		return name.replace(name.lastIndexOf("g"), 1, QString::fromUtf8("γ"));

	return name;
}
