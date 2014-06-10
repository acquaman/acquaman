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
		//connect(childNode, SIGNAL(childNodeAppended(AMPointerTreeNode*)), this, SIGNAL(childNodeAppended(AMPointerTreeNode*)));
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
