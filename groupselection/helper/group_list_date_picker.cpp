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

#include "group_list_date_picker.h"

#include <KDE/KLocale>

namespace KNode {
namespace GroupSelection {

GroupListDatePicker::GroupListDatePicker(QWidget* parent, KNNntpAccount::Ptr account)
    : KDialog(parent),
      mLastFetchDate(account->lastNewFetch())
{
    setupUi(this);
    setMainWidget(mPage);
    setCaption(i18nc("@title:window", "Check for New Groups"));
    mLastCheckButton->setText(KLocale::global()->formatDate(mLastFetchDate, KLocale::FancyLongDate));
    resetDatePicker();

    resize(sizeHint());

    connect(mLastCheckButton, SIGNAL(clicked(bool)),
            this, SLOT(resetDatePicker()));
}



GroupListDatePicker::~GroupListDatePicker()
{
}

const QDate& GroupListDatePicker::selectedDate() const
{
    return mDatePicker->date();
}

void GroupListDatePicker::resetDatePicker()
{
    mDatePicker->setDate(mLastFetchDate);
}


}
}
