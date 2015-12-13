/*
 * Copyright (c) 2015 Olivier Trichet <olivier@trichet.fr>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "base_grouping_proxy_model.h"

#include "../enums.h"

#include <QtGui/QFont>
#include <KI18n/KLocalizedString>

namespace KNode {
namespace GroupSelection {

BaseGroupingProxyModel::BaseGroupingProxyModel(QObject*parent)
    : QAbstractProxyModel(parent)
{
}

BaseGroupingProxyModel::~BaseGroupingProxyModel()
{
}


QModelIndex BaseGroupingProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if(!sourceIndex.isValid()) {
        return QModelIndex();
    }

    const QPersistentModelIndex index(sourceIndex);
    for(int type = 0 ; type < mGroupings.size() ; ++type) {
        int idx = mGroupings.at(type)->indexOf(index);
        if(idx != -1) {
            return createIndex(idx, COLUMN, type);
        }
    }

    return QModelIndex();
}

QModelIndex BaseGroupingProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if(!proxyIndex.isValid()) {
        return QModelIndex();
    }

    const int type = proxyIndex.internalId();
    if(0 <= type && type < mGroupings.size()) {
        QVector<QPersistentModelIndex>* grouping = mGroupings.at(type);
        return grouping->at(proxyIndex.row());
    }

    return QModelIndex();
}



QVariant BaseGroupingProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
    if(proxyIndex.internalId() == INDEX_TYPE_TITLE) {
        switch(role) {
        case Qt::DisplayRole: {
                const int type = proxyIndex.row();
                if(0 <= type && type < mGroupings.count()) {
                    return title(mGroupings.at(type));
                }
            }
            break;

        case Qt::FontRole: {
                QFont f = QAbstractProxyModel::data(proxyIndex, role).value<QFont>();
                f.setBold(true);
                return QVariant::fromValue(f);
            }
            break;

        default:
            return QVariant();
            break;
        }
    }

    return QAbstractProxyModel::data(proxyIndex, role);
}


Qt::ItemFlags BaseGroupingProxyModel::flags(const QModelIndex& index) const
{
    qint64 internalId = index.internalId();
    if(internalId == INDEX_TYPE_TITLE) {
        return Qt::NoItemFlags; // Inactive, unselectable, etc.
    }
    return QAbstractProxyModel::flags(index);
}



int BaseGroupingProxyModel::rowCount(const QModelIndex& parent) const
{
    if(!parent.isValid()) {
        return mGroupings.count();
    }

    if(parent.internalId() == INDEX_TYPE_TITLE) {
        const int type = parent.row();
        if(0 <= type && type < mGroupings.size()) {
            return mGroupings.at(type)->count();
        }
    }

    return 0;
}

int BaseGroupingProxyModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}



QModelIndex BaseGroupingProxyModel::parent(const QModelIndex& child) const
{
    const int type = child.internalId();
    if(type == INDEX_TYPE_TITLE) {
        return QModelIndex();
    }

    if(0 <= type && type < mGroupings.size()) {
        return createIndex(type, COLUMN, INDEX_TYPE_TITLE);
    }

    return QModelIndex();
}

QModelIndex BaseGroupingProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    if(!parent.isValid()) {
        if(0 <= row && row < mGroupings.size()) {
            return createIndex(row, column, INDEX_TYPE_TITLE);
        }
        return QModelIndex();
    }

    if(parent.internalId() == INDEX_TYPE_TITLE) {
        const int type = parent.row();
        QVector<QPersistentModelIndex>* grouping = mGroupings.at(type);
        if(row < grouping->size()) {
            return createIndex(row, column, type);
        }
    }

    return QModelIndex();
}

void BaseGroupingProxyModel::setSourceModel(QAbstractItemModel* source)
{
    if(sourceModel()) {
        sourceModel()->disconnect(this);
    }
    connect(source, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(sourceModelDataChanged(QModelIndex,QModelIndex)));
    connect(source, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(sourceModelRowsInserted(QModelIndex,int,int)));
    connect(source, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
            this, SLOT(sourceModelRowsAboutToBeRemoved(QModelIndex,int,int)));
    connect(source, SIGNAL(modelReset()),
            this, SLOT(refreshInternal()));

    QAbstractProxyModel::setSourceModel(source);

    refreshInternal();
}


void BaseGroupingProxyModel::refreshInternal()
{
    beginResetModel();

    Q_FOREACH(QVector<QPersistentModelIndex>* grouping, mGroupings) {
        grouping->clear();
    }

    QVector<QModelIndex> stack;
    stack << sourceModel()->index(0, 0).parent();
    while(!stack.isEmpty()) {
        const QModelIndex index = stack.first();

        const SubscriptionState s = index.data(SubscriptionStateRole).value<SubscriptionState>();
        QVector<QPersistentModelIndex>* dst = selectInternalGrouping(s);
        if(dst) {
            const int i = findInsertionPlace(*dst, index);
            dst->insert(i, index);
        }

        stack.remove(0);

        if(sourceModel()->hasChildren(index)) {
            const int rowCount = sourceModel()->rowCount(index);
            for(int r = 0 ; r < rowCount ; ++r) {
                stack << sourceModel()->index(r, 0, index);
            }
        }
    }

    endResetModel();

    emit changed();
}



void BaseGroupingProxyModel::sourceModelRowsInserted(const QModelIndex& parent, int start, int end)
{
    Q_ASSERT(start <= end);

    for(int row = start ; row <= end ; ++row) {
        const QModelIndex index = sourceModel()->index(row, 0, parent);
        const QVariant value = index.data(SubscriptionStateRole);
        SubscriptionState state = value.value<SubscriptionState>();
        QVector<QPersistentModelIndex>* dst = selectInternalGrouping(state);
        if(dst) {
            insertIntoGrouping(index, *dst, mGroupings.indexOf(dst));
        }
    }

    emit changed();
}

void BaseGroupingProxyModel::sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end)
{
    Q_ASSERT(start <= end);

    for(int row = start ; row <= end ; ++row) {
        const QPersistentModelIndex index(sourceModel()->index(row, 0, parent));
        for(int type = 0 ; type < mGroupings.size() ; ++type) {
            QVector<QPersistentModelIndex>* grouping = mGroupings.at(type);
            if(removeFromGrouping(index, *grouping, type)) {
                break;
            }
        }
    }

    emit changed();
}

void BaseGroupingProxyModel::sourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
    Q_ASSERT(topLeft.row() <= bottomRight.row());
    Q_ASSERT(topLeft.parent() == bottomRight.parent());

    sourceModelRowsAboutToBeRemoved(topLeft.parent(), topLeft.row(), bottomRight.row());
    sourceModelRowsInserted(topLeft.parent(), topLeft.row(), bottomRight.row());
}


void BaseGroupingProxyModel::insertIntoGrouping(const QModelIndex& index, QVector<QPersistentModelIndex>& grouping, int groupingRow)
{
    int i = findInsertionPlace(grouping, index);
    const QModelIndex parent = createIndex(groupingRow, COLUMN, INDEX_TYPE_TITLE);
    beginInsertRows(parent, i, i);
    grouping.insert(i, index);
    endInsertRows();
    emit dataChanged(parent, parent);
}

bool BaseGroupingProxyModel::removeFromGrouping(const QPersistentModelIndex& index, QVector<QPersistentModelIndex>& grouping, int groupingRow)
{
    int i = grouping.indexOf(index);
    if(i != -1) {
        const QModelIndex parent = createIndex(groupingRow, COLUMN, INDEX_TYPE_TITLE);
        beginRemoveRows(parent, i, i);
        grouping.remove(i);
        endRemoveRows();
        emit dataChanged(parent, parent);
        return true;
    }
    return false;
}

int BaseGroupingProxyModel::findInsertionPlace(QVector<QPersistentModelIndex>& list, const QModelIndex& idx) const
{
    const QString name = idx.data(Qt::DisplayRole).toString();
    int i = 0;
    while(i < list.size()) {
        const QString listItemName = list.at(i).data(Qt::DisplayRole).toString();
        if(name < listItemName) {
            break;
        }
        ++i;
    }
    return i;
}


}
}
