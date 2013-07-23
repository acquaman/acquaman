#ifndef AMBEAMCONFIGURATIONWIZARD_H
#define AMBEAMCONFIGURATIONWIZARD_H

#include "AMGraphicsViewWizard.h"


class QLabel;
class QCheckBox;

class AMBeamConfigurationWizard : public AMGraphicsViewWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check_One, Page_Check_Two, Page_Check_Three, Page_Set_One,
          Page_Set_Two, Page_Set_Three, Page_Wait_One, Page_Wait_Two, Page_Wait_Three, Page_Final,
          Wait_One_Again, Wait_Two_Again, Wait_Three_Again};

    enum WizardMessage {Title_Wizard,
                        Title_Intro};
    AMBeamConfigurationWizard(QWidget* parent = 0);
    ~AMBeamConfigurationWizard();

    int nextId() const;


    QString message(WizardMessage);

    void addPoint(QPointF position);

    QList<QVector3D*>* coordinateList();

public slots:
    void back();

    void next();

    void showHelp();


signals:
    void done();
protected:
    bool setting_;

    QList<QVector3D*>* coordinateList_;

    QList<QPointF*>* pointList_;

};

class AMBeamIntroPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMBeamIntroPage(QWidget* parent = 0);
};

class AMBeamCheckPage : public AMViewPage
{
    Q_OBJECT
public:
    AMBeamCheckPage(QWidget* parent = 0);
    void initializePage();

public slots:

    void configuredChanged(bool);

protected:
    QCheckBox* beamConfigured_;
};

class AMBeamWaitPage : public AMWaitPage
{
    Q_OBJECT
public:
    AMBeamWaitPage(QWidget* parent = 0);
    void initializePage();
};

class AMBeamSelectPage : public AMViewPage
{
    Q_OBJECT
public:
    AMBeamSelectPage(QWidget* parent = 0);

    void initializePage();
};

class AMBeamFinalPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMBeamFinalPage(QWidget* parent = 0);
    void initializePage();
};

#endif // AMBEAMCONFIGURATIONWIZARD_H
