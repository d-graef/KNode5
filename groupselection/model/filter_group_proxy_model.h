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

#ifndef KNODE_GROUPSELECTION_GROUPFILTERPROXYMODEL_H
#define KNODE_GROUPSELECTION_GROUPFILTERPROXYMODEL_H

#include <KDE/KRecursiveFilterProxyModel>

namespace KNode {
namespace GroupSelection {

/**
 * Filter group having the 'new' and/or 'subscribed' flag set.
 */
class GroupFilterProxyModel : public KRecursiveFilterProxyModel
{
    Q_OBJECT

    public:
        GroupFilterProxyModel(QObject* parent = 0);
        virtual ~GroupFilterProxyModel();

        bool isNewOnlyEnabled() const
        {
            return mFilterNew;
        }
        bool isSubscribedOnlyEnabled() const
        {
            return mFilterSubscribed;
        }

    public Q_SLOTS:
        void filterNew(bool enable);
        void filterSubscribed(bool enable);

    protected:
        virtual bool acceptRow(int sourceRow, const QModelIndex& sourceParent) const;

    private:
        bool mFilterNew;
        bool mFilterSubscribed;
};

}
}

#endif
