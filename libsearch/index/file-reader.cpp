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
#include "ocrobject.h"
using namespace UkuiSearch;
FileReader::FileReader(QObject *parent) : QObject(parent) {

}
void FileReader::getTextContent(QString path, QString &textContent) {
    QFileInfo file(path);
    QString strsfx =  file.suffix();
    if (strsfx == "docx") {
        FileUtils::getDocxTextContent(path, textContent);
    } else if (strsfx == "pptx") {
        FileUtils::getPptxTextContent(path, textContent);
    } else if (strsfx == "xlsx") {
        FileUtils::getXlsxTextContent(path, textContent);
    } else if (strsfx == "txt") {
        FileUtils::getTxtContent(path, textContent);
    } else if (strsfx == "doc" || strsfx == "dot" || strsfx == "wps" || strsfx == "ppt" ||
            strsfx == "pps" || strsfx == "dps" || strsfx == "et" || strsfx == "xls") {
        KBinaryParser searchdata;
        searchdata.RunParser(path, textContent);
    } else if (strsfx == "pdf") {
        FileUtils::getPdfTextContent(path, textContent);
    } else if (strsfx == "png" || strsfx == "jpg" || strsfx == "jpeg"){
        OcrObject::getInstance()->getTxtContent(path, textContent);;
    }
    return;
}
