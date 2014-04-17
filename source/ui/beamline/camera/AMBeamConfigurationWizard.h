#ifndef AMBEAMCONFIGURATIONWIZARD_H
#define AMBEAMCONFIGURATIONWIZARD_H

#include "AMGraphicsViewWizard.h"


class QLabel;
class QCheckBox;


/// Wizard to configure the beam.  After a failed check takes three sets of
/// two dimensional points.  Access these points to reposition the beam.
class AMBeamConfigurationWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum {Page_Intro, Page_Check_One, Page_Check_Two, Page_Check_Three, Page_Set_One,
		Page_Set_Two, Page_Set_Three, Page_Wait_One, Page_Wait_Two, Page_Wait_Three, Page_Final,
		Wait_One_Again, Wait_Two_Again, Wait_Three_Again, Skip_Check, Page_Option, Page_Free};


	AMBeamConfigurationWizard(QWidget* parent = 0);
	virtual ~AMBeamConfigurationWizard();

	virtual int nextId() const;

	virtual QString message(int);

	void showBeamShape();

	bool setting();

	bool reviewBeamShape();

public slots:
	void back();

	virtual void addPoint(QPointF position);

	void endPoint();

signals:
	void showShape(int);

	void showBeamMarker(int);

protected:
	bool allChecked() const;
	bool lastCheckPage() const;
	bool lastSetPage() const;
	bool isCheckPage() const;

protected:
	bool setting_;

	bool reviewBeamShape_;

	bool topLeft_;
};

class AMBeamIntroPage : public AMWizardPage
{
	Q_OBJECT
public:
	void initializePage();

	void timerEvent(QTimerEvent *event);

};

class AMBeamCheckPage : public AMCheckPage
{
	Q_OBJECT
public:
	void initializePage();
protected:
};

class AMBeamWaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();
};

class AMBeamSelectPage : public AMViewPage
{
	Q_OBJECT
public:

	void initializePage();

};



#endif // AMBEAMCONFIGURATIONWIZARD_H
