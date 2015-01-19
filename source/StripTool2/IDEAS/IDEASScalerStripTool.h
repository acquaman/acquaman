#ifndef IDEASSCALERSTRIPTOOL_H
#define IDEASSCALERSTRIPTOOL_H

#include "source/StripTool2/STWidget.h"


class IDEASScalerStripTool : public STWidget
{
    Q_OBJECT
public:
	explicit IDEASScalerStripTool(QWidget *parent = 0);

signals:

public slots:

protected slots:
	virtual void onCustomContextMenuRequested(QPoint position);

protected:
	void addSample();
	void addReference();
	void addPicoammeter();
	void removeAll();



};

#endif // IDEASSCALERSTRIPTOOL_H
