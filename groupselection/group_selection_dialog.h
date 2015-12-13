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

#ifndef KNODE_GROUPSELECTION_SELECTIONDIALOG_H
#define KNODE_GROUPSELECTION_SELECTIONDIALOG_H

#include "dialog_base.h"

namespace KNode {
namespace GroupSelection {

/**
 * Dialog to select groups. Used by the composer.
 */
class SelectionDialog : public BaseDialog
{
    Q_OBJECT

    public:
        SelectionDialog(QWidget* parent, KNNntpAccount::Ptr account);
        ~SelectionDialog();

        /**
         * Set the list of name of initially selected groups.
         */
        void setPreselectedGroups(const QStringList& groups);
        /**
         * Returns the current group selections.
         */
        QStringList selectedGroups() const;

    protected:
        virtual void setupDialog(QCheckBox* newOnly, QCheckBox* treeView, QCheckBox* subscribedOnly);
        virtual QList<KNGroupInfo>* receiveList(KNGroupListData::Ptr data);
        virtual QAbstractProxyModel* changesGroupingModel();

    private:
        QStringList mPreselected;
};

}
}

#endif
