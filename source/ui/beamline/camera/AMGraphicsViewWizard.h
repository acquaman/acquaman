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
class QGraphicsPolygonItem;
class QCheckBox;
class QGraphicsTextItem;
class QLineEdit;


/// Wizard class
/// contains a Graphics view with a video.
/// Also contains the common functionality
/// of the Camera, Beam, Sample plate, and rotation wizards
/// such as move sample plate to point,
/// mapping point to the video as well as
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
	/// Default page numbers.  If using a different scheme, define own page numbers
	enum
	{
		Default_Intro,
		Default_Check,
		Default_Option,
		Default_Final,
		Default_Free
	};
	 /// These are the different message elements
	enum
	{
		Wizard_Title,
		Help_Title,
		Title,
		Text,
		Help,
		Other,
		Default
	};
	/// next page -1 is always finish
	enum {FINISHED = -1};
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



    /// the number of points for this wizard.
    /// used to generate the options page
    int numberOfPoints() const;
	/// number of set/wait type pages in the wizard
	/// usually the same as number of points
	int numberOfPages() const;



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

	/// checks if rotation is enabled as a part of the wizard
	/// if it is not, no rotation will happen
	bool rotationEnabled() const;

	/// returns the list of coordinates
	virtual const QList<QVector3D*>* getCoordinateList() const;

	/// returns the list of points
	virtual const QList<QPointF*>* getPointList() const;

	/// returns the list of rotations
	virtual const QList<double>* getRotationList() const;



public slots:
    /// sets the view
    void setView(AMSampleCameraGraphicsView* view);

    /// sets the point in pointList at index to point
    virtual void setPoint(QPointF point, int index);

    /// sets the coordinate in coordinateList at index to coordinate
    virtual void setCoordinate(QVector3D coordinate, int index);

	/// sets the rotation in rotationList at index
	/// this is only relevant if rotation is enabled
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

	/// updates a shape in the scene
    void updateShape(QGraphicsPolygonItem* item);

    /// repositions the "fix text".
    void fixText();

    /// connects the move succeeded signal to a slot.
    void testMoveSlot();

signals:
	/// emitted when finished
    void done();
	/// emitted when non-rotating move requested
    void moveTo(QVector3D);
	/// emitted when rotating move requested
	void moveTo(QVector3D,double);
	/// requests motor movement state
    void requestMotorMovementEnabled();
	/// emitted when move has finished
    void moveSucceeded();

protected slots:
	virtual void setHasHelpButton(bool hasHelp);
	virtual void setDefaultWindowTitle();
	virtual void showOptions(int id);
	virtual void showOptionsButton(int id);
    void mediaPlayerStateChanged(QMediaPlayer::MediaStatus);
    void mediaPlayerErrorChanged(QMediaPlayer::Error);
	void setRotationEnabled(bool rotationEnabled);
	void setNumberOfPages(int numberOfPages);
	void setNumberOfPoints(int numberOfPoints);
	void setShowOptionPage(bool showOptionPage);


	void coordinateListAppend(QVector3D *coordinate);
	void pointListAppend(QPointF *point);
	void rotationsAppend(double rotation);
	void setFreePage(int freePage);

protected:
	/// number of coordinates
	virtual int coordinateListCount() const;
	/// the relative id of the current page
	virtual int relativeId() const;
	/// the relative id of the specified page
	virtual int relativeId(int pageId) const;
	/// get the page number of wait or set page
	/// based on its relative id (should be 0<=relativeId<numberOfPages)
	virtual int pageWait(int relativeId) const;
	virtual int pageSet(int relativeId) const;
	/// returns the free page number. Must set it if not using default pages
	virtual int freePage() const;

	/// checks to see if the page is a wait page or free page
	virtual bool isWaitPage(int pageNumber) const;
	virtual bool isSetPage(int pageNumber) const;

	/// returns the list of coordinates
	virtual QList<QVector3D*>* coordinateList() const;

	/// returns the list of points
	virtual QList<QPointF*>* pointList() const;

	/// returns the list of rotations
	virtual QList<double>* rotations() const;



private slots:

	/// sets the state of motorMovementEnabled
    void setMotorMovementEnabled(bool motorMovementEnabled);

private:
	/// used to see if motor movement is actually allowed.
		// if this is true but motorMovement is not allowed
		// in the main application, it will hang.
    bool motorMovementEnabled_;

	/// the media player used to display the camera feed
    QMediaPlayer* mediaPlayer_;

	/// sets whether the wizard controls rotation
	/// some wizards may need rotation, some may
	/// need to have no rotation
	bool rotationEnabled_;

	/// the number of points to save
	int numberOfPoints_;

	/// the number of wait/set type pages
	int numberOfPages_;

	/// sets whether to show the option page next
	bool showOptionPage_;

	/// first free page (highest assigned page number + 1)
	int freePage_;

	/// the page id to show the options page from
	int optionsPage_;

	/// The fix item is used to prevent the screen from flashing
	/// due to having two views up at the same time
	QGraphicsTextItem* fixItem_;

	/// List of points
	QList<QPointF*>* pointList_;

	/// The graphics view used by this wizard
	AMSampleCameraGraphicsView* view_;

	/// the scale of the view's image
	QPointF* scale_;



	/// a list of coordinates to move to.
	QList<QVector3D*>* coordinateList_;

	/// list of rotations corresponding to coordinateList_
	QList<double>* rotations_;


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
