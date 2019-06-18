#include <QtWidgets>

#include "treeitem.h"
#include "treemodel.h"

TreeModel::TreeModel(const QStringList &headers, const QString &data, QObject *parent)
	: QAbstractItemModel(parent)
{
	QVector<QVariant> rootData;
	foreach(QString header, headers)
		rootData << header;

	rootItem = new TreeItem(rootData, this);
	QList<QString> strList = data.split(QString("\r\n"));
	setupModelData(data.split(QString("\r\n")), rootItem);
}

TreeItem::TreeItem(const QVector<QVariant> &data, TreeModel* treeModel, TreeItem* parent)
{
	parentItem = parent;
	itemData = data;
	model = treeModel;
}

TreeModel::~TreeModel()
{
	delete rootItem;
}

int TreeModel::columnCount(const QModelIndex & /* parent */) const
{
	return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole && role != Qt::EditRole)
		return QVariant();

	TreeItem *item = getItem(index);

	return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

TreeItem *TreeModel::getItem(const QModelIndex &index) const
{
	if (index.isValid()) {
		TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
		if (item)
			return item;
	}
	return rootItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
	int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (parent.isValid() && parent.column() != 0)
		return QModelIndex();

	TreeItem *parentItem = getItem(parent);

	TreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

bool TreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginInsertColumns(parent, position, position + columns - 1);
	success = rootItem->insertColumns(position, columns);
	endInsertColumns();

	return success;
}

bool TreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	bool success;

	beginInsertRows(parent, position, position + rows - 1);
	success = parentItem->insertChildren(position, rows, rootItem->columnCount());
	endInsertRows();

	return success;
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = getItem(index);
	TreeItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->childNumber(), 0, parentItem);
}

bool TreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
	bool success;

	beginRemoveColumns(parent, position, position + columns - 1);
	success = rootItem->removeColumns(position, columns);
	endRemoveColumns();

	if (rootItem->columnCount() == 0)
		removeRows(0, rowCount());

	return success;
}

bool TreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	bool success = true;

	beginRemoveRows(parent, position, position + rows - 1);
	success = parentItem->removeChildren(position, rows);
	endRemoveRows();

	return success;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	TreeItem *parentItem = getItem(parent);

	return parentItem->childCount();
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (role != Qt::EditRole)
		return false;

	TreeItem *item = getItem(index);
	bool result = item->setData(index.column(), value);

	if (result)
		emit dataChanged(index, index, { role });

	return result;
}

bool TreeModel::setHeaderData(int section, Qt::Orientation orientation,
	const QVariant &value, int role)
{
	if (role != Qt::EditRole || orientation != Qt::Horizontal)
		return false;

	bool result = rootItem->setData(section, value);

	if (result)
		emit headerDataChanged(orientation, section, section);

	return result;
}

void TreeModel::setupModelData(const QStringList &lines, TreeItem *parent)
{
	//QList<TreeItem*> parents;
	//QList<int> indentations;
	//parents << parent;
	//indentations << 0;

	//int number = 0;

	//while (number < lines.count()) {
	//	int position = 0;
	//	while (position < lines[number].length()) {
	//		if (lines[number].at(position) != ' ')
	//			break;
	//		++position;
	//	}

	//	QString lineData = lines[number].mid(position).trimmed();

	//	if (!lineData.isEmpty()) {
	//		// Read the column data from the rest of the line.
	//		QStringList columnStrings = lineData.split("\t", QString::SkipEmptyParts);
	//		QVector<QVariant> columnData;
	//		for (int column = 0; column < columnStrings.count(); ++column)
	//			columnData << columnStrings[column];

	//		if (position > indentations.last()) {
	//			// The last child of the current parent is now the new parent
	//			// unless the current parent has no children.

	//			if (parents.last()->childCount() > 0) {
	//				parents << parents.last()->child(parents.last()->childCount() - 1);
	//				indentations << position;
	//			}
	//		}
	//		else {
	//			while (position < indentations.last() && parents.count() > 0) {
	//				parents.pop_back();
	//				indentations.pop_back();
	//			}
	//		}

	//		// Append a new item to the current parent's list of children.
	//		TreeItem *parent = parents.last();
	//		parent->insertChildren(parent->childCount(), 1, rootItem->columnCount());
	//		for (int column = 0; column < columnData.size(); ++column)
	//			parent->child(parent->childCount() - 1)->setData(column, columnData[column]);
	//	}

	//	++number;
	//}
	TreeItem *rootItem = parent;
	int levelNum = 0;
	for (int i = 0; i < lines.size(); ++i)
	{
		int index = 0;
		while (lines.at(i).at(index) == '\t')
		{
			levelNum++;
			index++;
		}
		QList<QString> splitData = lines.at(i).right(lines.at(i).size() - index).split(' ');
		QVector<QVariant> itemData;
		for each (QString var in splitData)
		{
			itemData.append(var);
		}
		while (itemData.count() < parent->columnCount())
		{
			itemData.append(QVariant(" "));
		}
		while (levelNum > 0)
		{
			parent = parent->child(parent->childCount() - 1);
			levelNum--;
		}
		TreeItem *item = new TreeItem(itemData, this, parent);
		//parent->insertChildren(parent->childCount(), 1, item->columnCount());
		parent->addChild(item);
		parent = rootItem;
	}

}


void TreeModel::sort(int column, Qt::SortOrder order)
{
	if (column < 0 || column >= columnCount()) return;
	//layoutAboutToBeChanged and layoutChanged will be called by sortChildren
	rootItem->sortChildren(column, order, true);
}



bool TreeModel::itemLessThan(const QPair<TreeItem*, int>&left, const QPair<TreeItem*, int>&right)
{
	return *(left.first) < *(right.first);
}


bool TreeModel::itemGreaterThan(const QPair<TreeItem*, int>&left, const QPair<TreeItem*, int>&right)
{
	return *(right.first) < *(left.first);
}


void TreeModel::sortItems(QList<TreeItem*> *items, int column, Qt::SortOrder order)
{
	sortColumn = column;
	// store the original order of indexs
	QVector<QPair<TreeItem*, int>>sorting(items->count());
	for (size_t i = 0; i < sorting.count(); i++)
	{
		sorting[i].first = items->at(i);
		sorting[i].second = i;
	}

	//do the sorting
	const auto compare = (order == Qt::AscendingOrder ? &itemLessThan : &itemGreaterThan); //对函数取地址和不加地址符号是一样的，不加地址符时编译器会隐式转换
	std::stable_sort(sorting.begin(), sorting.end(), compare); // stable_sort 用的是快排
	QModelIndexList fromList;
	QModelIndexList toList;
	int colCount = columnCount();
	for (size_t r = 0; r < sorting.count(); r++)
	{
		int oldRow = sorting.at(r).second;
		if (oldRow == r) continue;
		TreeItem* item = sorting.at(r).first;
		items->replace(r, item);
		QModelIndexList persisIndexList = persistentIndexList(); // 暂时这样处理
		for (size_t c = 0; c < colCount; c++)
		{
			QModelIndex from = createIndex(oldRow, c, item);
			if (persisIndexList.contains(from))
			{
				QModelIndex to = createIndex(r, c, item);
				fromList << from;
				toList << to;
			}
		}
	}
	changePersistentIndexList(fromList, toList);
}


int TreeModel::sortColumnNum()
{
	return sortColumn;
}