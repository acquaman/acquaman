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


#include "AMCurrentAmplifierView.h"


AMCurrentAmplifierView::AMCurrentAmplifierView(QWidget *parent) :
	QWidget(parent)
{
	initialized_ = false;

	viewMode_ = Basic;

	precision_ = 2;
	format_ = 'f';

	name_ = new QLabel();
	name_->hide();

	minus_ = new QToolButton();
	minus_->show();
	minus_->setMaximumSize(25, 25);
	minus_->setIcon(QIcon(":/22x22/list-remove.png"));
	connect( minus_, SIGNAL(clicked()), this, SLOT(onMinusClicked()) );

	plus_ = new QToolButton();
	plus_->show();
	plus_->setMaximumSize(25, 25);
	plus_->setIcon(QIcon(":/22x22/list-add.png"));
	connect( plus_, SIGNAL(clicked()), this, SLOT(onPlusClicked()) );

	value_ = new QComboBox();
	value_->setMaxVisibleItems(5);
	value_->hide();
	connect( value_, SIGNAL(currentIndexChanged(QString)), this, SLOT(onValueComboBoxChanged(QString)) );

	layout_ = new QHBoxLayout();

	layout_->addWidget(name_);
	layout_->addWidget(minus_);
	layout_->addWidget(plus_);
	layout_->addWidget(value_);

	setLayout(layout_);

	// initialize the context menu for view control
	basicViewAction_ = new QAction("Basic view", this);
	advancedViewAction_ = new QAction("Advanced view", this);
	connect( basicViewAction_, SIGNAL(triggered()), this, SLOT(onBasicViewActionTriggered()) );
	connect( advancedViewAction_, SIGNAL(triggered()), this, SLOT(onAdvancedViewActionTriggered()) );

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect( this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

	value_->setContextMenuPolicy(Qt::CustomContextMenu);
	connect( value_->view(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)) );

	setInitialized(true);
}

AMCurrentAmplifierView::~AMCurrentAmplifierView()
{

}

bool AMCurrentAmplifierView::initialized() const
{
	return initialized_;
}

AMCurrentAmplifierView::ViewMode AMCurrentAmplifierView::viewMode() const
{
	return viewMode_;
}

int AMCurrentAmplifierView::precision() const
{
	return precision_;
}

char AMCurrentAmplifierView::format() const
{
	return format_;
}

QString AMCurrentAmplifierView::name() const
{
	return name_->text();
}

void AMCurrentAmplifierView::setViewMode(ViewMode newMode)
{
	if (viewMode_ != newMode) {
		viewMode_ = newMode;

		if (viewMode_ == Basic) {
			value_->hide();
			plus_->show();
			minus_->show();

		} else {
			value_->show();
			plus_->hide();
			minus_->hide();
		}

		emit viewModeChanged(viewMode_);
	}
}

void AMCurrentAmplifierView::setPrecision(int newPrecision)
{
	if (newPrecision > 0 && precision_ != newPrecision) {
		precision_ = newPrecision;
		refreshView();
		emit precisionChanged(precision_);
	}
}

void AMCurrentAmplifierView::setFormat(char newFormat)
{
	if ((newFormat == 'e' || newFormat == 'f' || newFormat == 'g') && format_ != newFormat) {
		format_ = newFormat;
		refreshView();
		emit formatChanged(format_);
	}
}

void AMCurrentAmplifierView::setName(const QString &newName)
{
	if (name_->text() != newName) {
		name_->setText(newName);
		emit nameChanged(name_->text());
	}
}

void AMCurrentAmplifierView::showName(bool show)
{
	if (show)
		name_->show();
	else
		name_->hide();
}

void AMCurrentAmplifierView::setViewableValuesMax(int newMax)
{
	value_->setMaxVisibleItems(newMax);
}

void AMCurrentAmplifierView::setInitialized(bool isInitialized)
{
	if (initialized_ != isInitialized) {
		initialized_ = isInitialized;
		emit initialized(initialized_);
	}
}

void AMCurrentAmplifierView::onValueComboBoxChanged(const QString &newText)
{
	if (initialized_ && isValid())
		onValueComboBoxChangedImplementation(newText);
}

void AMCurrentAmplifierView::onMinusClicked()
{
	if (isValid())
		onMinusClickedImplementation();
}

void AMCurrentAmplifierView::onPlusClicked()
{
	if (isValid())
		onPlusClickedImplementation();
}

void AMCurrentAmplifierView::refreshView()
{
	// initialized_ = false here prevents the view from changing the amplifier value while populating <-- undesireable behavior.
	if (isValid()) {
		initialized_ = false;
		refreshViewImplementation();
		initialized_ = true;
	}
}

void AMCurrentAmplifierView::onCustomContextMenuRequested(QPoint position)
{
	if (isValid()) {
		basicViewAction_->setEnabled(viewMode_ != Basic);
		advancedViewAction_->setEnabled(viewMode_ != Advanced);

		QMenu contextMenu(this);
		contextMenu.addAction(basicViewAction_);
		contextMenu.addAction(advancedViewAction_);

		onCustomContextMenuActionImplementation(&contextMenu);

		contextMenu.exec(mapToGlobal(position));
	}
}

void AMCurrentAmplifierView::onBasicViewActionTriggered()
{
	setViewMode(Basic);
}

void AMCurrentAmplifierView::onAdvancedViewActionTriggered()
{
	setViewMode(Advanced);
}

QString AMCurrentAmplifierView::toDisplay(double value, const QString &units) const
{
	return QString("%1 %2").arg(value, 0, format_, precision_).arg(units);
}
