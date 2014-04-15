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
};

class AMSimpleBeamConfigurationSetPage : public AMViewPage
{
	Q_OBJECT

};

class AMSimpleBeamConfigurationWaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();
};

#endif // AMSIMPLEBEAMCONFIGURATIONWIZARD_H
