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
#include "document.h"
#include <QDebug>
using namespace Zeeker;
void Document::setData(QString &data) {
    if (data.isEmpty())
        return;
    m_document.set_data(data.toStdString());
}

void Document::addPosting(std::string term, QVector<size_t> offset, int weight) {
    if (term == "")
        return;
    if (term.length() > 240)
        term = QString::fromStdString(term).left(30).toStdString();

    for(size_t i : offset) {
        m_document.add_posting(term, i, weight);
    }
}

void Document::addPosting(std::string &term, std::vector<size_t> &offset, int weight) {
    if (term == "")
        return;
    if (term.length() > 240)
        term = QString::fromStdString(term).left(30).toStdString();

    for(size_t i : offset) {
        m_document.add_posting(term, i, weight);
    }
}

void Document::addPosting(std::string term, unsigned int offset, int weight) {
    if (term == "")
        return;
    if (term.length() > 240)
        term = QString::fromStdString(term).left(30).toStdString();

    m_document.add_posting(term, offset, weight);
}

void Document::addTerm(QString term) {
    if (term.isEmpty())
        return;
    m_document.add_term(term.toStdString());
}

void Document::addTerm(std::string term) {
    if (term.empty())
        return;
    m_document.add_term(term);
}

void Document::addValue(QString value) {
    m_document.add_value(1, value.toStdString());
}

void Document::setUniqueTerm(QString term) {
    if (term.isEmpty())
        return;
    m_document.add_term(term.toStdString());

//    m_unique_term = new QString(term);
    m_unique_term = std::move(term.toStdString());
}

void Document::setUniqueTerm(std::string term) {
    if (term.empty())
        return;
    m_document.add_term(term);
    m_unique_term = term;
}

std::string Document::getUniqueTerm() {
//    qDebug()<<"m_unique_term!"<<*m_unique_term;
//    qDebug() << QString::fromStdString(m_unique_term.toStdString());
    return m_unique_term;//.toStdString();
}

void Document::setIndexText(QStringList indexText) {
//    QStringList indexTextList = indexText;
//    m_index_text = new QStringList(indexText);
    m_index_text = std::move(indexText);
}

QStringList Document::getIndexText() {
    return m_index_text;
}

Xapian::Document Document::getXapianDocument() {
    return m_document;
}

void Document::reuireDeleted()
{
    m_shouldDelete = true;
}

bool Document::isRequiredDeleted()
{
    return m_shouldDelete;
}
