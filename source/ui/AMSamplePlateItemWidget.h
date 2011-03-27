#ifndef AMSAMPLEPLATEITEMWIDGET_H
#define AMSAMPLEPLATEITEMWIDGET_H

#include <QWidget>
#include "dataman/AMControlInfoList.h"

namespace Ui {
	class AMSamplePlateItemWidget;
}

class AMSamplePlateItemWidget : public QWidget
{
	Q_OBJECT

public:
	explicit AMSamplePlateItemWidget(QWidget *parent = 0);
	~AMSamplePlateItemWidget();

	void setSampleNameText(const QString& string);
	void setDateTimeText(const QString& createdText);
	void setElementText(const QString& elementText);
	void setPositionText(const AMControlInfoList& controlInfo);

private:
	Ui::AMSamplePlateItemWidget *ui;
};

#endif // AMSAMPLEPLATEITEMWIDGET_H
