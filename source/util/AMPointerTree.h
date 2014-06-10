#ifndef AMPOINTERTREE_H
#define AMPOINTERTREE_H

#include <QObject>
#include <QHash>

class AMPointerTreeNode : public QObject
{
Q_OBJECT
public:
 	virtual ~AMPointerTreeNode();
	AMPointerTreeNode(void *item, AMPointerTreeNode *parentNode, QObject *parent = 0);

	void* item();
	const AMPointerTreeNode* parentNode() const;
	const AMPointerTreeNode* childNodeAt(int index) const;
	int childCount() const;
	int descendantCount() const;

	QList<void*> clearNode();

public slots:
	void appendChildNode(AMPointerTreeNode *childNode);

signals:
	void descendantNodeAppended(AMPointerTreeNode *descendantNode);

protected slots:
	void onDescendantNodeAppended(AMPointerTreeNode *descendantNode);

protected:
	void *item_;
	AMPointerTreeNode *parentNode_;
	QList<AMPointerTreeNode*> childrenNodes_;
	int descendantCount_;
};

class AMPointerTree : public QObject
{
Q_OBJECT
public:
	AMPointerTree(AMPointerTreeNode *rootNode, QObject *parent = 0);

	const AMPointerTreeNode* nodeFromItem(void *item) const;

	AMPointerTreeNode* rootNode();

	QList<void*> clearTree();

protected slots:
	void onDescendantNodeAppended(AMPointerTreeNode *descendantNode);

protected:
	AMPointerTreeNode *rootNode_;
	QHash<void*, AMPointerTreeNode*> itemsToNodes_;
};

#endif // AMPOINTERTREE_H
