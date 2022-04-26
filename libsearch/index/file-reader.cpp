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
void FileReader::getTextContent(QString path, QString &textContent, QString &suffix) {
    QFileInfo file(path);
    suffix =  file.suffix();

    if (suffix == "docx") {
        FileUtils::getDocxTextContent(path, textContent);
    } else if (suffix == "pptx") {
        FileUtils::getPptxTextContent(path, textContent);
    } else if (suffix == "xlsx") {
        FileUtils::getXlsxTextContent(path, textContent);
    } else if (strsfx == "txt" or strsfx == "html") {
        FileUtils::getTxtContent(path, textContent);
    } else if (suffix == "doc" || suffix == "dot" || suffix == "wps" || suffix == "ppt" ||
            suffix == "pps" || suffix == "dps" || suffix == "et" || suffix == "xls") {
        KBinaryParser searchdata;
        searchdata.RunParser(path, textContent);
    } else if (suffix == "pdf") {
        FileUtils::getPdfTextContent(path, textContent);
    } else if (true == targetPhotographTypeMap[suffix]){
        OcrObject::getInstance()->getTxtContent(path, textContent);
    }
    return;
}
