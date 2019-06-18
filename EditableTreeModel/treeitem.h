#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>
#include <QVector>
#include <qtreeview.h>
#include "treemodel.h"
class TreeItem
{
public:
	explicit TreeItem(const QVector<QVariant> &data, TreeItem *parent = 0);
	explicit TreeItem(const QVector<QVariant> &data, TreeModel* treeModel, TreeItem* parent = 0);
	~TreeItem();

	TreeItem *child(int number);
	int childCount() const;
	int columnCount() const;
	QVariant data(int column) const;
	bool insertChildren(int position, int count, int columns);
	bool insertColumns(int position, int columns);
	TreeItem *parent();
	bool removeChildren(int position, int count);
	bool removeColumns(int position, int columns);
	int childNumber() const;
	bool setData(int column, const QVariant &value);
	bool addChild(TreeItem* child);

	/* add sort feature support*/
	void setView(QTreeView* treeView);
	bool operator<(const TreeItem& other) const;
	static bool isVariantLessThen(const QVariant &left, const QVariant &right);
	void sortChildren(int column, Qt::SortOrder order, bool climb);

private:
	void sortChildrenPrivate(int column, Qt::SortOrder order, bool climb);
private:
	QList<TreeItem*> childItems;
	QVector<QVariant> itemData;
	TreeItem *parentItem;
	QTreeView* view; // 暂时放在这里，可能会用到
	TreeModel* model; //
};

#endif // TREEITEM_H