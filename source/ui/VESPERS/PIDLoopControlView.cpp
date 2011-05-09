#include "PIDLoopControlView.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>
#include <QVariant>

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

	QPushButton *fix = new QPushButton("Repair");
	connect(fix, SIGNAL(clicked()), pid_, SLOT(turnOn()));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(description, 0, Qt::AlignLeft);
	layout->addWidget(fix, 0, Qt::AlignCenter);

	setLayout(layout);

	//fix->setAutoFillBackground(true);
	QVariant start(QVariant::Palette);
	start.setValue(QPalette(Qt::blue));
	QVariant end(QVariant::Palette);
	end.setValue(QPalette(Qt::green));

	animation_ = new QPropertyAnimation(fix, "palette");
	animation_->setDuration(1500);
	animation_->setStartValue(start);
	animation_->setEndValue(end);
	animation_->start();

	timer_ = new QTimer;
	timer_->setInterval(2000);
	timer_->setSingleShot(false);
	connect(timer_, SIGNAL(timeout()), animation_, SLOT(start()));
}

PIDLoopControlView::~PIDLoopControlView()
{
	delete timer_;
	delete animation_;
}

QVariant PIDLoopControlView::paletteInterpolator(const QPalette &start, const QPalette &end, qreal progress)
{
	if (progress <0.5)
		return start;
	return end;
	/*QPalette palette(start);

	QColor converted;
	converted.setRedF(start.button().color().redF() + (end.button().color().redF()-start.button().color().redF())*progress);
	converted.setGreenF(start.button().color().greenF() + (end.button().color().greenF()-start.button().color().greenF())*progress);
	converted.setBlueF(start.button().color().blueF() + (end.button().color().blueF()-start.button().color().blueF())*progress);
	converted.setAlphaF(start.button().color().alphaF() + (end.button().color().alphaF()-start.button().color().alphaF())*progress);

	palette.setColor(QPalette::Button, converted);

	QVariant current(QVariant::Palette);
	current.setValue(palette);

	return current;*/
}

void PIDLoopControlView::onHiddenChanged(bool hidden)
{
	setHidden(hidden);

	if (hidden)
		timer_->stop();
	else
		timer_->start();
}
