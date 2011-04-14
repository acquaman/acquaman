#include "VESPERSXRFElementView.h"

#include <QGridLayout>
#include <QStringList>
#include <QToolButton>

VESPERSXRFElementView::VESPERSXRFElementView(QWidget *parent) :
	QWidget(parent)
{
	name_ = new QLabel();
	name_->setFont(QFont("Times New Roman", 16));
	number_ = new QLabel();
	symbol_ = new QLabel();
	symbol_->setFont(QFont("Times New Roman", 45, 10));

	LineView *temp;

	for (int i = 0; i < 6; i++){

		temp = new LineView(qMakePair(QString(), QString()));
		lines_ << temp;
	}


	/*QToolButton *addCustomButton = new QToolButton;
	addCustomButton->setText(tr("Add Custom"));
	connect(addCustomButton, SIGNAL(clicked()), this, SIGNAL(addCustomROI()));

	QToolButton *addROIButton = new QToolButton;
	addROIButton->setText(tr("Add Selected"));
	connect(addROIButton, SIGNAL(clicked()), this, SLOT(emitLines()));*/

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(name_);
	titleLayout->addWidget(symbol_);

	QVBoxLayout *linesLayout = new QVBoxLayout;
	for (int i = 0; i < lines_.size(); i++)
		linesLayout->addWidget(lines_.at(i));
	linesLayout->addStretch();
	linesLayout->setSpacing(0);

	/*QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(addCustomButton);
	buttonLayout->addWidget(addROIButton);*/

	QVBoxLayout *viewLayout = new QVBoxLayout;
	viewLayout->addLayout(titleLayout);
	viewLayout->addLayout(linesLayout);
	//viewLayout->addLayout(buttonLayout);

	setLayout(viewLayout);
}

VESPERSXRFElementView::VESPERSXRFElementView(AMElement *el, QWidget *parent)
	: QWidget(parent)
{
	name_ = new QLabel(el->name());
	name_->setFont(QFont("Times New Roman", 16));
	number_ = new QLabel(QString::number(el->atomicNumber()));
	symbol_ = new QLabel(el->symbol());
	symbol_->setFont(QFont("Times New Roman", 45, 10));

	LineView *temp;

	for (int i = 0; i < 6; i++){

		temp = new LineView(qMakePair(QString(), QString()));
		lines_ << temp;
	}

	fillEmissionLines(el);

	/*QToolButton *addCustomButton = new QToolButton;
	addCustomButton->setText(tr("Add Custom"));
	connect(addCustomButton, SIGNAL(clicked()), this, SIGNAL(addCustomROI()));

	QToolButton *addROIButton = new QToolButton;
	addROIButton->setText(tr("Add Selected"));
	connect(addROIButton, SIGNAL(clicked()), this, SLOT(emitLines()));*/

	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->addWidget(name_);
	titleLayout->addWidget(symbol_);

	QVBoxLayout *linesLayout = new QVBoxLayout;
	for (int i = 0; i < lines_.size(); i++)
		linesLayout->addWidget(lines_.at(i), Qt::AlignRight);
	linesLayout->addStretch();
	linesLayout->setSpacing(0);

	/*QHBoxLayout *buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(addCustomButton);
	buttonLayout->addWidget(addROIButton);*/

	QVBoxLayout *viewLayout = new QVBoxLayout;
	viewLayout->addLayout(titleLayout);
	viewLayout->addLayout(linesLayout);
	//viewLayout->addLayout(buttonLayout);

	setLayout(viewLayout);
}

void VESPERSXRFElementView::setElement(AMElement *el)
{
	element_ = el;
	name_->setText(el->name());
	number_->setText(QString::number(el->atomicNumber()));
	symbol_->setText(el->symbol());
	fillEmissionLines(el);
}


void VESPERSXRFElementView::fillEmissionLines(AMElement *el)
{
	lines_.at(0)->setLine(el->Kalpha());
	lines_.at(1)->setLine(el->Kbeta());
	lines_.at(2)->setLine(el->Lalpha());
	lines_.at(3)->setLine(el->Lbeta());
	lines_.at(4)->setLine(el->Lgamma());
	lines_.at(5)->setLine(el->Malpha());
}

void VESPERSXRFElementView::emitLines()
{
	for (int i = 0; i < lines_.size(); i++)
		if (lines_.at(i)->checked())
			emit addROI(element_, lines_.at(i)->line());
}
