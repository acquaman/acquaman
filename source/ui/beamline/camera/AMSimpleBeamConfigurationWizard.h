#ifndef AMSIMPLEBEAMCONFIGURATIONWIZARD_H
#define AMSIMPLEBEAMCONFIGURATIONWIZARD_H

#include "AMGraphicsViewWizard.h"

class AMSimpleBeamConfigurationWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro = 0, // description
		Page_Final = 1, // confirmation of completion
		Page_Option = 2, // set coordinates
		Page_Free = 3 // for wizard use

	};
	AMSimpleBeamConfigurationWizard(QWidget *parent = 0);
	virtual ~AMSimpleBeamConfigurationWizard();
	virtual int nextId() const;
	virtual void waitPage();
	virtual QString message(int type);

public slots:
	void back();
	void initBeamShape();
	void mousePressedHandler(QPointF);
	void moveBeamShapeHandler(QPointF);

signals:
	void createBeamShape(int);
	void mousePressed(QPointF, int);
	void moveBeamShape(QPointF, int);

};

class AMSimpleBeamConfigurationSetPage : public AMViewPage
{
	Q_OBJECT
public:
	void initializePage();
signals:
	void initBeamShape();
	void signalMousePressed(QPointF);
	void moveBeamShape(QPointF);

protected slots:
	void selectShape(QPointF);
	void releaseShape();
	void moveShape(QPointF);
	void disconnectMouseSignal();



};

class AMSimpleBeamConfigurationWaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();
};

#endif // AMSIMPLEBEAMCONFIGURATIONWIZARD_H
