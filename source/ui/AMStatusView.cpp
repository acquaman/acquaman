#include "AMStatusView.h"
#include <QDateTime>
#include "AMSettings.h"
#include <QHBoxLayout>

AMStatusView::AMStatusView(QWidget *parent) : QWidget(parent)
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
	shortText_ = new QLabel(QString("Acquaman: Welcome %1").arg(AMUserSettings::userName));
	hl_->addWidget(shortText_);

	hl_->addStretch(1);
	hl_->addStrut(32);
	hl_->setMargin(0);

	this->setLayout(hl_);

	/// pickup all connections from ErrorMon
	connect(AMErrorMon::mon(), SIGNAL(error(AMErrorReport)), this, SLOT(onAnyError(AMErrorReport)));

	/// enable mouse tracking:
	setMouseTracking(true);

	/// popup box for mouse-over detail view
	detailText_ = new QTextEdit();
	detailText_->setReadOnly(true);

	openAnime = NULL;
	open2Anime = NULL;
	closeAnime = NULL;

	popup_ = new AMHidingDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
	popup_->setGeometry(0, 0, AMSTATUSVIEW_LOG_WIDTH, AMSTATUSVIEW_LOG_HEIGHT);
	//popup_->setGraphicsEffect();
	QHBoxLayout* hl2 = new QHBoxLayout();
	hl2->setMargin(0);
	hl2->addWidget(detailText_);
	popup_->setLayout(hl2);
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
	if(msg.count() > 80)
		shortText_->setText(msg.left(80)+"...");
	else
		shortText_->setText(msg);

	/// Log to detail window:
//	detailText_->append(QString("\n\n%1: %2").arg(QDateTime::currentDateTime().toString("ddd hh:mmap")).arg(msg));
	popup_->append(msg, e.level);

}

