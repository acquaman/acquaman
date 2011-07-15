#include "AMLinePropertyEditor.h"

#include "ui_AMLinePropertyEditor.h"

#include <QPen>
#include <QBrush>

AMLinePropertyEditor::AMLinePropertyEditor(QWidget *parent) :
	QFrame(parent), ui_(new Ui::AMLinePropertyEditor)
{
	ui_->setupUi(this);

	ui_->fillColorButton->setEnabled(false);
	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), ui_->fillColorButton, SLOT(setEnabled(bool)));

	connect(ui_->colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->widthSlider, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->lineStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenSettingsChanged()));

	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(ui_->fillColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));
}

AMLinePropertyEditor::AMLinePropertyEditor(const QPen& initialPen, bool areaFilled, const QBrush& areaFillColor, QWidget* parent) :
	QFrame(parent), ui_(new Ui::AMLinePropertyEditor)
{
	ui_->setupUi(this);

	ui_->colorButton->setColor(initialPen.color());
	ui_->widthSlider->setValue(initialPen.width());
	ui_->lineStyleBox->setCurrentIndex(qBound(0, initialPen.style()-1, 4));

	ui_->fillCheckBox->setChecked(areaFilled);
	ui_->fillColorButton->setColor(areaFillColor.color());

	ui_->fillColorButton->setEnabled(areaFilled);
	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), ui_->fillColorButton, SLOT(setEnabled(bool)));

	connect(ui_->colorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->widthSlider, SIGNAL(valueChanged(int)), this, SLOT(onPenSettingsChanged()));
	connect(ui_->lineStyleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPenSettingsChanged()));

	connect(ui_->fillCheckBox, SIGNAL(clicked(bool)), this, SIGNAL(areaFilledChanged(bool)));
	connect(ui_->fillColorButton, SIGNAL(colorChanged(QColor)), this, SLOT(onAreaFillColorChanged(QColor)));
}


AMLinePropertyEditor::~AMLinePropertyEditor() {
	delete ui_;
}

#include <QDebug>

void AMLinePropertyEditor::onPenSettingsChanged()
{
	emit linePenChanged(QPen(ui_->colorButton->color(),
							 ui_->widthSlider->value(),
							 (Qt::PenStyle)(ui_->lineStyleBox->currentIndex()+1)));
}

void AMLinePropertyEditor::onAreaFillColorChanged(const QColor &color)
{
	emit areaFillBrushChanged(QBrush(color));
}
