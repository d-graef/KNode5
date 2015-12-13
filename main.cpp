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

#include <KDBusAddons/KDBusService>
#include <KI18n/KLocalizedString>
#include <QCommandLineParser>

#include "aboutdata.h"
#include "knapplication.h"
#include "utils/startup.h"


int main(int argc, char* argv[])
{
    KNApplication app(argc, argv);

    KNode::AboutData aboutData;
    KAboutData::setApplicationData(aboutData);

    KDBusService service(KDBusService::Unique);
    // If this point is reached, this is the only running instance


    KLocalizedString::setApplicationDomain(aboutData.componentName().toAscii());

    KNode::Utilities::Startup s;
    s.loadLibrariesIconsAndTranslations();
    s.updateDataAndConfiguration();

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addPositionalArgument("url", i18n("A 'news://server/group' URL"), "[url]");
    // Process the actual command line arguments given by the user
    parser.process(app);


    app.launch(parser.positionalArguments());

    return app.exec();
}
