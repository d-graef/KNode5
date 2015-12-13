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

#ifndef KNODE_GROUPSELECTION_GROUPMODEL_H
#define KNODE_GROUPSELECTION_GROUPMODEL_H

#include <QtCore/QAbstractItemModel>

#include "knnntpaccount.h"
#include "kngroup.h"
#include "kngroupmanager.h"

class KNGroupInfo;

namespace KNode {
namespace GroupSelection {

class Node;

/**
 * Base mode representing the list of all groups.
 */
class GroupModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        GroupModel(QObject* parent);
        ~GroupModel();

        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual QVariant data(const QModelIndex& index, int role) const;
        virtual int columnCount(const QModelIndex& parent) const;
        virtual int rowCount(const QModelIndex& parent) const;
        virtual QModelIndex parent(const QModelIndex& child) const;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;

        void newList(QList<KNGroupInfo>* groups);

        bool modelAsTree() const
        {
            return mBuildTree;
        }

    public Q_SLOTS:
        void modelAsTree(bool enable);

    private:
        QList<KNGroupInfo>* mGroups;
        Node* mRoot;
        bool mBuildTree;
};

}
}

#endif
