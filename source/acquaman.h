#ifndef ACQUAMAN_H
#define ACQUAMAN_H

#include <Qt>
#include <QStandardItem>

/// This namespace contains global definitions for the Acquaman / Dataman framework.

namespace AM {

	/// Application-wide role definitions used in the Qt StandardItem Model/View system
	enum ItemRoles { IdRole = Qt::UserRole+1, DateTimeRole = Qt::UserRole + 2, WeightRole = Qt::UserRole + 9, LinkRole = Qt::UserRole+10 };

	/// Application-wide type definitions used inthe Qt StandardItem Model/View system
	enum ItemTypes { SidebarItem = QStandardItem::UserType + 1, SidebarHeadingItem, RunItem, ExperimentItem };


}
#endif // ACQUAMAN_H
