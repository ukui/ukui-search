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
#include "file-reader.h"
#include "file-utils.h"
#include "binary-parser.h"
using namespace Zeeker;
FileReader::FileReader(QObject *parent) : QObject(parent) {

}

void FileReader::getTextContent(QString path, QString &textContent) {
    QMimeType type = FileUtils::getMimetype(path);
    QString name = type.name();
    QFileInfo file(path);
    QString strsfx =  file.suffix();
    if(name == "application/zip") {
        if(strsfx.endsWith("docx"))
            FileUtils::getDocxTextContent(path, textContent);
        if(strsfx.endsWith("pptx"))
            FileUtils::getPptxTextContent(path, textContent);
        if(strsfx.endsWith("xlsx"))
            FileUtils::getXlsxTextContent(path, textContent);
    } else if(name == "text/plain") {
        if(strsfx.endsWith("txt"))
            FileUtils::getTxtContent(path, textContent);
    } else if(type.inherits("application/msword") || type.name() == "application/x-ole-storage") {
        if(strsfx.endsWith("doc") || strsfx.endsWith("dot") || strsfx.endsWith("wps") || strsfx.endsWith("ppt") ||
                strsfx.endsWith("pps") || strsfx.endsWith("dps") || strsfx.endsWith("et") || strsfx.endsWith("xls")) {
            KBinaryParser searchdata;
            searchdata.RunParser(path, textContent);
        }
    } else if(name == "application/pdf") {
        if(strsfx.endsWith("pdf"))
            FileUtils::getPdfTextContent(path, textContent);
    } else {
        qWarning() << "Unsupport format:[" << path << "][" << type.name() << "]";
    }

    return;
}
