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


#include "AMPointerTree.h"

// AMPointerTreeNode
////////////////////////////

 AMPointerTreeNode::~AMPointerTreeNode(){}
AMPointerTreeNode::AMPointerTreeNode(void *item, AMPointerTreeNode *parentNode, QObject *parent) :
	QObject(parent)
{
	item_ = item;
	parentNode_ = parentNode;
	descendantCount_ = -1;
}

void* AMPointerTreeNode::item(){
	return item_;
}

const AMPointerTreeNode* AMPointerTreeNode::parentNode() const{
	return parentNode_;
}

const AMPointerTreeNode* AMPointerTreeNode::childNodeAt(int index) const{
	if(index == -1 && childrenNodes_.count() > 0)
		return childrenNodes_.last();
	if(index < -2 || index > childrenNodes_.count())
		return 0; //NULL
	return childrenNodes_.at(index);
}

int AMPointerTreeNode::childCount() const{
	return childrenNodes_.count();
}

int AMPointerTreeNode::descendantCount() const{
	return descendantCount_;
	/*
	if(descendantCount_ != -1)
		return descendantCount_;

	int totalDescendantCount = 0;
	for(int x = 0; x < childrenNodes_.count(); x++)
		totalDescendantCount += childrenNodes_.at(x)->descendantCount();

	totalDescendantCount += childCount();
	descendantCount_ = totalDescendantCount;
	return descendantCount_;
	*/
}

QList<void*> AMPointerTreeNode::clearNode(){
	QList<void*> descendantItems;
	while(!childrenNodes_.isEmpty()){
		descendantItems.append(childrenNodes_.last()->clearNode());
		AMPointerTreeNode *lastChildNode = childrenNodes_.takeLast();
		delete lastChildNode;
	}
	if(item_){
		descendantItems.append(item_);
		item_ = 0;
	}
	parentNode_ = 0;
	descendantCount_ = -1;
	return descendantItems;
}

void AMPointerTreeNode::appendChildNode(AMPointerTreeNode *childNode){
	if(childNode){
		childrenNodes_.append(childNode);
		emit descendantNodeAppended(childNode);
		descendantCount_++;
		connect(childNode, SIGNAL(descendantNodeAppended(AMPointerTreeNode*)), this, SLOT(onDescendantNodeAppended(AMPointerTreeNode*)));
	}
}

void AMPointerTreeNode::onDescendantNodeAppended(AMPointerTreeNode *descendantNode){
	emit descendantNodeAppended(descendantNode);
	descendantCount_++;
}


// AMPointerTree
////////////////////////////

AMPointerTree::AMPointerTree(AMPointerTreeNode *rootNode, QObject *parent) :
	QObject(parent)
{
	rootNode_ = rootNode;
	connect(rootNode_, SIGNAL(descendantNodeAppended(AMPointerTreeNode*)), this, SLOT(onDescendantNodeAppended(AMPointerTreeNode*)));
}

const AMPointerTreeNode* AMPointerTree::nodeFromItem(void *item) const{
	if(itemsToNodes_.contains(item))
		return itemsToNodes_.value(item);
	return 0;
}

AMPointerTreeNode* AMPointerTree::rootNode(){
	return rootNode_;
}

QList<void*> AMPointerTree::clearTree(){
	return rootNode_->clearNode();
}

void AMPointerTree::onDescendantNodeAppended(AMPointerTreeNode *descendantNode){
	if(!itemsToNodes_.contains(descendantNode->item()))
		itemsToNodes_.insert(descendantNode->item(), descendantNode);
}
