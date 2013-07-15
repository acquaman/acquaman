#ifndef AMCAMERACONFIGURATIONWIZARD_H
#define AMCAMERACONFIGURATIONWIZARD_H

#include <QWizard>

class QLabel;
class QCheckBox;
class QTimer;
class QGraphicsView;

class AMCameraConfigurationWizard : public QWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check, Page_Final, Page_Select_One, Page_Select_Two, Page_Select_Three, Page_Select_Four,
            Page_Select_Five, Page_Select_Six, Page_Wait_One, Page_Wait_Two, Page_Wait_Three, Page_Wait_Four, Page_Wait_Five};
    AMCameraConfigurationWizard(QWidget* parent = 0);
    int nextId() const;
private slots:
    void showHelp();
};


class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage(QWidget* parent = 0);

private:
    ///contents
    QLabel* topLabel_;
};


class CheckPage : public QWizardPage
{
    Q_OBJECT
public:
    CheckPage(QWidget* parent = 0);

private slots:
    void configuredSet(bool set);

private:
    QLabel* topLabel_;
    QCheckBox* isConfigured_;
    QGraphicsView* checkView_;
};

class FinalPage : public QWizardPage
{
    Q_OBJECT
public:
    FinalPage(QWidget* parent = 0);

private:
    QLabel* topLabel_;
};

class SelectPage : public QWizardPage
{
    Q_OBJECT
public:
    SelectPage(QWidget* parent = 0);

    void initializePage();

private:
    QLabel* topLabel_;
};

class WaitPage : public QWizardPage
{
    Q_OBJECT
public:
    WaitPage(QWidget* parent = 0);

    void initializePage();

    bool isComplete() const;

private slots:
    void nextPage();

private:
    QLabel* topLabel_;
    QTimer* waitTimer_;
};

#endif // AMCAMERACONFIGURATIONWIZARD_H
