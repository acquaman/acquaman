#include "CLSSIS3820ScalerView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QCheckBox>
#include <QLabel>

#include "beamline/CLS/CLSSIS3820Scaler.h"

CLSSIS3820ScalerView::CLSSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent) :
	QWidget(parent)
{
	scaler_ = scaler;
	if(scaler_ && scaler_->isConnected()){
		scanningCE_ = new AMControlEditor(scaler_->startToggleControl());
		continuousCE_ = new AMControlEditor(scaler_->continuousToggleControl());
		dwellTimeCE_ = new AMControlEditor(scaler_->dwellTimeControl());
		scansPerBufferCE_ = new AMControlEditor(scaler_->scansPerBufferControl());
		totalScansCE_ = new AMControlEditor(scaler_->totalScansControl());

		toggleLayout_ = new QFormLayout();
		toggleLayout_->addRow("Scanning", scanningCE_);
		toggleLayout_->addRow("Continuous", continuousCE_);

		dwellLayout_ = new QFormLayout();
		dwellLayout_->addRow("Dwell", dwellTimeCE_);
		dwellLayout_->addRow("Scans Per Buffer", scansPerBufferCE_);
		dwellLayout_->addRow("Total Scans", totalScansCE_);

		channelLayoutLeftTop_ = new QFormLayout();
		channelLayoutRightTop_ = new QFormLayout();
		channelLayoutLeftBottom_ = new QFormLayout();
		channelLayoutRightBottom_ = new QFormLayout();
		QCheckBox *tmpEnable;
		QLabel *tmpReading;
		QHBoxLayout *tmpHL;
		CLSSIS3820ScalerChannel *tmpChannel;
		int channelCount = scaler_->channels().count();
		for(int x = 0; x < channelCount; x++){
			tmpChannel = scaler_->channelAt(x);
			tmpEnable = new QCheckBox();
			tmpEnable->setChecked(tmpChannel->isEnabled());

			tmpReading = new QLabel();
			tmpReading->setText(QString("%1").arg(tmpChannel->reading()));

			tmpHL = new QHBoxLayout();
			tmpHL->addWidget(tmpEnable);
			tmpHL->addWidget(tmpReading);

			if(x < channelCount/4)
				channelLayoutLeftTop_->addRow(QString("%1").arg(x), tmpHL);
			else if(x < channelCount/2 && x >= channelCount/4)
				channelLayoutRightTop_->addRow(QString("%1").arg(x), tmpHL);
			else if(x >= channelCount/2 && x < channelCount/2+channelCount/4)
				channelLayoutLeftBottom_->addRow(QString("%1").arg(x), tmpHL);
			else
				channelLayoutRightBottom_->addRow(QString("%1").arg(x), tmpHL);

			connect(tmpChannel, SIGNAL(enabledChanged(bool)), tmpEnable, SLOT(setChecked(bool)));
			connect(tmpEnable, SIGNAL(toggled(bool)), tmpChannel, SLOT(setEnabled(bool)));
			connect(tmpChannel, SIGNAL(readingChanged(QString)), tmpReading, SLOT(setText(QString)));
		}

		QHBoxLayout *topHL = new QHBoxLayout();
		topHL->addLayout(toggleLayout_);
		topHL->addLayout(dwellLayout_);
		topHL->setContentsMargins(10, 10, 10, 0);

		QHBoxLayout *bottomHL1 = new QHBoxLayout();
		bottomHL1->addLayout(channelLayoutLeftTop_);
		bottomHL1->addLayout(channelLayoutRightTop_);
		bottomHL1->setContentsMargins(10, 0, 10, 5);

		QHBoxLayout *bottomHL2 = new QHBoxLayout();
		bottomHL2->addLayout(channelLayoutLeftBottom_);
		bottomHL2->addLayout(channelLayoutRightBottom_);
		bottomHL2->setContentsMargins(10, 5, 10, 10);

		QVBoxLayout *mainVL = new QVBoxLayout();
		mainVL->addLayout(topHL);
		mainVL->addLayout(bottomHL1);
		mainVL->addLayout(bottomHL2);
		mainVL->setContentsMargins(0,0,0,0);

		setLayout(mainVL);
	}
}
