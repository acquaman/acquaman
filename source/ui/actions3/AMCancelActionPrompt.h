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


#ifndef AMCANCELACTIONPROMPT_H
#define AMCANCELACTIONPROMPT_H

#include <QDialog>

class QLabel;
class QCheckBox;

/// This is a simple QMessageBox that is extended by having a "do no warn me again" check box.
class AMCancelActionPrompt : public QDialog
{
	Q_OBJECT

public:
	/// Constructor.
 	virtual ~AMCancelActionPrompt();
	AMCancelActionPrompt(QWidget *parent = 0);

	/// Sets the text for the dialog.
	void setText(const QString &newText);
	/// Returns the current text.
	const QString &text() const { return text_; }

	/// Returns whether the warning check box is checked or not.
	bool shouldWarn() const;

protected:
	/// The string that holds the text.
	QString text_;
	/// The label holding for the text.
	QLabel *textLabel_;
	/// The check box for "do not warn me again".
	QCheckBox *warningCheckBox_;
};

#endif // AMCANCELACTIONPROMPT_H
