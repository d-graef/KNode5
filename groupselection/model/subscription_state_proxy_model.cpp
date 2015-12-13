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

#include "subscription_state_proxy_model.h"

#include <KDE/KIcon>

#include "../enums.h"

namespace KNode {
namespace GroupSelection {

SubscriptionStateProxyModel::SubscriptionStateProxyModel(QObject* parent)
  : QSortFilterProxyModel(parent),
    mOriginalSubscriptions(),
    mSubscriptionChanges()
{
}

SubscriptionStateProxyModel::~SubscriptionStateProxyModel()
{
}

void SubscriptionStateProxyModel::setOriginalSubscriptions(const QStringList& subscriptions)
{
    beginResetModel();
    mOriginalSubscriptions = QSet<QString>::fromList(subscriptions);
    endResetModel();
}


QList< KNGroupInfo > SubscriptionStateProxyModel::subscribed() const
{
    return mSubscriptionChanges.keys(true);
}

QList< KNGroupInfo > SubscriptionStateProxyModel::unsubscribed() const
{
    return mSubscriptionChanges.keys(false);
}


QVariant SubscriptionStateProxyModel::data(const QModelIndex& index, int role) const
{
    switch (role) {
        case SubscriptionStateRole: {
            const QVariant v = QAbstractProxyModel::data(index, GroupInfoRole);
            if(v.isValid()) {
                const KNGroupInfo& gi = v.value<KNGroupInfo>();
                if(mSubscriptionChanges.contains(gi)) {
                    const bool subscribe = mSubscriptionChanges.value(gi);
                    return QVariant::fromValue((subscribe ? NewSubscription : NewUnsubscription));
                } else if(mOriginalSubscriptions.contains(gi.name)) {
                    return QVariant::fromValue(ExistingSubscription);
                } else {
                    return QVariant::fromValue(NoStateChange);
                }
            }
        }
        break;

        case Qt::DecorationRole:
            if(index.column() == GroupModelColumn_Name) {
                const QVariant v = QAbstractProxyModel::data(index, GroupInfoRole);
                if(v.isValid()) {
                    const KNGroupInfo& gi = v.value<KNGroupInfo>();
                    if(mSubscriptionChanges.contains(gi)) {
                        return (mSubscriptionChanges.value(gi) ? KIcon("news-subscribe") : KIcon("news-unsubscribe"));
                    } else if(mOriginalSubscriptions.contains(gi.name)) {
                        return KIcon("view-pim-news");
                    } else {
                        return QVariant(); // No icon if not subscribed, nor state changed.
                    }
                }
            }
            break;
    }

    return QAbstractProxyModel::data(index, role);
}

bool SubscriptionStateProxyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(role == SubscriptionStateRole) {
        bool subscribe;
        const SubscriptionState state = value.value<SubscriptionState>();
        if(state == NewSubscription) {
            subscribe = true;
        } else if(state == NewUnsubscription) {
            subscribe = false;
        } else {
            return false;
        }

        const QVariant v = QAbstractProxyModel::data(index, GroupInfoRole);
        if(v.isValid()) {

            const KNGroupInfo& gi = v.value<KNGroupInfo>();
            if(mSubscriptionChanges.contains(gi)) {
                if(mSubscriptionChanges.value(gi) != subscribe) {
                    mSubscriptionChanges.remove(gi); // change reverted
                    emit dataChanged(index, index);
                    return true;
                }
            } else {
                mSubscriptionChanges.insert(gi, subscribe);
                emit dataChanged(index, index);
                return true;
            }
        }
        return false;
    }

    return QAbstractProxyModel::setData(index, value, role);
}


}
}
