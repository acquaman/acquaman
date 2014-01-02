#ifndef AMCONTROLINFOLISTVIEW_H
#define AMCONTROLINFOLISTVIEW_H

#include "dataman/info/AMControlInfoList.h"

#include <QGroupBox>

class AMControlInfoListView : public QGroupBox
{
Q_OBJECT
public:
 	virtual ~AMControlInfoListView();
	AMControlInfoListView(const AMControlInfoList &infoList, QMap<int, QString> enumOverrides, QWidget *parent = 0);
};

#endif // AMCONTROLINFOLISTVIEW_H
