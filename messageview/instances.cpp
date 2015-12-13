/*
    KNode, the KDE newsreader
    Copyright (c) 2005-2006 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, US
*/

#include "instances.h"

#include "article_widget.h"

namespace KNode {
namespace MessageView {

QList<ArticleWidget*> Instances::mInstances;


void Instances::configChanged()
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    aw->readConfig();
    aw->updateContents();
  }
}

bool Instances::articleVisible( KNArticle::Ptr article )
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    if ( aw->article() == article )
      return true;
  }
  return false;
}

void Instances::articleRemoved( KNArticle::Ptr article )
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    if ( aw->article() == article )
      aw->setArticle( KNArticle::Ptr() );
  }
}

void Instances::articleChanged( KNArticle::Ptr article )
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    if ( aw->article() == article )
      aw->updateContents();
  }
}

void Instances::articleLoadError( KNArticle::Ptr article, const QString &error )
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    if ( aw->article() == article )
      aw->displayErrorMessage( error );
  }
}

void Instances::collectionRemoved( KNArticleCollection::Ptr coll )
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    if ( aw->article() && aw->article()->collection() == coll )
      aw->setArticle( KNArticle::Ptr() );
  }
}

void Instances::cleanup()
{
  Q_FOREACH(ArticleWidget* aw, mInstances) {
    aw->setArticle( KNArticle::Ptr() ); //delete orphant articles => avoid crash in destructor
  }
}


}
}
