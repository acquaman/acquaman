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

#include "ui/util/AMPeriodicTableDialog.h"

#include "ui/util/AMPeriodicTableView.h"

#include <QHBoxLayout>

 AMPeriodicTableDialog::~AMPeriodicTableDialog(){}
AMPeriodicTableDialog::AMPeriodicTableDialog(QWidget *parent)
	: QDialog(parent)
{
	setWindowTitle("Choose an element");

	element_ = 0;

	AMPeriodicTableView *ptView = new AMPeriodicTableView;
	connect(ptView, SIGNAL(elementSelected(AMElement*)), this, SLOT(onElementSelected(AMElement*)));

	QHBoxLayout *layout = new QHBoxLayout;
	layout->addWidget(ptView);

	setLayout(layout);
}

void AMPeriodicTableDialog::onElementSelected(AMElement *el)
{
	element_ = el;
	accept();
}

AMElement *AMPeriodicTableDialog::getElement(QWidget *parent)
{
	AMPeriodicTableDialog dialog(parent);

	return dialog.exec() ? dialog.element() : 0;
}

