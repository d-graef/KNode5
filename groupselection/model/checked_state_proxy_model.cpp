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

#include "checked_state_proxy_model.h"

#include "../enums.h"

namespace KNode {
namespace GroupSelection {

CheckedStateConvertionProxyModel::CheckedStateConvertionProxyModel(QObject* parent)
  : QSortFilterProxyModel(parent)
{
}

CheckedStateConvertionProxyModel::~CheckedStateConvertionProxyModel()
{
}



Qt::ItemFlags CheckedStateConvertionProxyModel::flags(const QModelIndex& index) const
{
    if(index.column() == GroupModelColumn_Name) {
        return QSortFilterProxyModel::flags(index) | Qt::ItemIsUserCheckable;
    }
    return QSortFilterProxyModel::flags(index);
}

QVariant CheckedStateConvertionProxyModel::data(const QModelIndex& index, int role) const
{
    if(role == Qt::CheckStateRole && index.column() == GroupModelColumn_Name) {
        QVariant v = index.data(SubscriptionStateRole);
        if(v.isValid()) {
            SubscriptionState state = v.value<SubscriptionState>();
            if(state == NewSubscription || state == ExistingSubscription) {
                return Qt::Checked;
            }
            return Qt::Unchecked;
        }
    }

    return QAbstractProxyModel::data(index, role);
}

bool CheckedStateConvertionProxyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if(role == Qt::CheckStateRole) {
        SubscriptionState newState = (value.toUInt() == Qt::Checked ? NewSubscription : NewUnsubscription);
        return QSortFilterProxyModel::setData(index, QVariant::fromValue(newState), SubscriptionStateRole);
    }

    return QSortFilterProxyModel::setData(index, value, role);
}

}
}
