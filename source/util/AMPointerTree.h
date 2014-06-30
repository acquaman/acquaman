/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


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
