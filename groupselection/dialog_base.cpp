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

#include "dialog_base.h"

#include <KDE/KIcon>
#include <KDE/KRecursiveFilterProxyModel>
#include <QtCore/QTimer>

#include "enums.h"
#include "model/checked_state_proxy_model.h"
#include "model/group_model.h"
#include "model/filter_group_proxy_model.h"
#include "model/subscription_state_proxy_model.h"

#include "scheduler.h"

namespace KNode {
namespace GroupSelection {

BaseDialog::BaseDialog(QWidget* parent, KNNntpAccount::Ptr account)
    : KDialog(parent),
      mAccount(account),
      mGroupModel(0),
      mSubscriptionModel(0)
{
    setupUi(this);
    setMainWidget(page);
    if(QApplication::isLeftToRight()) {
        mAddChangeButton->setIcon(KIcon("arrow-right"));
        mRevertChangeButton->setIcon(KIcon("arrow-left"));
    } else {
        mAddChangeButton->setIcon(KIcon("arrow-left"));
        mRevertChangeButton->setIcon(KIcon("arrow-right"));
    }

    QTimer::singleShot(5, this, SLOT(init()));
}



BaseDialog::~BaseDialog()
{
    KNode::Scheduler* s = KNGlobals::self()->scheduler();
    s->cancelJobs(KNJobData::JTLoadGroups);
    s->cancelJobs(KNJobData::JTFetchGroups);
}


KNNntpAccount::Ptr BaseDialog::account() const
{
    return mAccount;
}

SubscriptionStateProxyModel* BaseDialog::subscriptionModel() const
{
    return mSubscriptionModel;
}


void BaseDialog::slotReceiveList(KNGroupListData::Ptr data)
{
    QList<KNGroupInfo>* groups = receiveList(data);
    mGroupModel->newList(groups);
}


void BaseDialog::init()
{
    // Group model
    mGroupModel = new GroupModel(this);
    // Proxy that keeps trace of (un)subscription changes
    mSubscriptionModel = new SubscriptionStateProxyModel(this);
    mSubscriptionModel->setSourceModel(mGroupModel);

    // View of all groups and its dedicated proxy models
    KRecursiveFilterProxyModel* searchProxy = new KRecursiveFilterProxyModel(this);
    searchProxy->setSourceModel(mSubscriptionModel);
    GroupFilterProxyModel* filterGroupProxy = new GroupFilterProxyModel(this);
    filterGroupProxy->setSourceModel(searchProxy);
    CheckedStateConvertionProxyModel* checkableConvertionProxyModel = new CheckedStateConvertionProxyModel(this);
    checkableConvertionProxyModel->setSourceModel(filterGroupProxy);
    mGroupsView->setModel(checkableConvertionProxyModel);

    // View of subscription changes and its models
    QAbstractProxyModel* groupByStateProxy = changesGroupingModel();
    groupByStateProxy->setSourceModel(mSubscriptionModel);
    mChangeView->setModel(groupByStateProxy);
    connect(groupByStateProxy, SIGNAL(changed()),
            mChangeView, SLOT(expandAll()));

    // Filter & sort
    mSearchLine->setProxy(searchProxy);
    searchProxy->setFilterCaseSensitivity(Qt::CaseInsensitive);
    searchProxy->setSortLocaleAware(true);
    searchProxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    searchProxy->sort(GroupModelColumn_Name, Qt::AscendingOrder);
    mNewOnlyCheckbox->setChecked(filterGroupProxy->isNewOnlyEnabled());
    connect(mNewOnlyCheckbox, SIGNAL(toggled(bool)),
            filterGroupProxy, SLOT(filterNew(bool)));
    mSubscribedOnlyCheckbox->setChecked(filterGroupProxy->isSubscribedOnlyEnabled());
    connect(mSubscribedOnlyCheckbox, SIGNAL(toggled(bool)),
            filterGroupProxy, SLOT(filterSubscribed(bool)));
    mTreeviewCheckbox->setChecked(mGroupModel->modelAsTree());
    connect(mTreeviewCheckbox, SIGNAL(toggled(bool)),
            mGroupModel, SLOT(modelAsTree(bool)));

    // Operation on selection
    connect(mAddChangeButton, SIGNAL(clicked(bool)),
            this, SLOT(revertSelectionStateChange()));
    connect(mRevertChangeButton, SIGNAL(clicked(bool)),
            this, SLOT(revertSelectionStateChange()));
    connect(mChangeView->selectionModel(), SIGNAL(selectionChanged(QItemSelection, QItemSelection)),
            this, SLOT(slotSelectionChange()));
    connect(mGroupsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(slotSelectionChange()));

    // Inialize the state
    setupDialog(mNewOnlyCheckbox, mTreeviewCheckbox, mSubscribedOnlyCheckbox);

    int width = mViewSplitter->size().width();
    mViewSplitter->setSizes(QList<int>() << (width * 2 / 3) << (width * 1 / 3));

    QHeaderView* groupsViewHdr = mGroupsView->header();
    width = groupsViewHdr->size().width();
    groupsViewHdr->setSortIndicator(GroupModelColumn_Name, Qt::AscendingOrder);
    groupsViewHdr->resizeSection(GroupModelColumn_Name, width * 4 / 7);
    groupsViewHdr->resizeSection(GroupModelColumn_Description, width * 2 / 7);

    // Request the list of groups
    emit loadList(account());
}


void BaseDialog::revertSelectionStateChange()
{
    QAbstractItemView* view = (sender() == mAddChangeButton ? mGroupsView : mChangeView);

    // Operate on QPersistentModelIndex because call
    // to setData() below will invalidates QModelIndex.

    QList<QPersistentModelIndex> persistentIndices;
    const QModelIndexList& indexes = view->selectionModel()->selectedIndexes();
    foreach(const QModelIndex& index, indexes) {
        persistentIndices << index;
    }

    QAbstractItemModel* model = view->model();
    foreach(const QPersistentModelIndex& index, persistentIndices) {
        QVariant v = index.data(SubscriptionStateRole);
        if (!v.isValid()) {
            continue;
        }
        SubscriptionState state = v.value<SubscriptionState>();
        if(view == mChangeView) {
            if(state == NewSubscription || state == ExistingSubscription) {
                state = NewUnsubscription;
            } else if(state == NewUnsubscription) {
                state = NewSubscription;
            } else {
                continue;
            }
        } else if(view == mGroupsView) {
            if(state == ExistingSubscription) {
                state = NewUnsubscription;
            } else if(state == NoStateChange) {
                state = NewSubscription;
            } else {
                continue;
            }
        } else {
            continue;
        }

        model->setData(index, QVariant::fromValue(state), SubscriptionStateRole);
    }
}

void BaseDialog::slotSelectionChange()
{
    mAddChangeButton->setEnabled(mGroupsView->selectionModel()->hasSelection());
    mRevertChangeButton->setEnabled(mChangeView->selectionModel()->hasSelection());
}

}
}
