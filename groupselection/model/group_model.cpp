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

#include "group_model.h"

#include <QtCore/QModelIndex>
#include <QtCore/QTimer>
#include <QtGui/QFont>
#include <KI18n/KLocalizedString>

#include "../enums.h"
#include "kngroupmanager.h"

namespace KNode {
namespace GroupSelection {

class Node
{
    public:
        Node(Node* _parent, const QString& _name, int index = -1)
        : name(_name), parent(_parent), groupIdx(index),
          mParsed(false), mChildren()
        {}

        ~Node()
        {
            parent = 0;
            qDeleteAll(mChildren);
            mChildren.clear();
        }

        bool isGroup() const
        {
            return groupIdx != -1;
        }

        void parse()
        {
            if(mChildren.isEmpty()) {
                mParsed = true;
                return;
            }

            // Prefix size
            Node* n = this;
            int prefix = 0;
            while(n->parent) {
                prefix += n->name.length() + 1; // Add 1 for the dot;
                n = n->parent;
            }

            const QList<Node*> unparsedChildren = mChildren;
            mChildren.clear();
            Node* intermediateParent = 0;
            QString currentSub;
            for(int i = 0 ; i < unparsedChildren.count() ; ++i) {
                Node* c = unparsedChildren.at(i);

                int end = c->name.indexOf('.', prefix);
                if(end == -1) {
                    mChildren.append(c);
                } else {
                    const QString sub = c->name.mid(prefix, end - prefix);
                    if(sub != currentSub) {
                        intermediateParent = new Node(this, sub);
                        mChildren.append(intermediateParent);
                        currentSub = sub;
                    }

                    c->parent = intermediateParent;
                    intermediateParent->mChildren.append(c);
                }
            }

            // Compact the tree
            if(mChildren.size() == 1) {
                n = mChildren.takeFirst();
                if(!n->isGroup()) {
                    name += '.' + n->name;
                    mChildren.append(n->mChildren);
                    Q_FOREACH(Node *child, mChildren) {
                        child->parent = this;
                    }

                    n->mChildren.clear(); // ~Node() would delete children Node recursively
                    delete n;

                    // Parse again with the new intermediate name
                    parse();
                } else {
                    mChildren.append(n);
                }
            }

            mParsed = true;
        }

        const QList<Node*>& children()
        {
            if(!mParsed) {
                parse();
            }
            return mChildren;
        }

        void setChildren(QList<Node*> children)
        {
            mChildren = children;
        }

        void setBuildTree(bool enable)
        {
            mParsed = !enable;
        }

        QString name;
        Node* parent;
        const int groupIdx;
    private:
        bool mParsed;
        QList<Node*> mChildren;
};



GroupModel::GroupModel(QObject* parent)
    : QAbstractItemModel(parent),
      mGroups(0), mRoot(0), mBuildTree(true)

{
    newList(0);
}

GroupModel::~GroupModel()
{
    delete mGroups;
    delete mRoot;
}


void GroupModel::newList(QList<KNGroupInfo>* groups)
{
    Node* root = new Node(0, QString(), -1);
    root->setBuildTree(mBuildTree);

    if(groups) {
        qSort(*groups);

        QList<Node *> children;
        for(int i = 0 ; i < groups->size() ; ++i) {
            children.append(new Node(root, groups->at(i).name, i));
        }
        root->setChildren(children);
    }

    beginResetModel();
    delete mRoot;
    mRoot = root;
    // Needed because of newList(mGroups) in modelAsTree()
    if(mGroups != groups) {
        delete mGroups;
        mGroups = groups;
    }
    endResetModel();
}

void GroupModel::modelAsTree(bool enable)
{
    if(enable != mBuildTree) {
        mBuildTree = enable;
        newList(mGroups);
    }
}



QVariant GroupModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case GroupModelColumn_Name:
            return i18n("Name");
        case GroupModelColumn_Description:
            return i18n("Description");
        }
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}


QVariant GroupModel::data(const QModelIndex& index, int role) const
{
    switch(role) {
        case Qt::DisplayRole: {
            Node* n = static_cast<Node*>(index.internalPointer());
            switch(index.column()) {
                case GroupModelColumn_Name:
                    if(n->isGroup()) {
                        const KNGroupInfo& gi = mGroups->at(n->groupIdx);
                        if(gi.status == KNGroup::moderated) {
                            const QString ret = gi.name + QLatin1String(" (m)");
                            return ret;
                        } else {
                            return gi.name;
                        }
                    } else {
                        return n->name;
                    }
                    break;
                case GroupModelColumn_Description:
                    if(n->isGroup()) {
                        return mGroups->at(n->groupIdx).description;
                    }
                    break;
            }
        }
            break;

        case Qt::FontRole:
            if(index.column() == GroupModelColumn_Name) {
                Node* n = static_cast<Node*>(index.internalPointer());
                if(n->isGroup() && mGroups->at(n->groupIdx).newGroup) {
                    QFont f;
                    f.setBold(true);
                    return QVariant::fromValue(f);
                }
            }
            break;

        case GroupInfoRole:
            Node* n = static_cast<Node*>(index.internalPointer());
            if(n->isGroup()) {
                return QVariant::fromValue(mGroups->at(n->groupIdx));
            }
            break;
    }

    return QVariant();
}

int GroupModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return GroupModelColumn_Count;
}

int GroupModel::rowCount(const QModelIndex& parent) const
{
    Node* p = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : mRoot;
    return p->children().count();
}

QModelIndex GroupModel::parent(const QModelIndex& child) const
{
    if(!child.isValid()) {
        return QModelIndex();
    }

    Node* c = static_cast<Node*>(child.internalPointer());
    Node* p = c->parent;

    // Parent is the root
    if(p->parent == 0) {
        return QModelIndex();
    }

    int row = p->parent->children().indexOf(p);
    return createIndex(row, 0, p);
}

QModelIndex GroupModel::index(int row, int column, const QModelIndex& parent) const
{
    Node* p = parent.isValid() ? static_cast<Node*>(parent.internalPointer()) : mRoot;
    if(row >= p->children().count()) {
        return QModelIndex();
    }
    return createIndex(row, column, p->children().at(row));
}


}
}
