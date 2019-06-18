#include "treeitem.h"
#include <qdatetime.h>
#include <QStringList>

TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
{
	parentItem = parent;
	itemData = data;
}

TreeItem::~TreeItem()
{
	qDeleteAll(childItems);
}

TreeItem *TreeItem::child(int number)
{
	return childItems.value(number);
}

int TreeItem::childCount() const
{
	return childItems.count();
}

int TreeItem::childNumber() const
{
	if (parentItem)
		return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));

	return 0;
}

int TreeItem::columnCount() const
{
	return itemData.count();
}

QVariant TreeItem::data(int column) const
{
	return itemData.value(column);
}

bool TreeItem::insertChildren(int position, int count, int columns)
{
	if (position < 0 || position > childItems.size())
		return false;

	for (int row = 0; row < count; ++row) {
		QVector<QVariant> data(columns);
		TreeItem *item = new TreeItem(data, this);
		childItems.insert(position, item);
	}

	return true;
}

bool TreeItem::insertColumns(int position, int columns)
{
	if (position < 0 || position > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.insert(position, QVariant());

	foreach(TreeItem *child, childItems)
		child->insertColumns(position, columns);

	return true;
}

TreeItem *TreeItem::parent()
{
	return parentItem;
}

bool TreeItem::removeChildren(int position, int count)
{
	if (position < 0 || position + count > childItems.size())
		return false;

	for (int row = 0; row < count; ++row)
		delete childItems.takeAt(position);

	return true;
}

bool TreeItem::removeColumns(int position, int columns)
{
	if (position < 0 || position + columns > itemData.size())
		return false;

	for (int column = 0; column < columns; ++column)
		itemData.remove(position);

	foreach(TreeItem *child, childItems)
		child->removeColumns(position, columns);

	return true;
}

bool TreeItem::setData(int column, const QVariant &value)
{
	if (column < 0 || column >= itemData.size())
		return false;

	itemData[column] = value;
	return true;
}


bool TreeItem::addChild(TreeItem* child)
{
	childItems.append(child);
	return true;
}


void TreeItem::setView(QTreeView* treeView)
{
	view = treeView;
	foreach(TreeItem* item, childItems)
		item->setView(treeView);
}


bool TreeItem::operator<(const TreeItem& other) const
{
	int sortColumn;// = view->header()->sortIndicatorSection();
	sortColumn = model->sortColumnNum(); // 暂时这样处理
	if (sortColumn < 0) sortColumn = 0;
	const QVariant v1 = data(sortColumn);
	const QVariant v2 = other.data(sortColumn);
	return isVariantLessThen(v1, v2);
}


bool TreeItem::isVariantLessThen(const QVariant &left, const QVariant &right)
{
	if (left.userType() == QVariant::Invalid)
		return false;
	if (right.userType() == QVariant::Invalid)
		return true;
	switch (left.userType()) {
	case QVariant::Int:
		return left.toInt() < right.toInt();
	case QVariant::UInt:
		return left.toUInt() < right.toUInt();
	case QVariant::LongLong:
		return left.toLongLong() < right.toLongLong();
	case QVariant::ULongLong:
		return left.toULongLong() < right.toULongLong();
	case QMetaType::Float:
		return left.toFloat() < right.toFloat();
	case QVariant::Double:
		return left.toDouble() < right.toDouble();
	case QVariant::Char:
		return left.toChar() < right.toChar();
	case QVariant::Date:
		return left.toDate() < right.toDate();
	case QVariant::Time:
		return left.toTime() < right.toTime();
	case QVariant::DateTime:
		return left.toDateTime() < right.toDateTime();
	case QVariant::String:
	default:
		return left.toString().compare(right.toString()) < 0;
	}

}


void TreeItem::sortChildren(int column, Qt::SortOrder order, bool climb)
{
	if (!model) return;
	//if (model->isChanging()) return; //可能需要
	//QTreeModel::SkipSorting skipSorting(model); 可能需要
	emit model->layoutAboutToBeChanged();
	sortChildrenPrivate(column, order, climb);
	emit model->layoutChanged();
}


void TreeItem::sortChildrenPrivate(int column, Qt::SortOrder order, bool climb)
{
	if (!model) return;
	model->sortItems(&childItems, column, order);
	if (climb)
	{
		QList<TreeItem*>::iterator it = childItems.begin();
		for (; it != childItems.end(); it++)
		{
			(*it)->sortChildrenPrivate(column, order, climb);
		}
	}
}