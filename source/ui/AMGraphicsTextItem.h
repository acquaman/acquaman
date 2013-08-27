#ifndef GRAPHICSTEXTITEM_H
#define GRAPHICSTEXTITEM_H
#include <QGraphicsTextItem>

class QTextDocument;

class AMGraphicsTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    enum {Type = UserType + 1};
    AMGraphicsTextItem(QGraphicsItem* parent = 0, QGraphicsScene* scene = 0);

    void setShapeIndex(int index);
    int shapeIndex();

    int type() const;

    void setPlainText(const QString &text);

    bool selectAll();
public slots:
    void selectAllText();
    void setSelectAll(const bool &selectAll);
	void clearSelection();

signals:
    void textChanged(int);
    void gotFocus(int);
    void returnPressed(int);

protected slots:
    void changingText();

protected:
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

protected:
    QTextDocument* document_;

    int shapeIndex_;

    bool dontChangeSelection_;
    bool selectAll_;
};

#endif // GRAPHICSTEXTITEM_H
