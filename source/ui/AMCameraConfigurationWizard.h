#ifndef AMCAMERACONFIGURATIONWIZARD_H
#define AMCAMERACONFIGURATIONWIZARD_H

#include <QWizard>
#include <QList>
#include "AMGraphicsViewWizard.h"


class QLabel;
class QCheckBox;
class QTimer;
class AMShapeDataSetGraphicsView;
class QGraphicsItem;
class QPointF;
class QVector3D;

/** The AMCameraConfigurationWizard is the wizard for configuring the camera
*   The user is presented with an image and asked to check if it is already configured.
*   If it is not configured, it goes through a series of pages asking the user to select
*   the appropriate point, as well as moving when necessary \note (currently it just waits).
*/
class AMCameraConfigurationWizard : public AMGraphicsViewWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check, Page_Final, Page_Select_One, Page_Select_Two, Page_Select_Three, Page_Select_Four,
            Page_Select_Five, Page_Select_Six, Page_Wait_One, Page_Wait_Two, Page_Wait_Three, Page_Wait_Four, Page_Wait_Five, Page_Wait_Six};

    AMCameraConfigurationWizard(QWidget* parent = 0);
    ~AMCameraConfigurationWizard();

    /// reimplementation of nextId.
    /// it is necessary to control the page flow from the wizard
    /// rather than the pages becuase there is more than one instance of several pages
    int nextId() const;

    virtual QString message(int messageType);

    int relativeId();

public slots:
    /// reimplementation of the back slot, used to move the motor back to the appropriate place
    /// on going back to a previous page
    void back();

    /// sets the appropriate point in the list and goes on to the next page
    virtual void addPoint(QPointF position);
protected:
    double coordinateX(int id);
    double coordinateY(int id);
    double coordinateZ(int id);
};

/// Intro page is just the introduction/howto for this wizard.

class IntroPage : public AMWizardPage
{
    Q_OBJECT
public:
    void initializePage();

    void timerEvent(QTimerEvent *event);

};


class CheckPage : public AMViewPage
{
    Q_OBJECT
public:
    CheckPage(QWidget* parent = 0);
public slots:

    void initializePage();

protected slots:
    void configuredSet(bool set);
protected:
    QCheckBox* isConfigured_;
};


class SelectPage : public AMViewPage
{
    Q_OBJECT
public:
    void initializePage();

public slots:

    void addPoint(QPointF position);
};

class WaitPage : public AMWaitPage
{
    Q_OBJECT
public:
    void initializePage();
};



#endif // AMCAMERACONFIGURATIONWIZARD_H
