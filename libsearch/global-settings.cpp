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
#include <QApplication>
#include <QPalette>
#include "global-settings.h"

static GlobalSettings *global_instance_of_global_settings = nullptr;

GlobalSettings *GlobalSettings::getInstance()
{
    if (!global_instance_of_global_settings) {
        global_instance_of_global_settings = new GlobalSettings;
    }
    return global_instance_of_global_settings;
}

GlobalSettings::GlobalSettings(QObject *parent) : QObject(parent)
{
    m_settings = new QSettings("org.ukui/ukui-search", "ukui-search", this);
//    m_settings->setAtomicSyncRequired(false);
    m_block_dirs_settings = new QSettings("org.ukui/ukui-search","ukui-search-block-dirs",this);
    m_block_dirs_settings->setIniCodec(QTextCodec::codecForName("UTF-8"));
    this->forceSync();
    //the default number of transparency in mainwindow is 0.7
    //if someone changes the num in mainwindow, here should be modified too
    m_cache.insert(TRANSPARENCY_KEY, 0.7);
    if (QGSettings::isSchemaInstalled(CONTROL_CENTER_PERSONALISE_GSETTINGS_ID)) {
        m_gsettings = new QGSettings(CONTROL_CENTER_PERSONALISE_GSETTINGS_ID, QByteArray(), this);
        connect(m_gsettings, &QGSettings::changed, this, [=](const QString& key) {
            if (key == TRANSPARENCY_KEY) {
                m_cache.remove(TRANSPARENCY_KEY);
                m_cache.insert(TRANSPARENCY_KEY, m_gsettings->get(TRANSPARENCY_KEY).toDouble());
                qApp->paletteChanged(qApp->palette());
            }
        });
        m_cache.remove(TRANSPARENCY_KEY);
        m_cache.insert(TRANSPARENCY_KEY, m_gsettings->get(TRANSPARENCY_KEY).toDouble());
    }
}

const QVariant GlobalSettings::getValue(const QString &key)
{
    return m_cache.value(key);
}

bool GlobalSettings::isExist(const QString &key)
{
    return !m_cache.value(key).isNull();
}

void GlobalSettings::reset(const QString &key)
{
    m_cache.remove(key);
    QtConcurrent::run([=]() {
//        if (m_mutex.tryLock(1000)) {
            m_settings->remove(key);
            m_settings->sync();
//            m_mutex.unlock();
//        }
    });
    Q_EMIT this->valueChanged(key);
}

void GlobalSettings::resetAll()
{
    QStringList tmp = m_cache.keys();
    m_cache.clear();
    for (auto key : tmp) {
        Q_EMIT this->valueChanged(key);
    }
    QtConcurrent::run([=]() {
        if (m_mutex.tryLock(1000)) {
            m_settings->clear();
            m_settings->sync();
            m_mutex.unlock();
        }
    });
}

bool GlobalSettings::setBlockDirs(const QString &path, int &returnCode, bool remove)
{
    if(remove)
    {
        if(path.isEmpty())
        {
            returnCode = PATH_EMPTY;
            return false;
        }

        m_block_dirs_settings->remove(path);
        return true;
    }
    if(!path.startsWith("/home"))
    {
//        returnCode = QString(tr("I can only search your user directory, it doesn't make any sense if you block an directory which is not in user directory!"));
        returnCode = PATH_NOT_IN_HOME;
        return false;
    }

    //why QSetting's key can't start with "/"??
    QString pathKey = path.right(path.length()-1);

    QStringList blockDirs = m_block_dirs_settings->allKeys();
    for(QString i:blockDirs)
    {
        if(pathKey.startsWith(i))
        {
//            returnCode = QString(tr("My parent folder has been blocked!"));
            returnCode = PATH_PARENT_BLOCKED;
            return false;
        }

        if(i.startsWith(pathKey))
            m_block_dirs_settings->remove(i);
    }
    m_block_dirs_settings->setValue(pathKey,"0");
    return true;
}

QStringList GlobalSettings::getBlockDirs()
{
    return m_block_dirs_settings->allKeys();
}

//here should be override
//MouseZhangZh
void GlobalSettings::setValue(const QString &key, const QVariant &value)
{
    //    qDebug()<<"setvalue========"<<key<<":"<<value;
    m_cache.insert(key, value);
    //     m_settings->sync();
    QtConcurrent::run([=]() {
        //        qDebug()<<m_settings->status();
        //        if (m_mutex.tryLock(1000)) {
        //        m_mutex.lock();
        m_settings->setValue(key, value);
        //            qDebug()<<"setvalue========finish!!!"<<key<<":"<<value;
        m_settings->sync();
        //            qDebug()<<"setvalue========sync!!!"<<key<<":"<<value;
        //            m_mutex.unlock();
        //        }
    });
}

void GlobalSettings::forceSync(const QString &key)
{
    m_settings->sync();
    if (key.isNull()) {
        m_cache.clear();
        for (auto key : m_settings->allKeys()) {
            m_cache.insert(key, m_settings->value(key));
        }
    } else {
        m_cache.remove(key);
        m_cache.insert(key, m_settings->value(key));
    }
}
