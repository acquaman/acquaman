#include "SGMSIS3820ScalerView.h"

 SGMSIS3820ScalerView::~SGMSIS3820ScalerView(){}
SGMSIS3820ScalerView::SGMSIS3820ScalerView(CLSSIS3820Scaler *scaler, QWidget *parent) :
	CLSSIS3820ScalerView(scaler, parent)
{
	mainVL_->removeItem(channelLayout_);

	QHBoxLayout *channelHL = new QHBoxLayout();
	channelHL->addLayout(channelLayout_);
	channelHL->addStretch(10);

	mainVL_->addLayout(channelHL);
}
