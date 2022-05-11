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
    } else if (strsfx == "txt" or strsfx == "html") {
        FileUtils::getTxtContent(path, textContent);
    } else if (strsfx == "doc" || strsfx == "dot" || strsfx == "wps" || strsfx == "ppt" ||
            strsfx == "pps" || strsfx == "dps" || strsfx == "et" || strsfx == "xls") {
        KBinaryParser searchdata;
        searchdata.RunParser(path, textContent);
    } else if (strsfx == "pdf") {
        FileUtils::getPdfTextContent(path, textContent);
    } else if (strsfx == "uof") {
        QString mimeName = FileUtils::getMimetype(path).name();
        if (mimeName == "application/xml" || mimeName == "application/uof") {
            FileUtils::getUOFTextContent(path, textContent);

        } else if (mimeName == "application/x-ole-storage") {
            //uof的ppt文档不支持修改母版。一旦进行这些操作，uof文档可能会被wps存为doc文件
            KBinaryParser searchdata;
            searchdata.RunParser(path, textContent);
        }

    } else if (strsfx == "uot" || strsfx == "uos") {
        FileUtils::getUOF2TextContent(path, textContent);

    } else if (strsfx == "uop") {
        FileUtils::getUOF2PPTContent(path, textContent);

    } else if (strsfx == "ofd") {
        FileUtils::getOFDTextContent(path, textContent);

    } else if (strsfx == "png" || strsfx == "jpg" || strsfx == "jpeg"){
        OcrObject::getInstance()->getTxtContent(path, textContent);;
    }
    return;
}
