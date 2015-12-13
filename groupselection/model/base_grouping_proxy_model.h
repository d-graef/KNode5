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

#ifndef KNODE_GROUPSELECTION_BASEGROUPINGPROXYMODEL_H
#define KNODE_GROUPSELECTION_BASEGROUPINGPROXYMODEL_H

#include <QtCore/QSortFilterProxyModel>
#include <QtCore/QVector>

#include "../enums.h"

namespace KNode {
namespace GroupSelection {

/**
 * Base model to group newsgroups in the view of selection/subscription.
 * It assumes a SubscriptionChangeFilterProxyModel is a source model.
 */
class BaseGroupingProxyModel : public QAbstractProxyModel
{
    Q_OBJECT

    protected:
        // Use the internalId of QModelIndex to store the type of item.
        // Type >= 0, are grouping (and index in mGroupings).
        static const int INDEX_TYPE_TITLE = -1;

        static const int COLUMN = 0;

    public:
        BaseGroupingProxyModel(QObject* parent);
        virtual ~BaseGroupingProxyModel();

        /**
         * Reimplemented to provide data for titles.
         */
        virtual QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;
        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex& child) const;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;

        virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
        virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;

        virtual void setSourceModel(QAbstractItemModel* sourceModel);

    Q_SIGNALS:
        /**
         * Emitted to inform the view that a change occurs.
         */
        void changed();

    protected:
        /**
         * Needs to be call by subclasse to initialize the ordered list of groupings.
         */
        void setGroupings(const QList<QVector<QPersistentModelIndex>* > groupings)
        {
            mGroupings = groupings;
        }

        /**
         * Returns the displayed title of a group.
         */
        virtual const QString title(QVector<QPersistentModelIndex>* grouping) const = 0;

        /**
         * Called by #refreshInternal() to get the target group of a newsgroup.
         */
        virtual QVector<QPersistentModelIndex>* selectInternalGrouping(SubscriptionState state) = 0;

    private Q_SLOTS:
        /**
         * Connected to the source model to update groupings.
         */
        virtual void sourceModelRowsInserted(const QModelIndex& parent, int start, int end);
        /**
         * Connected to the source model to update groupings.
         */
        virtual void sourceModelRowsAboutToBeRemoved(const QModelIndex& parent, int start, int end);
        /**
         * Connected to the source model to update groupings.
         */
        void sourceModelDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);

        /**
         * Refresh the internal state when the source model is resetted.
         */
        void refreshInternal();

    private:
        /**
         * Returns the index of @p list to insert @p idx such as the QModelIndex in the list
         * are sorted by their display role.
         */
        int findInsertionPlace(QVector<QPersistentModelIndex>& list, const QModelIndex& idx) const;
        /**
         * Inserts an index at the right place into a grouping.
         */
        void insertIntoGrouping(const QModelIndex& index, QVector<QPersistentModelIndex>& grouping, int groupingRow);
        /**
         * Attempts to remove an index at the right place into a grouping.
         */
        bool removeFromGrouping(const QPersistentModelIndex& index, QVector<QPersistentModelIndex>& grouping, int groupingRow);

        QList<QVector<QPersistentModelIndex>*> mGroupings;
};

}
}

#endif
