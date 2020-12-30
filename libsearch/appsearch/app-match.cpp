#include "app-match.h"
#include <glib.h>
#include "file-utils.h"
AppMatch::AppMatch(QObject *parent) : QObject(parent)
{
    this->getDesktopFilePath();
}

QStringList AppMatch::startMatchApp(QString input){
    input.replace(" ","");
    m_sourceText=input;
    m_returnResult.clear();
    if(input.isEmpty()){
        return m_returnResult;
    }
    this->getAppName();
    m_returnResult=m_midResult;
    m_midResult.clear();
    return m_returnResult;
}

/**
 * @brief AppMatch::getAllDesktopFilePath 遍历所有desktop文件
 * @param path 存放desktop文件夹
 */
void AppMatch::getAllDesktopFilePath(QString path){

    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QDir dir(path);
    if (!dir.exists()) {
        return;
    }
    dir.setFilter(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    list.removeAll(QFileInfo("/usr/share/applications/screensavers"));
    if(list.size()< 1 ) {
        return;
    }
    int i=0;

    //递归算法的核心部分
    do{
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool isDir = fileInfo.isDir();
        if(isDir) {
            getAllDesktopFilePath(fileInfo.filePath());
        }
        else{
            //过滤LXQt、KDE
            QString filePathStr=fileInfo.filePath();
            if(filePathStr.contains("KDE",Qt::CaseInsensitive)||
               filePathStr.contains("mate",Qt::CaseInsensitive)||
               filePathStr.contains("LX",Qt::CaseInsensitive) ){
                i++;
                continue;
            }
            //过滤后缀不是.desktop的文件
            if(!filePathStr.endsWith(".desktop"))
            {
                i++;
                continue;
            }
            QByteArray fpbyte=filePathStr.toLocal8Bit();
            char* filepath=fpbyte.data();
            g_key_file_load_from_file(keyfile,filepath,flags,nullptr);
            char* ret_1=g_key_file_get_locale_string(keyfile,"Desktop Entry","NoDisplay", nullptr, nullptr);
            if(ret_1!=nullptr)
            {
                QString str=QString::fromLocal8Bit(ret_1);
                if(str.contains("true"))
                {
                    i++;
                    continue;
                }
            }
            char* ret_2=g_key_file_get_locale_string(keyfile,"Desktop Entry","NotShowIn", nullptr, nullptr);
            if(ret_2!=nullptr)
            {
                QString str=QString::fromLocal8Bit(ret_2);
                if(str.contains("UKUI"))
                {
                    i++;
                    continue;
                }
            }
            //过滤中英文名为空的情况
            QLocale cn;
            QString language=cn.languageToString(cn.language());
            if(QString::compare(language,"Chinese")==0)
            {
                char* nameCh=g_key_file_get_string(keyfile,"Desktop Entry","Name[zh_CN]", nullptr);
                char* nameEn=g_key_file_get_string(keyfile,"Desktop Entry","Name", nullptr);
                if(QString::fromLocal8Bit(nameCh).isEmpty() && QString::fromLocal8Bit(nameEn).isEmpty())
                {
                    i++;
                    continue;
                }
            }
            else {
                char* name=g_key_file_get_string(keyfile,"Desktop Entry","Name", nullptr);
                if(QString::fromLocal8Bit(name).isEmpty())
                {
                    i++;
                    continue;
                }
            }

            m_filePathList.append(filePathStr);
        }
        i++;

    } while(i < list.size());
    g_key_file_free(keyfile);
}

/**
 * @brief AppMatch::getDesktopFilePath
 * 对遍历结果处理
 */
void AppMatch::getDesktopFilePath()
{
    m_filePathList.clear();
    getAllDesktopFilePath("/usr/share/applications/");
    m_filePathList.removeAll("/usr/share/applications/software-properties-livepatch.desktop");
    m_filePathList.removeAll("/usr/share/applications/mate-color-select.desktop");
    m_filePathList.removeAll("/usr/share/applications/blueman-adapters.desktop");
    m_filePathList.removeAll("/usr/share/applications/blueman-manager.desktop");
    m_filePathList.removeAll("/usr/share/applications/mate-user-guide.desktop");
    m_filePathList.removeAll("/usr/share/applications/nm-connection-editor.desktop");
    m_filePathList.removeAll("/usr/share/applications/debian-uxterm.desktop");
    m_filePathList.removeAll("/usr/share/applications/debian-xterm.desktop");
    m_filePathList.removeAll("/usr/share/applications/im-config.desktop");
    m_filePathList.removeAll("/usr/share/applications/fcitx.desktop");
    m_filePathList.removeAll("/usr/share/applications/fcitx-configtool.desktop");
    m_filePathList.removeAll("/usr/share/applications/onboard-settings.desktop");
    m_filePathList.removeAll("/usr/share/applications/info.desktop");
    m_filePathList.removeAll("/usr/share/applications/ukui-power-preferences.desktop");
    m_filePathList.removeAll("/usr/share/applications/ukui-power-statistics.desktop");
    m_filePathList.removeAll("/usr/share/applications/software-properties-drivers.desktop");
    m_filePathList.removeAll("/usr/share/applications/software-properties-gtk.desktop");
    m_filePathList.removeAll("/usr/share/applications/gnome-session-properties.desktop");
    m_filePathList.removeAll("/usr/share/applications/org.gnome.font-viewer.desktop");
    m_filePathList.removeAll("/usr/share/applications/xdiagnose.desktop");
    m_filePathList.removeAll("/usr/share/applications/gnome-language-selector.desktop");
    m_filePathList.removeAll("/usr/share/applications/mate-notification-properties.desktop");
    m_filePathList.removeAll("/usr/share/applications/transmission-gtk.desktop");
    m_filePathList.removeAll("/usr/share/applications/mpv.desktop");
    m_filePathList.removeAll("/usr/share/applications/system-config-printer.desktop");
    m_filePathList.removeAll("/usr/share/applications/org.gnome.DejaDup.desktop");
    m_filePathList.removeAll("/usr/share/applications/yelp.desktop");
    m_filePathList.removeAll("/usr/share/applications/peony-computer.desktop");
    m_filePathList.removeAll("/usr/share/applications/peony-home.desktop");
    m_filePathList.removeAll("/usr/share/applications/peony-trash.desktop");
    m_filePathList.removeAll("/usr/share/applications/peony.desktop");

    //v10
    m_filePathList.removeAll("/usr/share/applications/mate-about.desktop");
    m_filePathList.removeAll("/usr/share/applications/time.desktop");
    m_filePathList.removeAll("/usr/share/applications/network.desktop");
    m_filePathList.removeAll("/usr/share/applications/shares.desktop");
    m_filePathList.removeAll("/usr/share/applications/mate-power-statistics.desktop");
    m_filePathList.removeAll("/usr/share/applications/display-im6.desktop");
    m_filePathList.removeAll("/usr/share/applications/display-im6.q16.desktop");
    m_filePathList.removeAll("/usr/share/applications/openjdk-8-policytool.desktop");
    m_filePathList.removeAll("/usr/share/applications/kylin-io-monitor.desktop");
    m_filePathList.removeAll("/usr/share/applications/wps-office-uninstall.desktop");
    m_filePathList.removeAll("/usr/share/applications/wps-office-misc.desktop");
}

/**
 * @brief AppMatch::getAppName
 * 获取应用名字
 */
void AppMatch::getAppName()
{
    GKeyFileFlags flags=G_KEY_FILE_NONE;
    GKeyFile* keyfile=g_key_file_new ();

    QByteArray fpbyte;
    QString str;
    char* filepath;
    char* name;
    QString namestr;
    for(int i=0;i<m_filePathList.size();i++){
        str=m_filePathList.at(i);
        fpbyte=str.toLocal8Bit();
        filepath=fpbyte.data();
        g_key_file_load_from_file(keyfile,filepath,flags,nullptr);
        name=g_key_file_get_locale_string(keyfile,"Desktop Entry","Name", nullptr, nullptr);
        namestr=QString::fromLocal8Bit(name);
        appNameMatch(namestr,str);
    }

    g_key_file_load_from_file(keyfile,filepath,flags,nullptr);
    g_key_file_free(keyfile);
}

/**
 * @brief AppMatch::appNameMatch
 * 进行匹配
 * @param appname
 * 应用名字
 * @param desktoppath
 * desktop路径
 */
void AppMatch::appNameMatch(QString appname,QString desktoppath){
    if(appname.contains(m_sourceText)){
        m_midResult.append(desktoppath);
        return;
    }
    QString pinyin=FileUtils::findMultiToneWords(appname).at(0);// 中文转拼音
    if(pinyin.contains(m_sourceText,Qt::CaseInsensitive)){
        m_midResult.append(desktoppath);
        return;
    }
    QString shouzimu=FileUtils::findMultiToneWords(appname).at(1);// 中文转首字母
    if(shouzimu.contains(m_sourceText,Qt::CaseInsensitive)){
        m_midResult.append(desktoppath);
    }
}
