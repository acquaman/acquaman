/*
Copyright 2010, 2011 Mark Boots, David Chevrier.

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


#include "AMStatusView.h"
#include <QDateTime>
#include "util/AMSettings.h"
#include <QHBoxLayout>
#include "dataman/AMUser.h"

#include <QHeaderView>

AMStatusView::AMStatusView(QWidget *parent) : QAbstractButton(parent)
{
	/// widget layout
	hl_ = new QHBoxLayout();

	/// Icons representing the nature of the last notification
	iconInfo_ = new QLabel("info!");
	iconInfo_->setPixmap(QPixmap(":/dialog-information.png"));
	hl_->addWidget(iconInfo_);
	iconAlert_ = new QLabel("alert!");
	iconAlert_->setPixmap(QPixmap(":/dialog-warning.png"));
	hl_->addWidget(iconAlert_);
	iconSerious_ = new QLabel("serious!");
	iconSerious_->setPixmap(QPixmap(":/dialog-error.png"));
	hl_->addWidget(iconSerious_);
	iconDebug_ = new QLabel("debug!");
	iconDebug_->setPixmap(QPixmap(":/applications-development.png"));
	hl_->addWidget(iconDebug_);

	/// Hide inactive:
	iconAlert_->hide();
	iconSerious_->hide();
	iconDebug_->hide();
	currentIcon_ = iconInfo_;

	setFixedWidth(250);

	/// last notification text (
	shortText_ = new QLabel();
	shortText_->setWordWrap(true);
	QFont font = shortText_->font();
	font.setPointSize(font.pointSize()-2);
	shortText_->setFont(font);
	hl_->addWidget(shortText_, 1, Qt::AlignLeft | Qt::AlignVCenter);

	// hl_->addStretch(1);
	hl_->addStrut(32);
	hl_->setMargin(0);

	this->setLayout(hl_);

	/// pickup all connections from ErrorMon
	connect(AMErrorMon::mon(), SIGNAL(error(AMErrorReport)), this, SLOT(onAnyError(AMErrorReport)));

	logView_ = new AMStatusLogView(this);

	setToolTip("Click for details... [Show log]");
	connect(this, SIGNAL(clicked()), logView_, SLOT(show()));
	// logView_->show();

	onAnyError(AMErrorReport(0, AMErrorReport::Information, 0, QString("Acquaman: Welcome %1").arg(AMUser::user()->name())));

}

/// Handles any errors that are logged using AMErrorMon::report(AMErrorCode())
void AMStatusView::onAnyError(AMErrorReport e) {

	QString msg;

	/// switch icons:
	currentIcon_->hide();

	switch(e.level) {
	case AMErrorReport::Information:
		currentIcon_ = iconInfo_;
		break;

	case AMErrorReport::Alert:
		currentIcon_ = iconAlert_;
		msg.append("Alert: ");
		break;

	case AMErrorReport::Serious:
		currentIcon_ = iconSerious_;
		msg.append("Error: ");
		break;

	case AMErrorReport::Debug:
		currentIcon_ = iconDebug_;
		msg.append("Debug Message: ");
		break;
	}

	currentIcon_->show();

	/// Append class name of source, if available:
	if(e.source) {
		msg.append(e.source->metaObject()->className()).append(": ");
	}

	msg.append(QString("%1 (%2).").arg(e.description).arg(e.errorCode));

	/// Display as screen status:
	shortText_->setText(shortText_->fontMetrics().elidedText(msg, Qt::ElideRight, shortText_->width()*2));

	/// Log to detail window:
	logView_->addError(e);

}

AMStatusLogView::AMStatusLogView(QWidget *parent)
	: QWidget(parent, Qt::Tool)
{
	logModel_ = new QStandardItemModel(0, 5, this);
	logView_ = new QTableView();
	logView_->setModel(logModel_);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->setContentsMargins(0,0,0,0);
	vl->addWidget(logView_);
	setLayout(vl);

	logModel_->setHeaderData(0, Qt::Horizontal, QString());
	logModel_->setHeaderData(1, Qt::Horizontal, "Message");
	logModel_->setHeaderData(2, Qt::Horizontal, "From");
	logModel_->setHeaderData(3, Qt::Horizontal, "Time");
	logModel_->setHeaderData(4, Qt::Horizontal, "Code");

	debugIcon_ = QIcon(":/applications-development.png");
	infoIcon_ = QIcon(":/dialog-information.png");
	alertIcon_ = QIcon(":/dialog-warning.png");
	seriousIcon_ = QIcon(":/dialog-error.png");

	logView_->setShowGrid(false);
	logView_->setWordWrap(true);
	logView_->setAlternatingRowColors(true);

	QFont font = logView_->font();
	font.setPointSize(font.pointSize()-2);
	logView_->setFont(font);

	logView_->verticalHeader()->hide();
	logView_->horizontalHeader()->setResizeMode(0, QHeaderView::Interactive);
	logView_->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
	logView_->horizontalHeader()->setResizeMode(2, QHeaderView::Interactive);
	logView_->horizontalHeader()->setResizeMode(3, QHeaderView::Interactive);
	logView_->horizontalHeader()->setResizeMode(4, QHeaderView::Interactive);
	logView_->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
	logView_->setColumnWidth(0, 24);
	logView_->setColumnWidth(2, 80);
	logView_->setColumnWidth(3, 80);
	logView_->setColumnWidth(4, 40);
	logView_->setMinimumWidth(320);

	logView_->setSelectionBehavior(QAbstractItemView::SelectRows);
	logView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	logView_->setEditTriggers(QTableView::NoEditTriggers);

	resize(400, 120);
}


void AMStatusLogView::addError(const AMErrorReport &e)
{
	QStandardItem* icon, *time, *message, *from, *errorCode;

	switch(e.level) {
	case AMErrorReport::Debug:
		icon = new QStandardItem(debugIcon_, QString() );
		icon->setToolTip("Debug");
		break;
	case AMErrorReport::Information:
		icon = new QStandardItem(infoIcon_, QString() );
		icon->setToolTip("Information");
		break;
	case AMErrorReport::Alert:
		icon = new QStandardItem(alertIcon_, QString() );
		icon->setToolTip("Alert");
		break;
	case AMErrorReport::Serious:
		icon = new QStandardItem(seriousIcon_, QString() );
		icon->setToolTip("Serious");
		break;
	}

	icon->setData(Qt::AlignTop, Qt::DecorationPropertyRole);

	time = new QStandardItem(QDateTime::currentDateTime().toString("h:mm:ss ap MMM d"));

	message = new QStandardItem(e.description);

	QString fromObjectName;
	if(e.source)
		fromObjectName = e.source->metaObject()->className();
	from = new QStandardItem(fromObjectName);

	errorCode = new QStandardItem(QString("%1").arg(e.errorCode));

	QList<QStandardItem*> list;
	list << icon << message << from << time << errorCode;

	logModel_->appendRow(list);
}

