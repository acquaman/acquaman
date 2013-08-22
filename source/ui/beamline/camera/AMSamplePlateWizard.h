#ifndef AMSAMPLEPLATEWIZARD_H
#define AMSAMPLEPLATEWIZARD_H

#include "AMGraphicsViewWizard.h"

class QSlider;


/// Wizard for repositioning the Sample plate.
/// used to adjust it based on its exact position.
class AMSamplePlateWizard : public AMGraphicsViewWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check, Page_Wait, Page_Set, Page_Final, Page_Option};
    AMSamplePlateWizard(QWidget* parent = 0);
    ~AMSamplePlateWizard();

    int nextId() const;

    void waitPage();

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

class AMSampleCheckPage : public AMCheckPage
{
    Q_OBJECT
protected slots:
    virtual void checkBoxChanged(bool state);
};

#endif // AMSAMPLEPLATEWIZARD_H