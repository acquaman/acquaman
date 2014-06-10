#ifndef AMOLDDETECTORINFOSETVIEW_H
#define AMOLDDETECTORINFOSETVIEW_H

#include <QGroupBox>

#include "dataman/info/AMOldDetectorInfoSet.h"

class QScrollArea;

class AMOldDetectorInfoSetViewInternal;

class AMOldDetectorInfoSetView : public QGroupBox
{
Q_OBJECT
public:
 	virtual ~AMOldDetectorInfoSetView();
	AMOldDetectorInfoSetView(const AMOldDetectorInfoSet &infoSet, QWidget *parent = 0);

protected:
	AMOldDetectorInfoSetViewInternal *internalView_;
	QScrollArea *scrollArea_;
};

class AMOldDetectorInfoSetViewInternal : public QWidget
{
Q_OBJECT
public:
 	virtual ~AMOldDetectorInfoSetViewInternal();
	AMOldDetectorInfoSetViewInternal(const AMOldDetectorInfoSet &infoSet, QWidget *parent = 0);
};

#endif // AMOLDDETECTORINFOSETVIEW_H
