#ifndef ACQUAMAN_H
#define ACQUAMAN_H

#include <Qt>
#include <QStandardItem>
#include <QList>


/// This namespace contains global definitions for the Acquaman / Dataman framework.

namespace AM {

	/// Application-wide role definitions used in the Qt StandardItem Model/View system
	enum ItemRoles { IdRole = Qt::UserRole+20, DateTimeRole, WeightRole, LinkRole, PointerRole, PriorityRole, LinePenRole, ModifiedRole, DescriptionRole, WidgetRole };

	/// Application-wide type definitions used inthe Qt StandardItem Model/View system
	enum ItemTypes { SidebarItem = QStandardItem::UserType + 1, SidebarHeadingItem, RunItem, ExperimentItem };


	/// Application-wide QVariant user types
	enum AcquamanType { IntList = QVariant::UserType + 20, DoubleList };

}


typedef QList<int> AMIntList;
typedef QList<double> AMDoubleList;

Q_DECLARE_METATYPE(AMIntList);
Q_DECLARE_METATYPE(AMDoubleList);

#endif // ACQUAMAN_H
