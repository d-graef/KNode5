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

#ifndef KNODE_GROUPSELECTION_SUBSCRIPTIONSTATEPROXYMODEL_H
#define KNODE_GROUPSELECTION_SUBSCRIPTIONSTATEPROXYMODEL_H

#include <QtCore/QSet>
#include <QtCore/QSortFilterProxyModel>

#include "kngroupmanager.h"

namespace KNode {
namespace GroupSelection {

class SubscriptionStateProxyModel : public QSortFilterProxyModel
{
    public:
        SubscriptionStateProxyModel(QObject* parent);
        virtual ~SubscriptionStateProxyModel();

        void setOriginalSubscriptions(const QStringList& subscriptions);

        /**
         * Returns the list of groups that were subscribed.
         */
        QList<KNGroupInfo> subscribed() const;
        /**
         * Returns the list of groups that were unsubscribed.
         */
        QList<KNGroupInfo> unsubscribed() const;


        /**
         * Reimplemented to monitor subscription changes.
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
        /**
         * Reimplemented to provide data under the role SubscriptionOverrideRole.
         */
        virtual QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const;

    private:
        QSet<QString> mOriginalSubscriptions;
        QHash<KNGroupInfo, bool> mSubscriptionChanges;
};

}
}


#endif

