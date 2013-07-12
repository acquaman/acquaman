#ifndef AMCAMERACONFIGURATIONWIZARD_H
#define AMCAMERACONFIGURATIONWIZARD_H

#include <QWizard>

class QLabel;
class QCheckBox;

class AMCameraConfigurationWizard : public QWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check};
    AMCameraConfigurationWizard(QWidget* parent = 0);
private slots:
    void showHelp();
};


class IntroPage : public QWizardPage
{
    Q_OBJECT
public:
    IntroPage(QWidget* parent = 0);

    int nextId() const;

private:
    ///contents
    QLabel* topLabel_;
};


class CheckPage : public QWizardPage
{
    Q_OBJECT
public:
    CheckPage(QWidget* parent = 0);

    int nextId() const;

private:
    QLabel* topLabel_;
    QCheckBox* isConfigured_;
};

#endif // AMCAMERACONFIGURATIONWIZARD_H
