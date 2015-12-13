/*
    KNode, the KDE newsreader
    Copyright (c) 2005 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#ifndef KNODE_MESSAGEVIEW_INSTANCES_H
#define KNODE_MESSAGEVIEW_INSTANCES_H

#include "knarticle.h"
#include "knarticlecollection.h"

namespace KNode {
namespace MessageView {

class ArticleWidget;

/**
 * Helper to manages instances of ArticleWidget.
 */
class Instances
{
  friend class ArticleWidget;

  public:
      // TODO: déclarer le constructeur privé

    /// notify all instances about a config change
    static void configChanged();
    /** check whether the given article is displayed in any instance
     * @param article The article to check.
     */
    static bool articleVisible( KNArticle::Ptr article );
    /** notify all instances that the given article has been removed
     * @param article The removed article.
     */
    static void articleRemoved( KNArticle::Ptr article );
    /** notify all instances that the given article has changed
     * @param article The changed article.
     */
    static void articleChanged( KNArticle::Ptr article );
    /** notify all instances about an error during loading the given article
     * @param article The article that couldn't be loaded.
     * @param error The error message.
     */
    static void articleLoadError( KNArticle::Ptr article, const QString &error );
    /** notify all instances that the given collection has been removed
     * @param coll The removed article collection (a group or a folder).
     */
    static void collectionRemoved( KNArticleCollection::Ptr coll );
    /// cleanup all instances
    static void cleanup();

  private:
    /// list of all instances of this class
    static QList<ArticleWidget*> mInstances;
};

}
}

#endif
