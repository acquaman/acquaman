#ifndef AMCAMERACONFIGURATIONWIZARD_H
#define AMCAMERACONFIGURATIONWIZARD_H

#include <QWizard>
#include <QList>
#include "AMGraphicsViewWizard.h"


class QLabel;
class QCheckBox;
class QTimer;
class AMSampleCameraGraphicsView;
class QGraphicsItem;
class QPointF;
class QVector3D;

/** The AMCameraConfigurationWizard is the wizard for configuring the camera
*   The user is presented with an image and asked to check if it is already configured.
*   If it is not configured, it goes through a series of pages asking the user to select
*   the appropriate point, as well as moving when necessary.
*/
class AMCameraConfigurationWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro,
		Page_Option,
		Page_Check,
		Page_Final,
		Page_Free
	};

	AMCameraConfigurationWizard(QWidget* parent = 0);
	virtual ~AMCameraConfigurationWizard();

	/// reimplementation of nextId.
	/// it is necessary to control the page flow from the wizard
	/// rather than the pages because there is more than one instance of several pages
	virtual int nextId() const;

	virtual QString message(int messageType);


public slots:
	/// reimplementation of the back slot, used to move the motor back to the appropriate place
	/// on going back to a previous page
	void back();

	/// sets the appropriate point in the list and goes on to the next page
	virtual void addPoint(QPointF position);

protected:
	/// get coordinate (x/y/z) for specified id
	/// -this is the relative id
	double coordinateX(int id);
	double coordinateY(int id);
	double coordinateZ(int id);
	void clearPoints();
};

/// Intro page is just the introduction/howto for this wizard.

class IntroPage : public AMWizardPage
{
	Q_OBJECT
public:
	void initializePage();

	void timerEvent(QTimerEvent *event);

};

/// Check page is used to check if the camera is actually aligned,
/// so that the wizard may be skipped early on.
class CheckPage : public AMCheckPage
{
	Q_OBJECT
public slots:

	void initializePage();

protected slots:
};

/// SelectPage is where the calibration point is selected.
class SelectPage : public AMViewPage
{
	Q_OBJECT
public:
	void initializePage();

public slots:

	void addPoint(QPointF position);
};

/// Wait page causes the user to wait while the motor moves
/// to the next position
class WaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();

};



#endif // AMCAMERACONFIGURATIONWIZARD_H
