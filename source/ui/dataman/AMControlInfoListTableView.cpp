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


#include "AMControlInfoListTableView.h"

#include <QTableWidget>
#include <QBoxLayout>

#include "dataman/info/AMControlInfoList.h"

 AMControlInfoListTableView::~AMControlInfoListTableView(){}
AMControlInfoListTableView::AMControlInfoListTableView(const AMControlInfoList* list, QWidget *parent) :
	QWidget(parent)
{
	// setup UI:
	QVBoxLayout* vl = new QVBoxLayout(this);
	table_ = new QTableWidget();
	table_->setColumnCount(3);
	table_->setRowCount(0);
	table_->setHorizontalHeaderLabels(QStringList() << "Control" << "Position" << "Units");
	vl->addWidget(table_);

	// Populate the table
	setFromInfoList(list);
}

void AMControlInfoListTableView::setFromInfoList(const AMControlInfoList* list)
{
	table_->setRowCount(0);

	if(list) {
		table_->setRowCount(list->count());

		for(int i=0,cc=list->count(); i<cc; i++) {
			const AMControlInfo& info = list->at(i);

			QTableWidgetItem* item = new QTableWidgetItem(info.description().isEmpty() ? info.name() : info.description());
			table_->setItem(i, 0, item);

			item = new QTableWidgetItem(info.isEnum() ? info.enumString() : QString::number(info.value()));
			table_->setItem(i, 1, item);

			item = new QTableWidgetItem(info.units());
			table_->setItem(i, 2, item);
		}
	}
}
