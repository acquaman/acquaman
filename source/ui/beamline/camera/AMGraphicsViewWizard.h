#ifndef AMGRAPHICSVIEWWIZARD_H
#define AMGRAPHICSVIEWWIZARD_H

#include <QWizard>
#include <QVector3D>
#include <QMediaPlayer>

class AMSampleCameraGraphicsView;
class QPointF;
class QTimer;
class QFrame;
class QLabel;
//class QVector3D;
class QGraphicsPolygonItem;
class QCheckBox;
class QGraphicsTextItem;
class QLineEdit;
//class QMediaPlayer;


/// Wizard class
/// contains a Graphics view with a video.
/// Also contains the common functionality
/// of the Camera, Beam, and Sample plate wizards
/// such as move sample plate to point,
/// mapping point to the video and as well as
/// common wizard functionality that makes it
/// faster to create a wizard with less extra classes and methods

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
    virtual ~AMGraphicsViewWizard();

    /// returns the view held by this wizard
    AMSampleCameraGraphicsView* view() const;

    /// returns the scaling of the view
    QPointF scale() const;

    /// setting the scale of the view allows it to be zoomed in or out
    void setScale(QPointF scale);
    void setScale(double scaleFactor);

    /// emits moveTo signal with currentId as the argument
    virtual void waitPage();

    /// returns the list of points
    virtual QList<QPointF*>* pointList() const;

    /// returns the list of coordinates
    virtual QList<QVector3D*>* coordinateList() const;

	virtual QList<double>* rotations() const;

    /// the number of points for this wizard.
    /// used to generate the options page
    int numberOfPoints() const;



    /// used to set all the text for the wizard in one easy to find
    ///  location.  Must be reimplemented to get desired text.
    /// Simply do a switch for each page and check for each
    /// element of the message type enum.
    virtual QString message(int type);

    /// maps a point to the video item, so that point 0,0 is
    /// the top left corner of the image and 1,1 is the bottom left
    /// corner.  Note that this does NOT map directly to
    /// the view/scene in AMSampleCameraView. It needs to be
    /// remapped from the the video bounding rect to the
    /// video item.
    QPointF mapPointToVideo(QPointF) const;

    /// used to access the state of the checkbox in an AMCheckPage
    virtual bool checked(int page) const;

    /// adds the option page, making it accessible from
    /// the page with page number 'id'
    void addOptionPage(int id);

    /// returns true if the page is set to be shown
    bool showOptionPage() const;

    /// sets the page to access the options page from
    void setOptionPage(int id);

    // These functions are to protect from accidental motor movement during testing

    /// in order to prevent motorMovementEnabled_ from being changed outside of where it should be
    /// it is not operated in the same way as a regular member.  It is always false except immediately
    /// after it requests its state.  After the state is read, it immediately becomes false again
    bool motorMovementEnabled();

    /// call this to request the motorMovementState
    /// this must be called before motorMovementEnabled()
    /// in order to get the true value
    void checkMotorMovementState();

	bool rotationEnabled() const;

public slots:
    /// sets the view
    void setView(AMSampleCameraGraphicsView* view);

    /// sets the point in pointList at index to point
    virtual void setPoint(QPointF point, int index);

    /// sets the coordinate in coordinateList at index to coordinate
    virtual void setCoordinate(QVector3D coordinate, int index);

	virtual void setRotation(double rotation, int index);



    /// shows the help message.
    /// if message is not reimplemented it will only
    /// display "Default message"
    /// either reimplement message (and define text
    /// for each page for the enum Help) or reimplement
    /// this directly.
    virtual void showHelp();

    /// appends a point to pointList
    virtual void addPoint(QPointF position);

    /// updates the scene
    void updateScene(AMSampleCameraGraphicsView* view);

    void updateShape(QGraphicsPolygonItem* item);

    /// repositions the "fix text".
    void fixText();

    /// connects the move succeeded signal to a slot.
    void testMoveSlot();

signals:
    void done();
    void moveTo(QVector3D);
	void moveTo(QVector3D,double);
    void requestMotorMovementEnabled();
    void moveSucceeded();

protected slots:
    void showOptions(int id);
    void showOptionsButton(int id);
    void mediaPlayerStateChanged(QMediaPlayer::MediaStatus);
    void mediaPlayerErrorChanged(QMediaPlayer::Error);
	void setRotationEnabled(bool rotationEnabled);

protected:
    AMSampleCameraGraphicsView* view_;
    QPointF* scale_;
    int numberOfPoints_;

    QList<QVector3D*>* coordinateList_;

	QList<double>* rotations_;

    QList<QPointF*>* pointList_;

    QGraphicsTextItem* fixItem_;

    bool showOptionPage_;

    int optionsPage_;



private slots:
    void setMotorMovementEnabled(bool motorMovementEnabled);
private:
    bool motorMovementEnabled_;

    QMediaPlayer* mediaPlayer_;

	bool rotationEnabled_;


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
    void initializePage();
    bool isComplete() const;
    void stopTimer();
    void startTimer(int msec);
protected slots:
    void nextPage();
    bool checkState();


private:
    QTimer* waitTimer_;
    bool waitingToMove_;


};


class AMViewPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMViewPage(QWidget* parent = 0);

    AMSampleCameraGraphicsView* view();

public slots:
    void setView(AMSampleCameraGraphicsView* view);

    void initializePage();
    void cleanupPage();


protected slots:
    void addView();

private:
    AMSampleCameraGraphicsView* view_;

    QLayout* layout_;
    QFrame* viewFrame_;

};

class AMCheckPage : public AMViewPage
{
    Q_OBJECT
public:
    AMCheckPage(QWidget* parent = 0);
    virtual void initializePage();

signals:
    void checked(bool);

protected slots:
    virtual void checkBoxChanged(bool state);

protected:
    QCheckBox* isConfigured_;
};

class AMWizardOptionPage : public AMWizardPage
{
    Q_OBJECT
public:
    AMWizardOptionPage(QWidget* parent = 0);
    virtual void initializePage();

    virtual void cleanupPage();

    virtual bool isComplete() const;
protected slots:
    void textChanged();

protected:
    QLineEdit** coordinateEdit_;
    QFrame* coordinateFrame_;
};


#endif // AMGRAPHICSVIEWWIZARD_H
