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

#ifndef KNODE_GROUPSELECTION_ENUMS_H
#define KNODE_GROUPSELECTION_ENUMS_H

#include <QtCore/qnamespace.h>

namespace KNode {
namespace GroupSelection {

    /**
     * Specific role for QAbstractItemModel::data().
     */
    enum ItemDataRole
    {
        /**
         * Associated to the SubscriptionState.
         */
        SubscriptionStateRole = Qt::UserRole,
        /**
         * Role to get the KNGroupInfo.
         */
        GroupInfoRole,
    };

    /**
     * Used in the model to indicate subscription changes.
     */
    enum SubscriptionState
    {
        Invalid = 0,             ///< Default constructed value
        ExistingSubscription,    ///< The group was already subscribed to.
        NewSubscription,         ///< New subscription request.
        NewUnsubscription,       ///< Unsubscription requested.
        NoStateChange,           ///< None of the aboves.
    };

    /**
     * Column index of the GroupModel.
     */
    enum GroupModelColumnIndex
    {
        GroupModelColumn_Name = 0,
        GroupModelColumn_Description,

        GroupModelColumn_Count,
    };

}
}

Q_DECLARE_METATYPE(KNode::GroupSelection::SubscriptionState);

#endif
