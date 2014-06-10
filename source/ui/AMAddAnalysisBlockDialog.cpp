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


#include "AMAddAnalysisBlockDialog.h"

#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "util/AMFontSizes.h"
#include "acquaman.h"

 AMAddAnalysisBlockDialog::~AMAddAnalysisBlockDialog(){}
AMAddAnalysisBlockDialog::AMAddAnalysisBlockDialog(int rank, QWidget *parent)
	: QDialog(parent)
{
	listWidget_ = new QListWidget(this);
	listWidget_->setViewMode(QListView::IconMode);

	connect(listWidget_, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(onCurrentItemChanged(QListWidgetItem*,QListWidgetItem*)));

	if (rank == 0 || rank == 1)
		populateWith1D();
	else if (rank == 2 || rank == 3)
		populateWith2D();

	QFont font;
	font.setWeight(AM_FONT_LARGE);
	font.setBold(true);

	QLabel *title = new QLabel("Add Analysis Block...");
	title->setFont(font);

	icon_ = new QLabel;

	name_ = new QLabel("[None Selected]");
	name_->setFont(font);

	description_ = new QLabel;
	description_->setWordWrap(true);
	font.setWeight(AM_FONT_REGULAR);
	description_->setFont(font);
	description_->setFixedWidth(200);

	addAnalysisButton_ = new QPushButton("Add Tool");
	addAnalysisButton_->setDefault(true);
	addAnalysisButton_->setEnabled(false);

	connect(addAnalysisButton_, SIGNAL(clicked()), this, SLOT(accept()));
	connect(listWidget_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));

	QVBoxLayout *descriptionLayout = new QVBoxLayout;
	descriptionLayout->addWidget(icon_);
	descriptionLayout->addWidget(name_);
	descriptionLayout->addWidget(description_);
	descriptionLayout->addStretch();
	descriptionLayout->addWidget(addAnalysisButton_);

	QHBoxLayout *bodyLayout = new QHBoxLayout;
	bodyLayout->addWidget(listWidget_);
	bodyLayout->addLayout(descriptionLayout);

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(title);
	mainLayout->addLayout(bodyLayout);

	setLayout(mainLayout);
	setMinimumSize(400, 400);
}

QString AMAddAnalysisBlockDialog::getAnalysisBlock(int rank, QWidget *parent)
{
	AMAddAnalysisBlockDialog *dialog = new AMAddAnalysisBlockDialog(rank, parent);
	return dialog->exec() ? dialog->name() : "";
}

QString AMAddAnalysisBlockDialog::name() const
{
	if (listWidget_->currentItem())
		return listWidget_->currentItem()->text();

	return "";
}

void AMAddAnalysisBlockDialog::onCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
	Q_UNUSED(previous)

	if (current){

		addAnalysisButton_->setEnabled(true);
		icon_->setPixmap(current->icon().pixmap(64));
		name_->setText(current->text());
		description_->setText(current->data(AM::DescriptionRole).toString());
	}

	else{

		addAnalysisButton_->setEnabled(false);
		icon_->setPixmap(QPixmap());
		name_->setText("[None Selected]");
		description_->setText("");
	}
}

void AMAddAnalysisBlockDialog::populateWith1D()
{
	QListWidgetItem *item = new QListWidgetItem(QIcon(":/derivative.png"), "Derivative", listWidget_);
	item->setData(AM::DescriptionRole, "This tool takes the derivative of its input.");

	item = new QListWidgetItem(QIcon(":/integral.png"), "Integral", listWidget_);
	item->setData(AM::DescriptionRole, "This tool takes the integral of its input.");

	item = new QListWidgetItem(QIcon(":/expression.png"), "Expression", listWidget_);
	item->setData(AM::DescriptionRole, "This tool analyzes any arbitrary expression.");

	item = new QListWidgetItem(QIcon(":/spectrum.png"), "2D Summing", listWidget_);
	item->setData(AM::DescriptionRole, "This tool bins a 2D image along an axis to generate a 1D function.");

	item = new QListWidgetItem(QIcon(":/22x22/gamma.png"), "Normalization", listWidget_);
	item->setData(AM::DescriptionRole, "This tool normalizes its input.");

	item = new QListWidgetItem(QIcon(":/22x22/gamma.png"), "Calibrated Normalization", listWidget_);
	item->setData(AM::DescriptionRole, "This tool normalizes its input and allows for energy calibration.");

	item = new QListWidgetItem(QIcon(":/spectrum.png"), "Interpolated Curve Correction", listWidget_);
	item->setData(AM::DescriptionRole, "This tool interpolates between two shift curves for finer correction.");

}

void AMAddAnalysisBlockDialog::populateWith2D()
{
	QListWidgetItem *item = new QListWidgetItem(QIcon(":/22x22/gamma.png"), "2D Map Normalization", listWidget_);
	item->setData(AM::DescriptionRole, "This tool normalizes its input.");

	item = new QListWidgetItem(QIcon(":/spectrum.png"), "3D Binning", listWidget_);
	item->setData(AM::DescriptionRole, "This tool bins a spectra to generate a 2D map.");
}

