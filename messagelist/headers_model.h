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


#ifndef KNODE_HEADERS_MODEL_H
#define KNODE_HEADERS_MODEL_H

#include <QtCore/QAbstractItemModel>

#include <kmime/kmime_dateformatter.h>

#include "knarticle.h"
#include "knarticlecollection.h"

class QRegExp;
class KNArticleFilter;

namespace KNode
{
namespace MessageList
{

class Header;

class HeadersModel : public QAbstractItemModel
{
    Q_OBJECT

    public:
        /**
         * Column index.
         */
        enum HeaderColumnIndex
        {
            COLUMN_SUBJECT = 0,
            COLUMN_FROM,
            COLUMN_DATE,

            COLUMN_COUNT
        };

        /**
         * Custom role for the #data() method.
         */
        enum HeadersRole
        {
            InvalidRole = Qt::UserRole,
            ArticleRole,                 ///< The Article::Ptr.
            ReadRole,                    ///< Indicate if the article is read (bool).
            SortRole,                    ///< The data to sort the model on.
        };

    public:
        explicit HeadersModel(QObject* parent = 0);
        virtual ~HeadersModel();

        /**
         * Change the group.
         * @param group The new group.
         */
        void setCollection(const KNArticleCollection::Ptr collection);
        /**
         * Change the filter.
         */
        void setFilter(KNArticleFilter* filter);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
        virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
        virtual QModelIndex parent(const QModelIndex& child) const;

        void setSortedByThreadChangeDate(bool b) { mSortByThreadChangeDate = b; };
        bool sortedByThreadChangeDate() { return mSortByThreadChangeDate; }

    public Q_SLOTS:
        /**
         * Show/hide threading.
         */
        void showThreads(bool b);
        /**
         * Receive notification of article changes.
         */
        void changedArticles(const KNArticle::List articles, bool deleted);

    private:
        /**
         * Reload the internal data structure.
         */
        void reload(KNArticleCollection::Ptr collection);
        void modifyInternal(KNArticle::List &articles, Header* parent, bool deletion);
        void refreshInternal(Header *hdr, int row = -1);

        Header* mRoot;
        KNArticleCollection::Ptr mCollection;
        KNArticleFilter* mFilter;
        KMime::DateFormatter mDateFormatter;
        QRegExp* mNormlizeSubject;
        bool mSortByThreadChangeDate;
};


}
}


#endif
