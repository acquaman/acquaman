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

    int relativeId();

    void waitPage();

public slots:
    void back();

    void next();

    void addPoint(QPointF position);

    void endPoint();

signals:
    void showShape(int);

protected:
    bool setting_;

    bool topLeft_;
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
