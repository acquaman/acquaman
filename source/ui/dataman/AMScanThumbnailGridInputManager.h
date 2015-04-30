#ifndef AMSCANTHUMBNAILGRIDINPUTMANAGER_H
#define AMSCANTHUMBNAILGRIDINPUTMANAGER_H

#include <QObject>
#include <QTimer>
#include <QRect>
#include <cmath>

// The distance which the mouse can travel during a drag before it is considered
// a rectangle selection and not just a slight drift in a point selection.
#define RECTANGLE_SELECTION_TRAVEL_TOLERANCE 50

class AMScanThumbnailGridView;
/**
 * A class for managing all the user input for a AMScanThumbnailGridView.
 * A flow chart of the interactions can be found at:
 *	https://creately.com/diagram/i7w2km9s1/pfyr3Eob1R0riPbWAUdEOcLEo%3D
 */
class AMScanThumbnailGridInputManager : public QObject {
	Q_OBJECT
public:
	/**
	 * Creates an instance of an AMScanThumbnailGridInputManager.
	 * @param parent::QObject* ~ The parent QObject
	 */
	explicit AMScanThumbnailGridInputManager(AMScanThumbnailGridView* view,
			QObject *parent = 0);

	void mouseDownAt(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons);

	void mouseMovedTo(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons);

	void mouseReleasedAt(int itemIndex, int positionX, int positionY, Qt::MouseButtons mouseButtons);



signals:
	void itemSelected(int itemIndex, QItemSelectionModel::SelectionFlags command);
	void itemDoubleClicked(int itemIndex);
	void contextMenuRequested(int itemIndex);
	void dragBegun(int itemIndex);
	void selectionRectangleChanged(const QRect& selectionRectangle);
	void selectionRectangleEnded(QItemSelectionModel::SelectionFlags command);
	void thumbnailIncremented(int itemIndex);
	void thumbnailDecremented(int itemIndex);

public slots:

private:

	void resetInteraction();

	int startPositionX_;
	int startPositionY_;

	int lastUpdatePositionX_;
	int lastUpdatePositionY_;

	int lastClickedItemIndex_;
	int lastMouseOverItemIndex_;
	
	QTimer doubleClickTimer_;
	QTimer hoverTimer_;
	AMScanThumbnailGridView* view_;

	bool dragInProgress_;

};

#endif // AMSCANTHUMBNAILGRIDINPUTMANAGER_H
