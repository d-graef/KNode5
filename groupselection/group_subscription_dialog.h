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

#ifndef KNODE_GROUPSELECTION_SUBSCRIPTIONDIALOG_H
#define KNODE_GROUPSELECTION_SUBSCRIPTIONDIALOG_H

#include "dialog_base.h"

namespace KNode {
namespace GroupSelection {

class SubscriptionDialog : public BaseDialog
{
    Q_OBJECT

    public:
        SubscriptionDialog(QWidget* parent, KNNntpAccount::Ptr account);
        ~SubscriptionDialog();

        /**
         * Returns the list of groups that were subscribed.
         */
        void toSubscribe(QList<KNGroupInfo>& list);
        /**
         * Returns the list of groups that were unsubscribed.
         */
        void toUnsubscribe(QStringList& list);

    protected:
        virtual void setupDialog(QCheckBox* newOnly, QCheckBox* treeView, QCheckBox* subscribedOnly);
        virtual QList<KNGroupInfo>* receiveList(KNGroupListData::Ptr data);
        virtual QAbstractProxyModel* changesGroupingModel();

    Q_SIGNALS:
        void fetchList(KNNntpAccount::Ptr account);
        void checkNew(KNNntpAccount::Ptr account, QDate since);

    private Q_SLOTS:
        void slotRequestNewList();
        void slotRequestGroupSince();
};

}
}

#endif
