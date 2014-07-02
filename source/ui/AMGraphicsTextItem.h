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


#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H
#include <QGraphicsTextItem>

class QTextDocument;


/**
 * @brief The AMGraphicsTextItem class is a text item for displaying information about an AMShapeData/AMSample
 */

class AMGraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
	/// redefines its type as userType + 1 -> for item.type()
    enum {Type = UserType + 1};
    AMGraphicsTextItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);

	/// index of the shape it corresponds to
    void setShapeIndex(int index);
    int shapeIndex();
	/// the type of the item, used for identifying it in a scene, and casting it to the correct type
    int type() const;

	/// sets the text
    void setPlainText(const QString &text);
    bool selectAll();
public slots:
	/// selects all the text
    void selectAllText();
    void setSelectAll(const bool &selectAll);
	/// sets the cursor to the start of the text
	void clearSelection();

signals:
    void textChanged(int);
    void gotFocus(int);
    void returnPressed(int);

protected slots:
    void changingText();

protected:
	/// called when the text is clicked on
    void focusInEvent(QFocusEvent *event);
	/// called when the text loses focus
    void focusOutEvent(QFocusEvent *event);
	/// reimplements Enter and delete
    void keyPressEvent(QKeyEvent *event);
	/// used to select a point in the text
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected:
    QTextDocument* document_;

    int shapeIndex_;

    bool dontChangeSelection_;
    bool selectAll_;
};

#endif // GRAPHICSTEXTITEM_H
