/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#ifndef VESPERSSETSTRINGACTIONINFO_H
#define VESPERSSETSTRINGACTIONINFO_H

#include "actions3/AMActionInfo3.h"

/// This is a VESPERS specific action info used for setting PV's that are ascii arrays instead of strings.
class VESPERSSetStringActionInfo : public AMActionInfo3
{
	Q_OBJECT

	Q_PROPERTY (QString text READ text WRITE setText)

public:
	/// Constructor.  Requires the text that will be set.
	VESPERSSetStringActionInfo(const QString &newText, QObject *parent = 0);
	/// Copy constructor.
	VESPERSSetStringActionInfo(const VESPERSSetStringActionInfo &other);
	/// Destructor.
	virtual ~VESPERSSetStringActionInfo();

	/// This function is used as a virtual copy constructor.
	virtual AMActionInfo3 *createCopy() const;

	// Re-implemented public functions
	/////////////////////////////////

	/// This should describe the type of the action
	virtual QString typeDescription() const { return "VESPERS specific ascii setter."; }

	/// Returns the text that will be set.
	const QString &text() const { return text_; }
	/// Sets the new text.
	void setText(const QString &newText);

protected:
	/// The text for the string.
	QString text_;
};

#endif // VESPERSSETSTRINGACTIONINFO_H
