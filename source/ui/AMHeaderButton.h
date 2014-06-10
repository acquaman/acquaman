/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

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


#ifndef AMHEADERBUTTON_H
#define AMHEADERBUTTON_H

#include <QToolButton>
#include <QMetaType>

/// This class provides a styled QToolButton with a look and feel appropriate for a title strip across the tops of other widgets. The button's clickability is used by AMVerticalStackWidget to trigger hiding and unhiding of the sub-widgets.
class AMHeaderButton : public QToolButton
{
Q_OBJECT
public:
 	virtual ~AMHeaderButton();
	explicit AMHeaderButton(QWidget *parent = 0);


	void setArrowType(Qt::ArrowType type);

	Qt::ArrowType arrowType() const { return arrowType_; }

signals:

public slots:
	void setText(const QString &text);

protected:
	Qt::ArrowType arrowType_;

	/// re-implemented from QToolButton to display arrows way over on the right, and keep the name shown
	virtual void paintEvent(QPaintEvent *);

	QPixmap arrowPix;


};

Q_DECLARE_METATYPE(AMHeaderButton*);

#endif // AMHEADERBUTTON_H
