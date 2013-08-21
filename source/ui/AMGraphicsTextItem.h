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

signals:
    void textChanged(int);
    void gotFocus(int);

protected slots:
    void changingText();

protected:
    void focusInEvent(QFocusEvent *event);
protected:
    QTextDocument* document_;

    int shapeIndex_;
};

#endif // GRAPHICSTEXTITEM_H
