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
#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <xapian.h>
#include <QString>
#include <QStringList>
#include <QVector>
namespace Zeeker {
class Document {
public:
    Document() = default;
    ~Document() {}
    Document(const Document& other) {
        m_document = other.m_document;
        m_index_text = other.m_index_text;
        m_unique_term = other.m_unique_term;
    }
    void operator=(const Document& other) {
        m_document = other.m_document;
        m_index_text = other.m_index_text;
        m_unique_term = other.m_unique_term;
    }
    void setData(QString &data);
    void addPosting(std::string term, QVector<size_t> offset, int weight = 1);
    void addPosting(std::string term, std::vector<size_t> offset, int weight = 1);
    void addPosting(std::string term, unsigned int offset, int weight = 1);
    void addTerm(QString term);
    void addValue(QString value);
    void setUniqueTerm(QString term);
    std::string getUniqueTerm();
    void setIndexText(QStringList indexText);
    QStringList getIndexText();
    Xapian::Document getXapianDocument();
private:
    Xapian::Document m_document;
    QStringList m_index_text;
    QString m_unique_term;

};
}

#endif // DOCUMENT_H
