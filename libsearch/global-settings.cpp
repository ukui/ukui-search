/*
 * Copyright (C) 2020, KylinSoft Co., Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: zhangzihao <zhangzihao@kylinos.cn>
 * Modified by: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#include <QtConcurrent>
#include <QPalette>
#include "global-settings.h"

using namespace Zeeker;
static GlobalSettings *global_instance_of_global_settings = nullptr;

GlobalSettings *GlobalSettings::getInstance() {
    if (!global_instance_of_global_settings) {
        global_instance_of_global_settings = new GlobalSettings;
    }
    return global_instance_of_global_settings;
}

GlobalSettings::GlobalSettings(QObject *parent) : QObject(parent) {
    m_settings = new QSettings(MAIN_SETTINGS, QSettings::IniFormat, this);
//    m_settings->setAtomicSyncRequired(false);
    m_block_dirs_settings = new QSettings(BLOCK_DIRS, QSettings::IniFormat, this);
    m_block_dirs_settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    m_block_dirs_settings->setValue("These_are_block_dirs_conf_for_ukui_search","0");
    m_block_dirs_settings->sync();
    m_confWatcher = new QFileSystemWatcher(this);
    m_confWatcher->addPath(BLOCK_DIRS);
    connect(m_confWatcher, &QFileSystemWatcher::fileChanged, this, [ & ]() {
        m_block_dirs_settings->sync();
        m_confWatcher->addPath(BLOCK_DIRS);
    });

    m_search_record_settings = new QSettings(SEARCH_HISTORY, QSettings::IniFormat, this);
    m_search_record_settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    for(QString i : m_search_record_settings->allKeys()) {
        m_history.append(QUrl::fromPercentEncoding(i.toLocal8Bit()));
    }
    if (!QDBusConnection::sessionBus().connect("org.kylinssoclient.dbus",
            "/org/kylinssoclient/path",
            "org.freedesktop.kylinssoclient.interface",
            "keyChanged",
            this, SLOT(updateSearchHistory(QString))))

        qWarning() << "Kylinssoclient Dbus connect fail!";

    this->forceSync();
    //the default number of transparency in mainwindow is 0.7
    //if someone changes the num in mainwindow, here should be modified too
    m_cache.insert(TRANSPARENCY_KEY, 0.7);
    if (QGSettings::isSchemaInstalled(CONTROL_CENTER_PERSONALISE_GSETTINGS_ID)) {
        m_trans_gsettings = new QGSettings(CONTROL_CENTER_PERSONALISE_GSETTINGS_ID, QByteArray(), this);
        connect(m_trans_gsettings, &QGSettings::changed, this, [ = ](const QString & key) {
            if (key == TRANSPARENCY_KEY) {
                m_cache.remove(TRANSPARENCY_KEY);
                m_cache.insert(TRANSPARENCY_KEY, m_trans_gsettings->get(TRANSPARENCY_KEY).toDouble());
                qApp->paletteChanged(qApp->palette());
            }
        });
        m_cache.remove(TRANSPARENCY_KEY);
        m_cache.insert(TRANSPARENCY_KEY, m_trans_gsettings->get(TRANSPARENCY_KEY).toDouble());
    }

    m_cache.insert(STYLE_NAME_KEY, "ukui-light");
    m_cache.insert(FONT_SIZE_KEY, 11);
    if (QGSettings::isSchemaInstalled(THEME_GSETTINGS_ID)) {
        m_theme_gsettings = new QGSettings(THEME_GSETTINGS_ID, QByteArray(), this);
        connect(m_theme_gsettings, &QGSettings::changed, this, [ = ](const QString & key) {
            if (key == STYLE_NAME_KEY) {
                //当前主题改变时也发出paletteChanged信号，通知主界面刷新
                m_cache.remove(STYLE_NAME_KEY);
                m_cache.insert(STYLE_NAME_KEY, m_theme_gsettings->get(STYLE_NAME_KEY).toString());
                qApp->paletteChanged(qApp->palette());
            } else if (key == FONT_SIZE_KEY) {
                m_cache.remove(FONT_SIZE_KEY);
                m_cache.insert(FONT_SIZE_KEY, m_theme_gsettings->get(FONT_SIZE_KEY).toDouble());
                qApp->paletteChanged(qApp->palette());
            } else if (key == ICON_THEME_KEY) {
                qApp->paletteChanged(qApp->palette());
            }
        });
        m_cache.remove(STYLE_NAME_KEY);
        m_cache.insert(STYLE_NAME_KEY, m_theme_gsettings->get(STYLE_NAME_KEY).toString());
        m_cache.remove(FONT_SIZE_KEY);
        m_cache.insert(FONT_SIZE_KEY, m_theme_gsettings->get(FONT_SIZE_KEY).toDouble());
    }
}

const QVariant GlobalSettings::getValue(const QString &key) {
    return m_cache.value(key);
}

bool GlobalSettings::isExist(const QString &key) {
    return !m_cache.value(key).isNull();
}

void GlobalSettings::reset(const QString &key) {
    m_cache.remove(key);
    QtConcurrent::run([ = ]() {
//        if (m_mutex.tryLock(1000)) {
        m_settings->remove(key);
        m_settings->sync();
//            m_mutex.unlock();
//        }
    });
    Q_EMIT this->valueChanged(key);
}

void GlobalSettings::resetAll() {
    QStringList tmp = m_cache.keys();
    m_cache.clear();
    for(auto key : tmp) {
        Q_EMIT this->valueChanged(key);
    }
    QtConcurrent::run([ = ]() {
        if (m_mutex.tryLock(1000)) {
            m_settings->clear();
            m_settings->sync();
            m_mutex.unlock();
        }
    });
}

bool GlobalSettings::setBlockDirs(const QString &path, int &returnCode, bool remove) {
    if (remove) {
        if (path.isEmpty()) {
            returnCode = PATH_EMPTY;
            return false;
        }

        m_block_dirs_settings->remove(path);
        return true;
    }
//    if (!path.startsWith("/home")) {
//        returnCode = QString(tr("I can only search your user directory, it doesn't make any sense if you block an directory which is not in user directory!"));
//        returnCode = PATH_NOT_IN_HOME;
//        return false;
//    }

    //why QSetting's key can't start with "/"??
    QString pathKey = path.right(path.length() - 1);

    if (pathKey.endsWith(QLatin1Char('/'))) {
        pathKey = pathKey.mid(0, pathKey.length() - 1);
    }

    QStringList blockDirs = m_block_dirs_settings->allKeys();
    for(QString i : blockDirs) {
        if (FileUtils::isOrUnder(pathKey, i)) {
//            returnCode = QString(tr("My parent folder has been blocked!"));
            returnCode = PATH_PARENT_BLOCKED;
            return false;
        }

        if (FileUtils::isOrUnder(i, pathKey))
            m_block_dirs_settings->remove(i);
    }
    m_block_dirs_settings->setValue(pathKey, "0");
    return true;
}

QStringList GlobalSettings::getBlockDirs() {
    return m_block_dirs_settings->allKeys();
}

//void GlobalSettings::appendCloudData(const QString &key, const QString &value)
//{
//    QSettings * m_qSettings = new QSettings(CLOUD_FILE, QSettings::IniFormat);
//    m_qSettings->beginGroup(key);
//    QStringList values = m_qSettings->value(key).toStringList();
//    m_qSettings->endGroup();
//    if (values.contains(value)) {
//        values.removeOne(value);
//    }
//    values.insert(0,value);

//    m_qSettings->beginGroup(key);
//    m_qSettings->setValue(key, values);
//    m_qSettings->endGroup();
//    if (m_qSettings) {
//        delete m_qSettings;
//        m_qSettings = NULL;
//    }
//}

void GlobalSettings::setSearchRecord(const QString &word, const QDateTime &time) {
    QStringList keys = m_search_record_settings->allKeys();
    if (keys.contains(QString(QUrl::toPercentEncoding(word))))
        m_history.removeOne(word);
    m_search_record_settings->setValue(QString(QUrl::toPercentEncoding(word)), time.toString("yyyy-MM-dd hh:mm:ss"));
    if (keys.size() >= 20)
        m_search_record_settings->remove(QString(QUrl::toPercentEncoding(m_history.takeFirst())));
    m_history.append(word);
}

QStringList GlobalSettings::getSearchRecord() {
    return m_history;
}

//bool GlobalSettings::removeOneCloudData(const QString &key, const QString &value)
//{
//    if (!QFileInfo(CLOUD_FILE).isFile()) return false;
//    QSettings * m_qSettings = new QSettings(CLOUD_FILE, QSettings::IniFormat);
//    m_qSettings->beginGroup(key);
//    QStringList values = m_qSettings->value(key).toStringList();
//    m_qSettings->endGroup();
//    if (values.contains(value)) {
//        values.removeOne(value);
//    } else return false;
//    m_qSettings->beginGroup(key);
//    m_qSettings->setValue(key, values);
//    m_qSettings->endGroup();
//    if (m_qSettings) {
//        delete m_qSettings;
//        m_qSettings = NULL;
//    }
//    return true;
//}

//bool GlobalSettings::removeAllCloudData(const QString &key)
//{
//    if (!QFileInfo(CLOUD_FILE).isFile()) return false;
//    QSettings * m_qSettings = new QSettings(CLOUD_FILE, QSettings::IniFormat);
//    m_qSettings->beginGroup(key);
//    m_qSettings->beginGroup(key);
//    m_qSettings->setValue(key, QStringList());
//    m_qSettings->endGroup();
//    if (m_qSettings) {
//        delete m_qSettings;
//        m_qSettings = NULL;
//    }
//    return true;
//}

//QStringList GlobalSettings::getCloudData(const QString &key)
//{
//    if (!QFileInfo(CLOUD_FILE).isFile()) return QStringList();
//    QSettings * m_qSettings = new QSettings(CLOUD_FILE, QSettings::IniFormat);
//    m_qSettings->beginGroup(key);
//    QStringList values = m_qSettings->value(key).toStringList();
//    m_qSettings->endGroup();
//    if (m_qSettings)
//        delete m_qSettings;
//    return values;
//}

//this method is designed for main process settings only!!
void GlobalSettings::setValue(const QString &key, const QVariant &value) {
    m_cache.insert(key, value);
    QtConcurrent::run([ = ]() {
        if (m_mutex.tryLock(1000)) {
            m_settings->setValue(key, value);
            m_settings->sync();
            m_mutex.unlock();
        }
    });
}

void GlobalSettings::forceSync(const QString &key) {
    m_settings->sync();
    if (key.isNull()) {
        m_cache.clear();
        for(auto key : m_settings->allKeys()) {
            m_cache.insert(key, m_settings->value(key));
        }
    } else {
        m_cache.remove(key);
        m_cache.insert(key, m_settings->value(key));
    }
}

void GlobalSettings::updateSearchHistory(QString key) {
    if (key == "search") {
        m_search_record_settings->sync();
        m_history.clear();
        for(QString i : m_search_record_settings->allKeys()) {
            m_history.append(QUrl::fromPercentEncoding(i.toLocal8Bit()));
        }
    }
}
