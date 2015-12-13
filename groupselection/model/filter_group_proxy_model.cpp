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

#include "filter_group_proxy_model.h"

#include "../enums.h"
#include "kngroupmanager.h"

namespace KNode {
namespace GroupSelection {


GroupFilterProxyModel::GroupFilterProxyModel(QObject* parent)
    : KRecursiveFilterProxyModel(parent),
      mFilterNew(false), mFilterSubscribed(false)
{
}

GroupFilterProxyModel::~GroupFilterProxyModel()
{
}

void GroupFilterProxyModel::filterNew(bool enable)
{
    beginResetModel();
    mFilterNew = enable;
    endResetModel();
}

void GroupFilterProxyModel::filterSubscribed(bool enable)
{
    beginResetModel();
    mFilterSubscribed = enable;
    endResetModel();
}


bool GroupFilterProxyModel::acceptRow(int sourceRow, const QModelIndex& sourceParent) const
{
    bool accept = true;
    if(mFilterNew || mFilterSubscribed) {
        const QVariant v = sourceModel()->index(sourceRow, 0, sourceParent).data(GroupInfoRole);
        accept &= v.isValid();
        if(accept && mFilterNew) {
            accept &= v.value<KNGroupInfo>().newGroup;
        }
        if(accept && mFilterSubscribed) {
            accept &= v.value<KNGroupInfo>().subscribed;
        }
    }
    return accept;
}


}
}
