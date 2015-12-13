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

#include "subscription_grouping_proxy_model.h"

#include "../enums.h"

#include <KI18n/KLocalizedString>

namespace KNode {
namespace GroupSelection {

SubscriptionGroupingProxyModel::SubscriptionGroupingProxyModel(QObject*parent)
    : BaseGroupingProxyModel(parent),
      mSubscribed(), mUnsubscribed(), mExisting()
{
    QList<QVector<QPersistentModelIndex>*> groupings;
    groupings << &mSubscribed;
    groupings << &mUnsubscribed;
    groupings << &mExisting;
    setGroupings(groupings);
}

SubscriptionGroupingProxyModel::~SubscriptionGroupingProxyModel()
{
}


const QString SubscriptionGroupingProxyModel::title(QVector<QPersistentModelIndex>* grouping) const
{
    if(grouping == &mSubscribed) {
        return i18nc("@title %1:number of groups", "Subscribed groups (%1)", mSubscribed.size());
    }
    if(grouping == &mUnsubscribed) {
        return i18nc("@title %1:number of groups", "Unsubscribed groups (%1)", mUnsubscribed.size());
    }
    if(grouping == &mExisting) {
        return i18nc("@title %1:number of groups", "Current subscription (%1)", mExisting.size());
    }
    return QString();
}


QVector<QPersistentModelIndex>* SubscriptionGroupingProxyModel::selectInternalGrouping(SubscriptionState state)
{
    switch(state) {
    case ExistingSubscription:
        return &mExisting;
        break;
    case NewSubscription:
        return &mSubscribed;
        break;
    case NewUnsubscription:
        return &mUnsubscribed;
        break;
    case NoStateChange:
    case Invalid:
        break;
    }
    return 0;
}

}
}
