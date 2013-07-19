#ifndef AMCAMERACONFIGURATIONWIZARD_H
#define AMCAMERACONFIGURATIONWIZARD_H

#include <QWizard>
#include <QList>


class QLabel;
class QCheckBox;
class QTimer;
class AMShapeDataSetGraphicsView;
class QGraphicsItem;
class QPointF;
class QVector3D;

class AMCameraConfigurationWizard : public QWizard
{
    Q_OBJECT
public:
    enum {Page_Intro, Page_Check, Page_Final, Page_Select_One, Page_Select_Two, Page_Select_Three, Page_Select_Four,
            Page_Select_Five, Page_Select_Six, Page_Wait_One, Page_Wait_Two, Page_Wait_Three, Page_Wait_Four, Page_Wait_Five, Page_Wait_Six};
    AMCameraConfigurationWizard(QWidget* parent = 0);
    ~AMCameraConfigurationWizard();
    int nextId() const;
    AMShapeDataSetGraphicsView* view();
    QGraphicsItem* videoItem();

    QPointF scale();

    void setScale(QPointF scale);

    void setScale(double scaleFactor);

    void addPoint(QPointF position);

    QList<QPointF*>* pointList();

    QList<QVector3D*>* coordinateList();


public slots:
    void back();
    /// sets the view
    /// duplicates the scene in the passed view, does not keep the original one
    void setView(AMShapeDataSetGraphicsView* view);

    void allDone();


signals:
    void done();

private slots:
    void showHelp();
private:
    bool backwards_;

    QPointF scale_;

    QList<QPointF*>* pointList_;
    QList<QVector3D*>* coordinateList_;

    AMShapeDataSetGraphicsView* view_;

    int numberOfPoints_;
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
public slots:
    void setCheckView(AMShapeDataSetGraphicsView* view);

    void initializePage();
    void cleanupPage();

private slots:
    void configuredSet(bool set);

private:
    QLabel* topLabel_;
    QCheckBox* isConfigured_;
    AMShapeDataSetGraphicsView* checkView_;
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

    void setSelectionView(AMShapeDataSetGraphicsView* view);
    void initializePage();

    void cleanupPage();

public slots:
    void setView(AMShapeDataSetGraphicsView* view);

    void addPoint(QPointF position);

private:
    QLabel* topLabel_;
    AMShapeDataSetGraphicsView* view_;
};

class WaitPage : public QWizardPage
{
    Q_OBJECT
public:
    WaitPage(QWidget* parent = 0);

    void initializePage();

    bool isComplete() const;

    void stopTimer();

private slots:
    void nextPage();

private:
    QLabel* topLabel_;
    QTimer* waitTimer_;
};

#endif // AMCAMERACONFIGURATIONWIZARD_H
