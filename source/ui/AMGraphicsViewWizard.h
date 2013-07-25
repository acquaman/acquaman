#ifndef AMGRAPHICSVIEWWIZARD_H
#define AMGRAPHICSVIEWWIZARD_H

#include <QWizard>

class AMShapeDataSetGraphicsView;
class QPointF;
class QTimer;
class QFrame;
class QLabel;
class QVector3D;
class QGraphicsPolygonItem;


/// Wizard class
/// contains a Graphics view with a video.
/// Also contains the common functionality
/// of the Camera, Beam, and Sample plate wizards
/// such as move sample plate to point,
/// mapping point to the video and a message function,
///  which allows all text to be easily defined in one place.

/// All pages in a subclass of this wizard should be descendents of
/// AMWizard page.
/// to show the view they should subclass AMViewPage
/// and to be move-to type pages they should
/// subclass AMWaitPage

class AMGraphicsViewWizard : public QWizard
{
    Q_OBJECT
public:
    /// These are the different message elements
    enum {Wizard_Title, Help_Title, Title, Text, Help, Other, Default};
    AMGraphicsViewWizard(QWidget* parent = 0);

    AMShapeDataSetGraphicsView* view();

    QPointF scale();

    void setScale(QPointF scale);
    void setScale(double scaleFactor);

    /// emits moveTo signal with currentId as the argument
    virtual void waitPage();

    virtual QList<QPointF*>* pointList();

    virtual QList<QVector3D*>* coordinateList();

    /// used to set all the text for the wizard in one easy to find
    ///  location.  Must be reimplemented to get desired text.
    /// Simply do a switch for each page and check for each
    /// element of the message type enum.
    virtual QString message(int type);

    /// maps a point to the video item, so that point 0,0 is
    /// the top left corner of the image and 1,1 is the bottom left
    /// corner.  Note that this does NOT map directly to
    /// the view/scene in AMShapeDataSetView. It needs to be
    /// remapped from the the video bounding rect to the
    /// video item.
    QPointF mapPointToVideo(QPointF);


public slots:
    void setView(AMShapeDataSetGraphicsView* view);

    /// shows the help message.
    /// if message is not reimplemented it will only
    /// say "Default message"
    /// either reimplement message (and define text
    /// for each page for the enum Help) or reimplement
    /// this directly.
    virtual void showHelp();

    virtual void addPoint(QPointF position) = 0;

    void updateScene(AMShapeDataSetGraphicsView* view);

    void updateShape(QGraphicsPolygonItem* item);

    void fixText();

signals:
    void done();
    void moveTo(int);
protected:




protected:
    AMShapeDataSetGraphicsView* view_;
    QPointF* scale_;
    int numberOfPoints_;

    QList<QVector3D*>* coordinateList_;

    QList<QPointF*>* pointList_;

};


class AMWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    enum {Title, Text, Help, Other, Default};
    AMWizardPage(QWidget* parent = 0);
    AMGraphicsViewWizard* viewWizard() const;

    virtual void initializePage();

    /// used to set messages using the message function
    /// in AMGraphicsViewWizard
    /// simply invokes message with the corresponding type
    /// (if type == Title calls AMGraphicsViewWizard::message(AMGraphicsViewWizard::Title)
    virtual QString message(int type);
public slots:
    void setLabelText(QString text);
protected:
    QLabel* topLabel_;

};


class AMWaitPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMWaitPage(QWidget* parent = 0);
    bool isComplete() const;
    void stopTimer();
    void startTimer(int msec);
protected slots:
    void nextPage();


private:
    QTimer* waitTimer_;


};


class AMViewPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMViewPage(QWidget* parent = 0);

    AMShapeDataSetGraphicsView* view();

public slots:
    void setView(AMShapeDataSetGraphicsView* view);

    void initializePage();
    void cleanupPage();


protected slots:
    void addView();

private:
    AMShapeDataSetGraphicsView* view_;

    QLayout* layout_;
    QFrame* viewFrame_;

};


#endif // AMGRAPHICSVIEWWIZARD_H
