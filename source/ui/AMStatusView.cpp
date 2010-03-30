#include "AMStatusView.h"

AMStatusView::AMStatusView(QWidget *parent) :
	QWidget(parent)
{
	/// widget layout
	hl_ = new QHBoxLayout();

	/// Icons representing the nature of the last notification
	iconInfo_ = new QLabel("info!");
	hl_->addWidget(iconInfo_);
	iconAlert_ = new QLabel("alert!");
	hl_->addWidget(iconAlert_);
	iconSerious_ = new QLabel("serious!");
	hl_->addWidget(iconSerious_);
	iconDebug_ = new QLabel("debug!");
	hl_->addWidget(iconDebug_);

	/// last notification text
	shortText_ = new QLabel("Latest status: Fuck Off, Dave!");
	hl_->addWidget(shortText_);

	this->setLayout(hl_);



	/// popup box for mouse-over detail view
	popup_ = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
	/// contents of detail view
	longText_ = new QTextEdit(popup_);

}
