#include "PIDLoopControlView.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFont>

PIDLoopControlView::PIDLoopControlView(PIDLoopControl *pid, QWidget *parent)
	: QWidget(parent)
{
	pid_ = pid;
	connect(pid_, SIGNAL(stateChanged(bool)), this, SLOT(setDisabled(bool)));
	connect(pid_, SIGNAL(stateChanged(bool)), this, SLOT(onHiddenChanged(bool)));

	QFont font(this->font());
	font.setBold(true);

	QLabel *description = new QLabel(pid_->name());
	description->setFont(font);

	fix_ = new QPushButton("Repair");
	connect(fix_, SIGNAL(clicked()), pid_, SLOT(turnOn()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(description, 0, Qt::AlignLeft);
	layout->addWidget(fix_, 0, Qt::AlignCenter);

	setLayout(layout);

	highlight_ = false;

	timer_ = new QTimer;
	timer_->setInterval(1000);
	timer_->setSingleShot(false);
	connect(timer_, SIGNAL(timeout()), this, SLOT(toggleButtonColor()));
}

PIDLoopControlView::~PIDLoopControlView()
{
	delete timer_;
}

void PIDLoopControlView::toggleButtonColor()
{
	highlight_ = !highlight_;

	if (highlight_){

		QPalette palette(fix_->palette());
		palette.setColor(QPalette::Button, QColor(255, 64, 64));
		fix_->setPalette(palette);
	}
	else
		fix_->setPalette(this->palette());
}

void PIDLoopControlView::onHiddenChanged(bool hidden)
{
	setHidden(hidden);

	if (hidden)
		timer_->stop();
	else
		timer_->start();
}
