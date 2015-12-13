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

#include "group_selection_dialog.h"

#include <KDE/KMessageBox>

#include "model/selection_grouping_proxy_model.h"
#include "model/subscription_state_proxy_model.h"

#include "utilities.h"

namespace KNode {
namespace GroupSelection {

SelectionDialog::SelectionDialog(QWidget* parent, KNNntpAccount::Ptr account)
    : BaseDialog(parent, account),
      mPreselected()
{
}

SelectionDialog::~SelectionDialog()
{
    KNHelper::saveWindowSize("groupSelDlg", this->size());
}


void SelectionDialog::setupDialog(QCheckBox* newOnly, QCheckBox* treeView, QCheckBox* subscribedOnly)
{
    setCaption(i18nc("@title:window", "Select Destinations"));
    setButtons(Ok | Cancel);

    newOnly->setVisible(false);
    newOnly->setChecked(false);
    subscribedOnly->setVisible(true);
    subscribedOnly->setChecked(true);
    treeView->setVisible(true);
    treeView->setChecked(false);

    KNHelper::restoreWindowSize("groupSelDlg", this, QSize(659,364));
}


void SelectionDialog::setPreselectedGroups(const QStringList& groups)
{
    mPreselected = groups;
}

QStringList SelectionDialog::selectedGroups() const
{
    QStringList selection;
    bool moderated = false;

    selection.append(mPreselected);
    Q_FOREACH(const KNGroupInfo gi, subscriptionModel()->subscribed()) {
        moderated |= (gi.status == KNGroup::moderated);
        selection.append(gi.name);
    }
    Q_FOREACH(const KNGroupInfo& gi, subscriptionModel()->unsubscribed()) {
        selection.removeAll(gi.name);
    }

    if (moderated && selection.size() > 1) {
        KMessageBox::information(parentWidget(),
                                 i18n("You are crossposting to a moderated newsgroup.\nPlease be aware that your article will not appear in any group\nuntil it has been approved by the moderators of the moderated group."),
                                 QString(), "crosspostModeratedWarning");
    }


    return selection;
}


QAbstractProxyModel* SelectionDialog::changesGroupingModel()
{
    return new SelectionGroupingProxyModel(this);
}


QList<KNGroupInfo>* SelectionDialog::receiveList(KNGroupListData::Ptr data)
{
    subscriptionModel()->setOriginalSubscriptions(mPreselected);
    return data->extractList();
}


}
}
