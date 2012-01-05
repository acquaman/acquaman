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


#ifndef GENERALUTILITIES_H
#define GENERALUTILITIES_H

#include <QString>

class GeneralUtilities
{
public:

	/// Takes an emission line name and turns the greek letter into a roman equivalent and returns the result (ie: alpha -> a).
	static QString removeGreek(QString name);
	/// Takes an ASCII plain text emission line name and adds the corresponding greek letter to it (ie: Ka1 -> Kalpha1).
	static QString addGreek(QString name);
};

#endif // GENERALUTILITIES_H
