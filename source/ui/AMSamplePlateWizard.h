#ifndef AMSAMPLEPLATEWIZARD_H
#define AMSAMPLEPLATEWIZARD_H

#include "AMGraphicsViewWizard.h"

class QSlider;

class AMSamplePlateWizard : public AMGraphicsViewWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check, Page_Wait, Page_Set, Page_Final};
    AMSamplePlateWizard(QWidget* parent = 0);
    ~AMSamplePlateWizard();

    int nextId() const;

    QString message(int type);

public slots:
    void back();
    void sliderChanged();

signals:
    void movePlate(int);


};

class AMSampleWaitPage : public AMWaitPage
{
    Q_OBJECT
public:
    void initializePage();
};

class AMSampleSetPage : public  AMViewPage
{
    Q_OBJECT
public:
    AMSampleSetPage(QWidget* parent = 0);
    void initializePage();

signals:
    void slider();

protected slots:
    void sliderChanged();

protected:
    QSlider* adjustmentSlider_;
};

#endif // AMSAMPLEPLATEWIZARD_H
