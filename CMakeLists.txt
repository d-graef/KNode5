project(knode)

cmake_minimum_required(VERSION 2.8.12)

set(LIBRARY_NAMELINK LIBRARY NAMELINK_SKIP)

# we need some parts of the ECM CMake helpers
find_package(ECM 5.16.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
# include(ECMOptionalAddSubdirectory)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(ECMAddTests)
include(ECMMarkNonGuiExecutable)
include(GenerateExportHeader)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)
include(FeatureSummary)
include(CheckFunctionExists)
include(ECMGeneratePriFile)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)

# KDEPIM_VERSION
# Version scheme: "x.y.z build".
#
# x is the version number.
# y is the major release number.
# z is the minor release number.
#
# "x.y.z" follow the kdelibs version kdepim is released with.
#
# If "z" is 0, the version is "x.y"
#
# KDEPIM_DEV_VERSION is empty for final versions.
# For development versions "build" is something like "pre", "alpha1", "alpha2", "beta1", "beta2", "rc1", "rc2".
#
# Examples in chronological order:
# 3.0, 3.0.1, 3.1 alpha1, 3.1 beta1, 3.1 beta2, 3.1 rc1, 3.1, 3.1.1, 3.2 pre, 3.2 alpha1

# Do NOT add quote
set(KDEPIM_DEV_VERSION pre)

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
  set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()

set(KDEPIM_VERSION_NUMBER "5.1.42")
set(KDEPIM_VERSION "${KDEPIM_VERSION_NUMBER}${KDEPIM_DEV_VERSION}")


set(KDEPIMLIBS_LIB_VERSION "5.1.42")
set(KCONTACTS_LIB_VERSION "4.92.0")
# set(KCALENDARCORE_LIB_VERSION "4.81.0")
set(IDENTITYMANAGEMENT_LIB_VERSION "4.82.0")
# set(KLDAP_LIB_VERSION "4.80.0")
set(KMAILTRANSPORT_LIB_VERSION "4.78.0")
# set(CALENDARUTILS_LIB_VERSION "4.80.0")
# set(KHOLIDAYS_LIB_VERSION "4.80.0")
# set(KTNEF_LIB_VERSION "4.78.0")
# set(KIMAP_LIB_VERSION "4.78.0")
# set(KMBOX_LIB_VERSION "4.78.0")
# set(AKONADICALENDAR_LIB_VERSION "4.80.0")
# set(SYNDICATION_LIB_VERSION "4.78.0")
# set(GPGMEPP_LIB_VERSION "4.80.0")
set(KONTACTINTERFACE_LIB_VERSION "4.82.0")
# set(AKONADIKALARM_LIB_VERSION "4.78.0")
set(KMIME_LIB_VERSION "4.87.0")
set(XMLRPCCLIENT_LIB_VERSION "4.78.0")
# set(KBLOG_LIB_VERSION "4.80.0")
set(KPIMTEXTEDIT_LIB_VERSION "5.1.42")

set(KDEPIM_LIB_VERSION "${KDEPIM_VERSION_NUMBER}")
set(KDEPIM_LIB_SOVERSION "5")
# set(AKONADINOTES_LIB_VERSION "4.78.0")

set(QT_REQUIRED_VERSION "5.4.0")
set(KF5_VERSION "5.16.0")

find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Test WebKitWidgets Sql Concurrent UiTools OpenGL)
find_package(Qt5X11Extras NO_MODULE)
find_package(Grantlee5 "5.0" CONFIG REQUIRED)

find_package(Qt5Designer NO_MODULE)
set_package_properties(Qt5Designer PROPERTIES
   PURPOSE "Required to build the Qt Designer plugins"
   TYPE OPTIONAL
)

# find_package(Gpgme REQUIRED)


# Find KF5 package
find_package(KF5KDELibs4Support ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Wallet ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5NewStuff ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KCMUtils ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5WebKit ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Archive ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5NotifyConfig ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Config ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5KHtml ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Service ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Auth ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools REQUIRED)
find_package(Phonon4Qt5 CONFIG REQUIRED)
find_package(KF5DNSSD ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5TextEditor ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Sonnet ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5GlobalAccel ${KF5_VERSION} CONFIG REQUIRED)
find_package(KF5Codecs ${KF5_VERSION} CONFIG REQUIRED)

# Find KdepimLibs Package
find_package(KF5PimTextEdit ${KPIMTEXTEDIT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Akonadi ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KCONTACTS_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5CalendarCore ${KCALENDARCORE_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiContact ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5Ldap ${KLDAP_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5AkonadiMime ${KDEPIMLIBS_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5CalendarUtils ${CALENDARUTILS_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5Holidays ${KHOLIDAYS_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5Tnef ${KTNEF_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5IMAP ${KIMAP_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5Mbox ${KMBOX_LIB_VERSION} CONFIG REQUIRED)

# find_package(KF5AkonadiCalendar ${AKONADICALENDAR_LIB_VERSION} CONFIG REQUIRED)

# find_package(KF5Syndication ${SYNDICATION_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5Gpgmepp ${GPGMEPP_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5KontactInterface ${KONTACTINTERFACE_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5AlarmCalendar ${AKONADIKALARM_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5Mime ${KMIME_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5XmlRpcClient ${XMLRPCCLIENT_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5Blog ${KBLOG_LIB_VERSION} CONFIG REQUIRED)
# find_package(KF5AkonadiNotes ${AKONADINOTES_LIB_VERSION} CONFIG REQUIRED)


# set(LIBGRAVATAR_VERSION_LIB "5.1.41")
# set(LIBMAILIMPORTER_VERSION_LIB "5.1.41")
set(MAILCOMMON_LIB_VERSION_LIB "5.1.41")
set(KDEPIM_APPS_LIB_VERSION_LIB "5.1.41")
set(MESSAGELIB_LIB_VERSION_LIB "5.1.41")
set(LIBKLEO_LIB_VERSION_LIB "5.1.41")
set(LIBGRANTLEETHEME_LIB_VERSION_LIB "5.1.41")
set(PIMCOMMON_LIB_VERSION_LIB "5.1.41")
set(LIBKDEPIM_LIB_VERSION_LIB "5.1.41")
# set(LIBINCIDENCEEDITOR_LIB_VERSION_LIB "5.1.41")
# set(CALENDARSUPPORT_LIB_VERSION_LIB "5.1.41")
# set(EVENTVIEW_LIB_VERSION_LIB "5.1.41")
# set(KDGANTT2_LIB_VERSION_LIB "5.1.41")
# set(LIBKSIEVE_LIB_VERSION_LIB "5.1.41")
# find_package(KF5Gravatar ${LIBGRAVATAR_VERSION_LIB} CONFIG REQUIRED)
# # find_package(KF5MailImporter ${LIBMAILIMPORTER_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5MailCommon ${MAILCOMMON_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5KaddressbookGrantlee ${KDEPIM_APPS_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5MessageViewer ${MESSAGELIB_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5Libkleo ${LIBKLEO_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5GrantleeTheme ${LIBGRANTLEETHEME_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5PimCommon ${PIMCOMMON_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5Libkdepim ${LIBKDEPIM_LIB_VERSION_LIB} CONFIG REQUIRED)
# find_package(KF5IncidenceEditorsng ${LIBINCIDENCEEDITOR_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5MessageCore ${MESSAGELIB_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5MessageComposer ${KDEPIM_LIB_INSTALLED_VERSION} CONFIG REQUIRED)
find_package(KF5MessageList ${KDEPIM_LIB_INSTALLED_VERSION} CONFIG REQUIRED)
find_package(KF5TemplateParser ${KDEPIM_LIB_INSTALLED_VERSION} CONFIG REQUIRED)
# find_package(KF5CalendarSupport ${CALENDARSUPPORT_LIB_VERSION_LIB} CONFIG REQUIRED)
# find_package(KF5EventViews ${EVENTVIEW_LIB_VERSION_LIB} CONFIG REQUIRED)
# find_package(KF5KDGantt2 ${KDGANTT2_LIB_VERSION_LIB} CONFIG REQUIRED)
# find_package(KF5FollowupReminder ${KDEPIM_APPS_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(KF5SendLater ${KDEPIM_LIB_INSTALLED_VERSION} CONFIG REQUIRED)
find_package(KF5ComposerEditorNG ${KDEPIM_LIB_INSTALLED_VERSION} CONFIG REQUIRED)
find_package(KF5KdepimDBusInterfaces ${KDEPIM_LIB_INSTALLED_VERSION} CONFIG REQUIRED)
# find_package(KF5LibKSieve ${LIBKSIEVE_LIB_VERSION_LIB} CONFIG REQUIRED)
find_package(MailTransportDBusService CONFIG REQUIRED)


if (NOT APPLE)
    find_package(X11)
endif()

configure_file(knode-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/kdepim-version.h @ONLY)

include_directories(${knode_SOURCE_DIR} ${knode_BINARY_DIR})


add_definitions(-DQT_USE_QSTRINGBUILDER)


find_package(Boost 1.34.0)
set_package_properties(Boost PROPERTIES DESCRIPTION "Boost C++ Libraries" URL "http://www.boost.org" TYPE REQUIRED PURPOSE "Boost is required for building most KDEPIM applications")


# doc must be a subdir of kdepim or packagers will kill us
# add_subdirectory(doc)

install( FILES knode.categories DESTINATION ${KDE_INSTALL_CONFDIR} )

feature_summary(WHAT ALL
                INCLUDE_QUIET_PACKAGES
                FATAL_ON_MISSING_REQUIRED_PACKAGES
)































add_definitions( -DTRANSLATION_DOMAIN=\"knode\" )
add_definitions( -DQT_NO_URL_CAST_FROM_STRING )
# add_definitions( -DQT_NO_CAST_FROM_ASCII )
# add_definitions( -DQT_NO_CAST_TO_ASCII )

if(BUILD_TESTING)
    add_subdirectory( tests )
endif()
add_subdirectory( pics )
add_subdirectory( filters )
add_subdirectory( kconf_update )

include_directories(
  ${Boost_INCLUDE_DIR}
)


########### Private library ###############

set(knodecommon_LIB_SRCS
   knapplication.cpp
   knconfigmanager.cpp
   knconfig.cpp
   knconfigwidgets.cpp
   knode.cpp
   settings.cpp
   scheduler.cpp
   knjobdata.cpp
   mailsendjob.cpp
   nntpjobs.cpp
   knaccountmanager.cpp
   kncollection.cpp
   kncollectionviewitem.cpp
   knserverinfo.cpp
   knnntpaccount.cpp
   kngroupmanager.cpp
   knarticlecollection.cpp
   kngroup.cpp
   kngrouppropdlg.cpp
   knfoldermanager.cpp
   knfolder.cpp
   knmemorymanager.cpp
   kncleanup.cpp
   knarticlemanager.cpp
   knarticle.cpp
   kndisplayedheader.cpp
   knsourceviewwindow.cpp
   knarticlewindow.cpp
#    kncomposer.cpp
#    kncomposerview.cpp
#    kncomposereditor.cpp
   knarticlefactory.cpp
   knfiltermanager.cpp
   knstatusfilter.cpp
   knstringfilter.cpp
   knrangefilter.cpp
   knarticlefilter.cpp
   knfilterconfigwidget.cpp
   knfilterdialog.cpp
   knsearchdialog.cpp
   utilities.cpp
   knmainwidget.cpp
   aboutdata.cpp
   knglobals.cpp
   kncollectionview.cpp
   articlewidget.cpp
#    csshelper.cpp
   composer/attachment_view.cpp
   composer/followupto_combobox.cpp
   configuration/identity_widget.cpp
   configuration/identity_edition_dialog.cpp
   groupselection/dialog_base.cpp
   groupselection/group_subscription_dialog.cpp
   groupselection/group_selection_dialog.cpp
   groupselection/model/checked_state_proxy_model.cpp
   groupselection/model/filter_group_proxy_model.cpp
   groupselection/model/subscription_state_proxy_model.cpp
   groupselection/model/base_grouping_proxy_model.cpp
   groupselection/model/subscription_grouping_proxy_model.cpp
   groupselection/model/selection_grouping_proxy_model.cpp
   groupselection/model/group_model.cpp
   groupselection/helper/group_list_date_picker.cpp
   utils/locale.cpp
   utils/startup.cpp
   messagelist/headers_widget.cpp
   messagelist/headers_model.cpp
   messagelist/headers_view.cpp
   treewidget.cpp
   foldertreewidget.cpp
)
ecm_qt_declare_logging_category(knodecommon_LIB_SRCS
                                HEADER knode_debug.h
                                IDENTIFIER KNODE_LOG
                                CATEGORY_NAME log_knode)

qt5_add_dbus_adaptor( knodecommon_LIB_SRCS org.kde.knode.xml knmainwidget.h KNMainWidget)


ki18n_wrap_ui(knodecommon_LIB_SRCS
   nntpaccountdialog_base.ui
   nntpaccountlistwidget_base.ui
   readnewsgeneralwidget_base.ui
   readnewsnavigationwidget_base.ui
   readnewsviewerwidget_base.ui
   postnewstechnicalwidget_base.ui
   postnewscomposerwidget_base.ui
   composer/composer_view.ui
   configuration/identity_widget.ui
   configuration/identity_edition_dialog.ui
   groupselection/dialog_base.ui
   groupselection/helper/group_list_date_picker.ui
)

kconfig_add_kcfg_files(knodecommon_LIB_SRCS settings_base.kcfgc )

add_library(knodecommon ${knodecommon_LIB_SRCS})
# generate_export_header(knodeprivate BASE_NAME knode)

target_link_libraries(knodecommon
    PRIVATE
        KF5::IdentityManagement
        KF5::I18n
        KF5::KCMUtils
        KF5::KDELibs4Support
        KF5::KHtml
        KF5::KIOCore
        KF5::Libkdepim
        KF5::Libkleo
        KF5::MailTransport
        KF5::MessageComposer
        KF5::Mime
        KF5::SonnetUi
        KF5::XmlGui
)


set_target_properties(knodecommon
   PROPERTIES VERSION ${KDEPIM_LIB_VERSION}
   SOVERSION ${KDEPIM_LIB_SOVERSION}
)
install(TARGETS knodecommon ${INSTALL_TARGETS_DEFAULT_ARGS})


########### KCM ###############

set(kcm_knode_PART_SRCS knconfigpages.cpp )
add_library(kcm_knode MODULE ${kcm_knode_PART_SRCS})
target_link_libraries(kcm_knode
    PRIVATE
        knodecommon
        KF5::IdentityManagement
        KF5::KCMUtils
        KF5::KDELibs4Support
        KF5::Mime
)
install(TARGETS kcm_knode  DESTINATION ${KDE_INSTALL_PLUGINDIR})

########### Main binary ###############

set(knode_SRCS main.cpp )
file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-knode.png")
ecm_add_app_icon(knode_SRCS ICONS ${ICONS_SRCS})
add_executable(knode ${knode_SRCS})
target_link_libraries(knode
    PRIVATE
        knodecommon
        KF5::CoreAddons
        KF5::DBusAddons
        KF5::I18n
        KF5::KDELibs4Support
)
install(TARGETS knode  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})


########### Install other files ###############

install( PROGRAMS org.kde.knode.desktop  DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES knode_config_identity.desktop
              knode_config_accounts.desktop
              knode_config_appearance.desktop
              knode_config_read_news.desktop
              knode_config_post_news.desktop
              knode_config_privacy.desktop
              knode_config_cleanup.desktop
        DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
install(FILES knodeui.rc
              knreaderui.rc
              kncomposerui.rc
        DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/knode)
install(FILES headers.rc
        DESTINATION ${KDE_INSTALL_DATADIR}/knode)
install(FILES org.kde.knode.xml DESTINATION ${KDE_INSTALL_DBUSINTERFACEDIR} )

add_subdirectory(icons)
