#include "VESPERSXRFElementView.h"

#include <QGridLayout>
#include <QStringList>
#include <QToolButton>

VESPERSXRFElementView::VESPERSXRFElementView(double minEnergy, double maxEnergy, QWidget *parent)
	: QWidget(parent)
{
	minimumEnergy_ = minEnergy;
	maximumEnergy_ = maxEnergy;

	element_ = 0;
	name_ = new QLabel();
	name_->setFont(QFont("Times New Roman", 16));
	number_ = new QLabel();
	symbol_ = new QLabel();
	symbol_->setFont(QFont("Times New Roman", 45, 10));

	LineView *temp;

	for (int i = 0; i < 6; i++){

		temp = new LineView(qMakePair(QString(), QString()));
		connect(temp, SIGNAL(lineChecked(LineView *)), this, SLOT(emitLines(LineView *)));
		lines_ << temp;
	}

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(name_);
	titleLayout->addWidget(symbol_);

	QVBoxLayout *linesLayout = new QVBoxLayout;
	linesLayout->setSpacing(0);
	for (int i = 0; i < lines_.size(); i++)
		linesLayout->addWidget(lines_.at(i));

	QVBoxLayout *viewLayout = new QVBoxLayout;
	viewLayout->addLayout(titleLayout);
	viewLayout->addLayout(linesLayout);
	viewLayout->addStretch();

	setMaximumSize(300, 300);

	setLayout(viewLayout);
}

VESPERSXRFElementView::VESPERSXRFElementView(AMElement *el, double minEnergy, double maxEnergy, QWidget *parent)
	: QWidget(parent)
{
	minimumEnergy_ = minEnergy;
	maximumEnergy_ = maxEnergy;

	element_ = el;
	name_ = new QLabel(el->name());
	name_->setFont(QFont("Times New Roman", 16));
	number_ = new QLabel(QString::number(el->atomicNumber()));
	symbol_ = new QLabel(el->symbol());
	symbol_->setFont(QFont("Times New Roman", 45, 10));

	LineView *temp;

	for (int i = 0; i < 6; i++){

		temp = new LineView(qMakePair(QString(), QString()));
		connect(temp, SIGNAL(lineChecked(LineView *)), this, SLOT(emitLines(LineView *)));
		lines_ << temp;
	}

	fillEmissionLines(el);

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(name_);
	titleLayout->addWidget(symbol_);

	QVBoxLayout *linesLayout = new QVBoxLayout;
	linesLayout->setSpacing(0);
	for (int i = 0; i < lines_.size(); i++)
		linesLayout->addWidget(lines_.at(i), Qt::AlignRight);

	QVBoxLayout *viewLayout = new QVBoxLayout;
	viewLayout->addLayout(titleLayout);
	viewLayout->addLayout(linesLayout);
	viewLayout->addStretch();

	setMaximumSize(300, 300);

	setLayout(viewLayout);
}

void VESPERSXRFElementView::setElement(AMElement *el, QList<QPair<int, QString> > checked)
{
	element_ = el;
	name_->setText(el->name());
	number_->setText(QString::number(el->atomicNumber()));
	symbol_->setText(el->symbol());
	checked_ = checked;
	fillEmissionLines(el);
}


void VESPERSXRFElementView::fillEmissionLines(AMElement *el)
{
	QList<QPair<QString, QString> > list(el->emissionLines());

	int lineIndex = 0;

	for (int i = 0; i < list.size(); i++){

		if (list.at(i).first.contains("1") && list.at(i).second.toDouble() >= minimumEnergy_ && list.at(i).second.toDouble() < maximumEnergy_){

			if (checked_.contains(qMakePair(el->atomicNumber(), list.at(i).first)))
				lines_.at(lineIndex++)->setLine(list.at(i), true);
			else
				lines_.at(lineIndex++)->setLine(list.at(i), false);
		}
	}

	for ( ; lineIndex < lines_.size(); lineIndex++)
		lines_.at(lineIndex)->setLine(qMakePair(QString(), QString()));
}

void VESPERSXRFElementView::emitLines(LineView *line)
{
	if (line->checked())
		emit addROI(element_, line->line());
	else
		emit removeROI(element_, line->line());
}
