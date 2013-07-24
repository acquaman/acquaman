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
          Wait_One_Again, Wait_Two_Again, Wait_Three_Again, Skip_Check};


    AMBeamConfigurationWizard(QWidget* parent = 0);
    ~AMBeamConfigurationWizard();

    int nextId() const;


    QString message(int);

    QList<QPointF*>* pointList();

    QList<QVector3D*>* coordinateList();

    int relativeId();




public slots:
    void back();

    void next();

    void showHelp();

    void addPoint(QPointF position);

    void endPoint(QPointF position);


signals:
    void done();
    void showShape(int);

protected:
    bool setting_;

    QList<QVector3D*>* coordinateList_;

    QList<QPointF*>* pointList_;

    bool topLeft_;

};

class AMBeamIntroPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMBeamIntroPage(QWidget* parent = 0);
    void initializePage();
//    QString message(int type);
};

class AMBeamCheckPage : public AMViewPage
{
    Q_OBJECT
public:
//    enum {Title, Text, CheckBox};
    AMBeamCheckPage(QWidget* parent = 0);
    void initializePage();
//    QString message(int type);

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
//    QString message(int type);
};

class AMBeamSelectPage : public AMViewPage
{
    Q_OBJECT
public:
    AMBeamSelectPage(QWidget* parent = 0);

    void initializePage();

//    QString message(int type);
};

class AMBeamFinalPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMBeamFinalPage(QWidget* parent = 0);
    void initializePage();
//    QString message(int type);
};

#endif // AMBEAMCONFIGURATIONWIZARD_H
