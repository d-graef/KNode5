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

#include "selection_grouping_proxy_model.h"

#include <KI18n/KLocalizedString>

#include "../enums.h"

namespace KNode {
namespace GroupSelection {

SelectionGroupingProxyModel::SelectionGroupingProxyModel(QObject* parent)
    : BaseGroupingProxyModel(parent),
      mSelection()
{
    QList<QVector<QPersistentModelIndex>*> groupings;
    groupings << &mSelection;
    setGroupings(groupings);
}

SelectionGroupingProxyModel::~SelectionGroupingProxyModel()
{
}

const QString SelectionGroupingProxyModel::title(QVector< QPersistentModelIndex >* grouping) const
{
    if(grouping == &mSelection) {
        return i18nc("@title", "Groups for this article");
    }
    return QString();
}


QVector<QPersistentModelIndex>* SelectionGroupingProxyModel::selectInternalGrouping(SubscriptionState state)
{
    switch(state) {
    case ExistingSubscription:
    case NewSubscription:
        return &mSelection;
        break;
    case NewUnsubscription:
    case NoStateChange:
    case Invalid:
        break;
    }
    return 0;
}


}
}
