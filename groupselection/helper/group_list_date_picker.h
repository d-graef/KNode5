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

#ifndef KNODE_GROUPSELECTION_GROUPLISTDATEPICKER_H
#define KNODE_GROUPSELECTION_GROUPLISTDATEPICKER_H

#include "ui_group_list_date_picker.h"

#include "knnntpaccount.h"

namespace KNode {
namespace GroupSelection {

class GroupListDatePicker : public KDialog, private Ui_NewGroupListDatePicker
{
    Q_OBJECT

    public:
        GroupListDatePicker(QWidget* parent, KNNntpAccount::Ptr account);
        ~GroupListDatePicker();

        const QDate& selectedDate() const;

    private Q_SLOTS:
        void resetDatePicker();

    private:
        QDate mLastFetchDate;
};

}
}

#endif
