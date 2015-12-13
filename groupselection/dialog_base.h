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

#ifndef KNODE_GROUPSELECTION_BASEDIALOG_H
#define KNODE_GROUPSELECTION_BASEDIALOG_H

#include "ui_dialog_base.h"

#include <KDE/KDialog>

#include "kngroupmanager.h"
#include "knnntpaccount.h"

class QAbstractProxyModel;

namespace KNode {
namespace GroupSelection {

class GroupModel;
class SubscriptionStateProxyModel;

class BaseDialog : public KDialog, private Ui::BaseDialog
{
    Q_OBJECT

    public:
        BaseDialog(QWidget* parent, KNNntpAccount::Ptr account);
        virtual ~BaseDialog();

    public Q_SLOTS:
        void slotReceiveList(KNGroupListData::Ptr data);

    Q_SIGNALS:
        void loadList(KNNntpAccount::Ptr account);

    protected:
        /**
         * Call at the end of the setup (models/proxies, connection, etc. are in place).
         */
        virtual void setupDialog(QCheckBox* newOnly, QCheckBox* treeView, QCheckBox* subscribedOnly) = 0;
        virtual QList<KNGroupInfo>* receiveList(KNGroupListData::Ptr data) = 0;
        KNNntpAccount::Ptr account() const;
        /**
         * Returns the proxy responsible for grouping item in the changes view.
         */
        virtual QAbstractProxyModel* changesGroupingModel() = 0;
        /**
         * For the subclass to get the subscription / selection.
         */
        SubscriptionStateProxyModel* subscriptionModel() const;

    private Q_SLOTS:
        void init();

        /**
         * Revert change to the subscription state of the current selection
         * in the view associated with the click button.
         */
        void revertSelectionStateChange();

        /**
         * Called when the items selection in any of the view is modified.
         */
        void slotSelectionChange();

    private:
        KNNntpAccount::Ptr mAccount;
        GroupModel* mGroupModel;
        SubscriptionStateProxyModel* mSubscriptionModel;
};

}
}

#endif
