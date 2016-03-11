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


#ifndef ACQUAMAN_H
#define ACQUAMAN_H

#include <Qt>
#include <QStandardItem>
#include <QList>
#include <QEvent>
#include <QDateTime>
#include <QVector3D>

/// This namespace contains global definitions for the Acquaman / Dataman framework.

namespace AM {

	/// Application-wide role definitions used in the Qt StandardItem Model/View system
	enum ItemRoles { IdRole = Qt::UserRole+20, DateTimeRole, EndDateTimeRole, WeightRole, LinkRole, PointerRole, PriorityRole, LinePenRole, ModifiedRole, NameRole, DescriptionRole, DetailedDescriptionRole, WidgetRole, CanCloseRole, RankRole, UserRole };

	/// Application-wide type definitions used inthe Qt StandardItem Model/View system
	enum ItemTypes { DragDropItem = QStandardItem::UserType + 1, SidebarItem, SidebarHeadingItem };

	/// Acquaman-specific event types
	enum EventType { AcqEvent = QEvent::User+20, AcqErrorEvent, VideoResizeEvent, ItemChangedEvent, ThumbnailsGeneratedEvent };

	/// custom QGraphicsItem types (see qgraphicsitem_cast<>() for more information)
	enum GraphicsItemType { ThumbnailScrollGraphicsWidgetType = 65540 };

	/// Register acquaman-defined types with the QMetaType system. Call on application startup before using these types.
	void registerTypes();

}

class AMAcqEvent : public QEvent{
public:
	virtual ~AMAcqEvent();
	AMAcqEvent() : QEvent( (QEvent::Type)AM::AcqEvent)
	{}

	QMap<int, double> dataPackage_;
	QMap<int, QList<double> > spectraPackage_;
	QMap<int, double> extraPackage_;
};

class AMAcqErrorEvent : public QEvent{
public:
	virtual ~AMAcqErrorEvent();
	AMAcqErrorEvent() : QEvent( (QEvent::Type)AM::AcqErrorEvent)
	{}

	int errorCode_;
	QString errorExplanation_;
};

class AMHighPrecisionDateTime
{
public:
	virtual ~AMHighPrecisionDateTime(){}
	AMHighPrecisionDateTime(){}

	QDateTime dateTime() const { return dateTime_; }

	operator QDateTime() const { return dateTime_; }
	AMHighPrecisionDateTime& operator =(const QDateTime &dateTime) { setDateTime(dateTime); return *this;}

	void setDateTime(QDateTime dateTime) { dateTime_ = dateTime; }

protected:
	QDateTime dateTime_;
};

class AMDbObject;
class AMConstDbObject;
typedef QList<int> AMIntList;
typedef QList<double> AMDoubleList;
typedef QList<AMDbObject*> AMDbObjectList;
typedef QList<AMConstDbObject*> AMConstDbObjectList;
typedef QVector<QVector3D> AMQVector3DVector;

class AMDSClientDataRequest;
typedef QMap<QString, AMDSClientDataRequest*> AMDSClientDataRequestMap;

Q_DECLARE_METATYPE(AMIntList);
Q_DECLARE_METATYPE(AMDoubleList);
Q_DECLARE_METATYPE(AMDbObjectList);
Q_DECLARE_METATYPE(AMConstDbObjectList);
Q_DECLARE_METATYPE(AMHighPrecisionDateTime);
Q_DECLARE_METATYPE(AMQVector3DVector);
Q_DECLARE_METATYPE(AMDSClientDataRequestMap);

#endif // ACQUAMAN_H
