#ifndef AMROTATIONWIZARD_H
#define AMROTATIONWIZARD_H

#include "AMGraphicsViewWizard.h"

/// The AMRotationWizard is used to calculate the centre of rotation
class AMRotationWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro,
		Page_Check,
		Page_Final,
		Page_Option,
		Page_Free
	};
	AMRotationWizard(QWidget* parent = 0);
	virtual ~AMRotationWizard();
	int nextId() const;

	void waitPage();

	virtual QString message(int type);

public slots:
	void back();
	virtual void addPoint(QPointF position);


};

class AMRotationWaitPage: public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();
};

class AMRotationSetPage: public AMViewPage
{
	Q_OBJECT
public:
	virtual void initializePage();
	virtual bool validatePage();
	virtual void cleanupPage();

public slots:
	void addPoint(QPointF position);

protected:
	void disconnectSignals();
};

class AMRotationCheckPage : public AMCheckPage
{
	Q_OBJECT
protected slots:
	virtual void checkBoxChanged(bool state);
};

#endif // AMROTATIONWIZARD_H
