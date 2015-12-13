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

#include "group_subscription_dialog.h"

#include <KDE/KMessageBox>

#include "helper/group_list_date_picker.h"
#include "model/subscription_grouping_proxy_model.h"
#include "model/subscription_state_proxy_model.h"

#include "utilities.h"

namespace KNode {
namespace GroupSelection {

SubscriptionDialog::SubscriptionDialog(QWidget* parent, KNNntpAccount::Ptr account)
    : BaseDialog(parent, account)
{
}

SubscriptionDialog::~SubscriptionDialog()
{
    KNHelper::saveWindowSize("groupDlg", this->size());
}

void SubscriptionDialog::toSubscribe(QList<KNGroupInfo>& list)
{
    list << subscriptionModel()->subscribed();

    Q_FOREACH(const KNGroupInfo& gi, list) {
        if(gi.status == KNGroup::moderated) {
            KMessageBox::information(parentWidget(),
                                     i18nc("@info", "You have subscribed to a moderated newsgroup.\n"
                                                    "Your articles will not appear in the group immediately.\n"
                                                    "They have to go through a moderation process."),
                                     QString(), "subscribeModeratedWarning");
            break;
        }
    }
}

void SubscriptionDialog::toUnsubscribe(QStringList& list)
{
    Q_FOREACH(const KNGroupInfo& gi, subscriptionModel()->unsubscribed()) {
        list << gi.name;
    }
}


void SubscriptionDialog::setupDialog(QCheckBox* newOnly, QCheckBox* treeView, QCheckBox* subscribedOnly)
{
    setCaption(i18nc("@title:window", "Subscribe to Newsgroups"));

    setButtons(Ok | Cancel | Help | User1 | User2);
    setHelp("anc-fetch-group-list");
    setButtonText(User1, i18nc("@action:button Fetch the list of groups from the server", "New &List"));
    connect(this, SIGNAL(user1Clicked()), this, SLOT(slotRequestNewList()));
    setButtonText(User2, i18nc("@action:button Fetch the list of groups from the server", "New &Groups..."));
    connect(this, SIGNAL(user2Clicked()), this, SLOT(slotRequestGroupSince()));

    newOnly->setVisible(true);
    newOnly->setChecked(false);
    subscribedOnly->setVisible(false);
    subscribedOnly->setVisible(false);
    treeView->setVisible(true);
    treeView->setChecked(true);

    KNHelper::restoreWindowSize("groupDlg", this, QSize(662,393));
}


QAbstractProxyModel* SubscriptionDialog::changesGroupingModel()
{
    return new SubscriptionGroupingProxyModel(this);
}


QList<KNGroupInfo>* SubscriptionDialog::receiveList(KNGroupListData::Ptr data)
{
    QStringList subscribed;
    QList<KNGroupInfo>* groups = 0;

    if(data) {
        groups = data->extractList();

        Q_FOREACH(const KNGroupInfo& gi, *groups) {
            if(gi.subscribed) {
                subscribed << gi.name;
            }
        }
    }
    subscriptionModel()->setOriginalSubscriptions(subscribed);

    enableButton(User1, true);
    enableButton(User2, true);

    return groups;
}


void SubscriptionDialog::slotRequestNewList()
{
    enableButton(User1, false);
    enableButton(User2, false);
    emit fetchList(account());
}

void SubscriptionDialog::slotRequestGroupSince()
{
    QPointer<GroupListDatePicker> diag = new GroupListDatePicker(this, account());
    if(diag->exec() == QDialog::Accepted) {
        enableButton(User1, false);
        enableButton(User2, false);
        emit checkNew(account(), diag->selectedDate());
    }
    delete diag;
}


}
}
