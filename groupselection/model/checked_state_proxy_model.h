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

#ifndef KNODE_GROUPSELECTION_CHECKEDCONVERTIONPROXYMODEL_H
#define KNODE_GROUPSELECTION_CHECKEDCONVERTIONPROXYMODEL_H

#include <QSortFilterProxyModel>

namespace KNode {
namespace GroupSelection {

/**
 * This proxy converts checked state to/from SubscriptionStateProxyModel::StateChange.
 * This proxy must be placed between the view and the SubscriptionStateModel.
 */
class CheckedStateConvertionProxyModel : public QSortFilterProxyModel
{
    public:
        CheckedStateConvertionProxyModel(QObject* parent);
        virtual ~CheckedStateConvertionProxyModel();

        /**
         * @reimp
         * Add checkable flag.
         */
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;

        /**
         * Reimplemented to handle the checkable role.
         */
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        /**
         * Reimplemented to intercept (un)checking of items to call setData() with the
         * proper SubscriptionState change.
         */
        virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
};

}
}

#endif
