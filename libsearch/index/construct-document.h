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
 * Authors: zhangpengfei <zhangpengfei@kylinos.cn>
 *
 */
#ifndef CONSTRUCTDOCUMENT_H
#define CONSTRUCTDOCUMENT_H

#include <QRunnable>
#include <QVector>
//#include <QMetaObject>
#include "document.h"
#include "index-generator.h"

namespace UkuiSearch {

class IndexGenerator;
class ConstructDocumentForPath : public QRunnable {
public:
    explicit ConstructDocumentForPath(QVector<QString> list);
    ~ConstructDocumentForPath() = default;
protected:
    void run();
private:
    QVector<QString> m_list;
};

class ConstructDocumentForContent : public QRunnable {
public:
    explicit ConstructDocumentForContent(QString path);
    ~ConstructDocumentForContent() = default;
protected:
    void run();
private:
    QString m_path;
};
}

#endif // CONSTRUCTDOCUMENT_H
