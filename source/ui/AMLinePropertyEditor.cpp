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


#include "AMLinePropertyEditor.h"
#include "ui/dataman/AMColorPickerButton.h"
#include <QPen>
#include <QBrush>
#include <QApplication>

AMLinePropertyEditor::AMLinePropertyEditor(QWidget *parent) :
	QFrame(parent)
{
	setupUi();

	fillColorButton->setEnabled(false);
	connect(fillCheckBox, SIGNAL(clicked(bool)), fillColorButton, SLOT(setEnabled(bool)));

	connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(widthSlider, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(lineStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenSettingsChanged()));

	connect(fillCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(fillColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));
}

AMLinePropertyEditor::AMLinePropertyEditor(const QPen& initialPen, bool areaFilled, const QBrush& areaFillColor, QWidget* parent) :
	QFrame(parent)
{
	setupUi();

	colorButton->setColor(initialPen.color());
	widthSlider->setValue(initialPen.width());
	lineStyleBox->setCurrentIndex(qBound(0, initialPen.style()-1, 4));

	fillCheckBox->setChecked(areaFilled);
	fillColorButton->setColor(areaFillColor.color());

	fillColorButton->setEnabled(areaFilled);
	connect(fillCheckBox, SIGNAL(clicked(bool)), fillColorButton, SLOT(setEnabled(bool)));

	connect(colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(widthSlider, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(lineStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenSettingsChanged()));

	connect(fillCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(fillColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));
}

void AMLinePropertyEditor::onPenSettingsChanged()
{
	emit linePenChanged(QPen(colorButton->color(),
							 widthSlider->value(),
							 (Qt::PenStyle)(lineStyleBox->currentIndex()+1)));
}

void AMLinePropertyEditor::onAreaFillColorChanged(const QColor &color)
{
	emit areaFillBrushChanged(QBrush(color));
}

void AMLinePropertyEditor::setupUi()
{
	if (objectName().isEmpty())
		setObjectName(QString::fromUtf8("AMLinePropertyEditor"));
	resize(159, 193);
	setStyleSheet(QString::fromUtf8("#AMLinePropertyEditor {\n"
				"	background-color: rgba(0, 0, 0, 179);\n"
				"}"));
	setFrameShape(QFrame::NoFrame);
	setFrameShadow(QFrame::Plain);
	verticalLayout = new QVBoxLayout(this);
	verticalLayout->setContentsMargins(6, 6, 6, 6);
	verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
	horizontalLayout = new QHBoxLayout();
	horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
	labelColor = new QLabel(this);
	labelColor->setObjectName(QString::fromUtf8("labelColor"));
	labelColor->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));

	horizontalLayout->addWidget(labelColor);

	colorButton = new AMColorPickerButton(this);
	colorButton->setObjectName(QString::fromUtf8("colorButton"));

	horizontalLayout->addWidget(colorButton);


	verticalLayout->addLayout(horizontalLayout);

	horizontalLayout_2 = new QHBoxLayout();
	horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
	labelWidth = new QLabel(this);
	labelWidth->setObjectName(QString::fromUtf8("labelWidth"));
	labelWidth->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));

	horizontalLayout_2->addWidget(labelWidth);

	widthSlider = new QSlider(this);
	widthSlider->setObjectName(QString::fromUtf8("widthSlider"));
	widthSlider->setMinimum(1);
	widthSlider->setMaximum(10);
	widthSlider->setPageStep(2);
	widthSlider->setOrientation(Qt::Horizontal);
	widthSlider->setTickPosition(QSlider::TicksBelow);
	widthSlider->setTickInterval(2);

	horizontalLayout_2->addWidget(widthSlider);


	verticalLayout->addLayout(horizontalLayout_2);

	lineStyleBox = new QComboBox(this);
	lineStyleBox->setObjectName(QString::fromUtf8("lineStyleBox"));

	verticalLayout->addWidget(lineStyleBox);

	verticalSpacer = new QSpacerItem(20, 10, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

	verticalLayout->addItem(verticalSpacer);

	horizontalLayout_3 = new QHBoxLayout();
	horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
	labelFill = new QLabel(this);
	labelFill->setObjectName(QString::fromUtf8("labelFill"));
	labelFill->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));

	horizontalLayout_3->addWidget(labelFill);

	horizontalSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

	horizontalLayout_3->addItem(horizontalSpacer);

	fillCheckBox = new QCheckBox(this);
	fillCheckBox->setObjectName(QString::fromUtf8("fillCheckBox"));

	horizontalLayout_3->addWidget(fillCheckBox);


	verticalLayout->addLayout(horizontalLayout_3);

	horizontalLayout_4 = new QHBoxLayout();
	horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
	labelFillColor = new QLabel(this);
	labelFillColor->setObjectName(QString::fromUtf8("labelFillColor"));
	labelFillColor->setStyleSheet(QString::fromUtf8("color: rgb(230, 230, 230);"));

	horizontalLayout_4->addWidget(labelFillColor);

	fillColorButton = new AMColorPickerButton(this);
	fillColorButton->setObjectName(QString::fromUtf8("fillColorButton"));

	horizontalLayout_4->addWidget(fillColorButton);


	verticalLayout->addLayout(horizontalLayout_4);


	setWindowTitle(QApplication::translate("AMLinePropertyEditor", "Frame", 0, QApplication::UnicodeUTF8));
	labelColor->setText(QApplication::translate("AMLinePropertyEditor", "Color", 0, QApplication::UnicodeUTF8));
	colorButton->setText(QApplication::translate("AMLinePropertyEditor", "...", 0, QApplication::UnicodeUTF8));
	labelWidth->setText(QApplication::translate("AMLinePropertyEditor", "Width", 0, QApplication::UnicodeUTF8));
	lineStyleBox->clear();
	lineStyleBox->insertItems(0, QStringList()
	 << QApplication::translate("AMLinePropertyEditor", "\342\216\257\342\216\257\342\216\257\342\216\257\342\216\257\342\216\257\342\216\257", 0, QApplication::UnicodeUTF8)
	 << QApplication::translate("AMLinePropertyEditor", "\342\200\224  \342\200\224  \342\200\224  \342\200\224  \342\200\224", 0, QApplication::UnicodeUTF8)
	 << QApplication::translate("AMLinePropertyEditor", "\302\267 \302\267 \302\267 \302\267 \302\267 \302\267 \302\267 \302\267 \302\267 \302\267 \302\267", 0, QApplication::UnicodeUTF8)
	 << QApplication::translate("AMLinePropertyEditor", "\342\200\224 \302\267 \342\200\224 \302\267 \342\200\224 \302\267 \342\200\224", 0, QApplication::UnicodeUTF8)
	 << QApplication::translate("AMLinePropertyEditor", " \342\200\224 \302\267 \302\267 \342\200\224 \302\267 \302\267 \342\200\224", 0, QApplication::UnicodeUTF8)
	);
	labelFill->setText(QApplication::translate("AMLinePropertyEditor", "Area Fill", 0, QApplication::UnicodeUTF8));
	fillCheckBox->setText(QString());
	labelFillColor->setText(QApplication::translate("AMLinePropertyEditor", "Fill Color", 0, QApplication::UnicodeUTF8));
	fillColorButton->setText(QApplication::translate("AMLinePropertyEditor", "...", 0, QApplication::UnicodeUTF8));
}
