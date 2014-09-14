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


#include "AMStatusView.h"
#include <QDateTime>
#include "util/AMSettings.h"
#include <QHBoxLayout>
#include "dataman/AMUser.h"

#include <QHeaderView>
#include <QSystemTrayIcon>

AMStatusView::~AMStatusView(){}

AMStatusView::AMStatusView(QWidget *parent) : QAbstractButton(parent)
{
	connect(&systemTrayIconFunctionCall_, SIGNAL(executed()), this, SLOT(handleSystemTrayIconRequests()));

	sicon_ = new QSystemTrayIcon(QIcon(":/utilities-system-monitor.png"), this);
	sicon_->show();

	/// widget layout
	hl_ = new QHBoxLayout();

	/// Icons representing the nature of the last notification
	iconInfo_ = new QLabel("info!");
	iconInfo_->setStyleSheet("border: 0px;");
	iconInfo_->setPixmap(QPixmap(":/dialog-information.png"));
	hl_->addWidget(iconInfo_);
	iconAlert_ = new QLabel("alert!");
	iconAlert_->setStyleSheet("border: 0px;");
	iconAlert_->setPixmap(QPixmap(":/dialog-warning.png"));
	hl_->addWidget(iconAlert_);
	iconSerious_ = new QLabel("serious!");
	iconSerious_->setStyleSheet("border: 0px;");
	iconSerious_->setPixmap(QPixmap(":/dialog-error.png"));
	hl_->addWidget(iconSerious_);
	iconDebug_ = new QLabel("debug!");
	iconDebug_->setStyleSheet("border: 0px;");
	iconDebug_->setPixmap(QPixmap(":/applications-development.png"));
	hl_->addWidget(iconDebug_);

	/// Hide inactive:
	iconAlert_->hide();
	iconSerious_->hide();
	iconDebug_->hide();
	currentIcon_ = iconInfo_;

	setFixedWidth(350);

	/// last notification text (
	shortText_ = new QLabel();
	shortText_->setWordWrap(true);
	shortText_->setStyleSheet("border: 0px;");
	QPalette palette = shortText_->palette();
	palette.setColor(QPalette::ButtonText, Qt::white);
	shortText_->setPalette(palette);
	hl_->addWidget(shortText_, 1, Qt::AlignLeft | Qt::AlignVCenter);

	hl_->addStrut(32);
	hl_->setMargin(0);

	this->setLayout(hl_);

	/// pickup all connections from ErrorMon
	connect(AMErrorMon::mon(), SIGNAL(error(AMErrorReport)), this, SLOT(onAnyError(AMErrorReport)));

	logView_ = new AMStatusLogView(this);
	logView_->setStyleSheet("QTableView { alternate-background-color: light gray; background-color: white; } QTableView::QHeaderView::section { backround-color: white }");

	setToolTip("Click for details... [Show log]");
	connect(this, SIGNAL(clicked()), logView_, SLOT(show()));

	onAnyError(AMErrorReport(0, AMErrorReport::Information, 0, QString("Acquaman: Welcome %1").arg(AMUser::user()->name())));

}

/// Handles any errors that are logged using AMErrorMon::report(AMErrorCode())
void AMStatusView::onAnyError(AMErrorReport e) {
	QString msg;

	/// switch icons:
	currentIcon_->hide();

	QString reportMessage = QString("%1 (code %2)").arg(e.description).arg(e.errorCode);

	switch(e.level) {
	case AMErrorReport::Information:
		currentIcon_ = iconInfo_;
		messagesToReport_.append(QString("%1||=||%2").arg(reportMessage).arg("Information"));
		systemTrayIconFunctionCall_.runLater(1000);
		break;

	case AMErrorReport::Alert:
		currentIcon_ = iconAlert_;
		msg.append("Alert: ");
		messagesToReport_.append(QString("%1||=||%2").arg(reportMessage).arg("Alert"));
		systemTrayIconFunctionCall_.runLater(1000);
		break;

	case AMErrorReport::Serious:
		currentIcon_ = iconSerious_;
		msg.append("Error: ");
		messagesToReport_.append(QString("%1||=||%2").arg(reportMessage).arg("Error"));
		systemTrayIconFunctionCall_.runLater(1000);
		break;

	case AMErrorReport::Debug:
		currentIcon_ = iconDebug_;
		msg.append("Debug Message: ");
		break;
	}

	currentIcon_->show();

	msg.append(QString("%1 (%2).").arg(e.description).arg(e.errorCode));

	/// Display as screen status:
	shortText_->setText(shortText_->fontMetrics().elidedText(msg, Qt::ElideRight, shortText_->width()*2));

	/// Log to detail window:
	logView_->addError(e);
}

void AMStatusView::handleSystemTrayIconRequests(){
	QString tempString;
	while(messagesToReport_.count() > 0){
		tempString = messagesToReport_.takeFirst();
		if(tempString.section("||=||", -1) == "Information")
			sicon_->showMessage(QString("Information:"), tempString.section("||=||", 0, 1), QSystemTrayIcon::Information, 5000);
		else if(tempString.section("||=||", -1) == "Alert")
			sicon_->showMessage(QString("Alert:"), tempString.section("||=||", 0, 1), QSystemTrayIcon::Warning, 5000);
		else if(tempString.section("||=||", -1) == "Error")
			sicon_->showMessage(QString("Serious Error:"), tempString.section("||=||", 0, 1), QSystemTrayIcon::Critical, 5000);
	}
}

 AMStatusLogView::~AMStatusLogView(){}
AMStatusLogView::AMStatusLogView(QWidget *parent)
	: QWidget(parent, Qt::Tool)
{
	logModel_ = new QStandardItemModel(0, 5, this);
	logView_ = new QTableView();
	logView_->setModel(logModel_);

	enableDebuggingCheckBox_ = new QCheckBox("Enable Debugging");
	QHBoxLayout *hl = new QHBoxLayout();
	hl->addWidget(enableDebuggingCheckBox_);

	QVBoxLayout* vl = new QVBoxLayout();
	vl->setContentsMargins(0,0,0,0);
	vl->addLayout(hl);
	vl->addWidget(logView_);
	setLayout(vl);

	logModel_->setHeaderData(0, Qt::Horizontal, QString());
	logModel_->setHeaderData(0, Qt::Horizontal, Qt::lightGray, Qt::BackgroundRole);
	logModel_->setHeaderData(1, Qt::Horizontal, "Message");
	logModel_->setHeaderData(1, Qt::Horizontal, Qt::lightGray, Qt::BackgroundRole);
	logModel_->setHeaderData(2, Qt::Horizontal, "From");
	logModel_->setHeaderData(2, Qt::Horizontal, Qt::lightGray, Qt::BackgroundRole);
	logModel_->setHeaderData(3, Qt::Horizontal, "Time");
	logModel_->setHeaderData(3, Qt::Horizontal, Qt::lightGray, Qt::BackgroundRole);
	logModel_->setHeaderData(4, Qt::Horizontal, "Code");
	logModel_->setHeaderData(4, Qt::Horizontal, Qt::lightGray, Qt::BackgroundRole);

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
	logView_->setColumnWidth(2, 120);
	logView_->setColumnWidth(3, 80);
	logView_->setColumnWidth(4, 60);
	logView_->setMinimumWidth(320);

	logView_->setSelectionBehavior(QAbstractItemView::SelectRows);
	logView_->setSelectionMode(QAbstractItemView::ExtendedSelection);
	logView_->setEditTriggers(QTableView::NoEditTriggers);

	resize(700, 500);

	connect(enableDebuggingCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(onEnableDebuggingCheckBoxStateChanged(int)));
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

void AMStatusLogView::onEnableDebuggingCheckBoxStateChanged(int state){
	if(state == Qt::Unchecked)
		AMErrorMon::enableDebugNotifications(false);
	else if(state == Qt::Checked)
		AMErrorMon::enableDebugNotifications(true);
}

