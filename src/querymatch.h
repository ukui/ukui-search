/*
 *   Copyright 2006-2007 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef PLASMA_QUERYMATCH_H
#define PLASMA_QUERYMATCH_H

#include <QList>
#include <QUrl>
#include <QSharedDataPointer>

#include "krunner_export.h"

class QAction;
class QIcon;
class QString;
class QVariant;
class QWidget;

namespace Plasma
{

class RunnerContext;
class AbstractRunner;
class QueryMatchPrivate;

/**
 * @class QueryMatch querymatch.h <KRunner/QueryMatch>
 *
 * @short A match returned by an AbstractRunner in response to a given
 * RunnerContext.
 */
class KRUNNER_EXPORT QueryMatch
{
    public:
        /**
         * The type of match. Value is important here as it is used for sorting
         */
        enum Type {              //是否为模糊搜索？
            NoMatch = 0,         /**< Null match  空匹配*/
            CompletionMatch = 10, /**< Possible completion for the data of the query  查询的数据可能完成*/
            PossibleMatch = 30,   /**< Something that may match the query  可能与查询匹配的内容*/
            InformationalMatch = 50, /**< A purely informational, non-actionable match,
                                       such as the answer to a question or calculation
                                       纯粹的信息性的、不可操作的匹配，如对问题或计算的回答*/
            HelperMatch = 70, /**< A match that represents an action not directly related
                                 to activating the given search term, such as a search
                                 in an external tool or a command learning trigger. Helper
                                 matches tend to be generic to the query and should not
                                 be autoactivated just because the user hits "Enter"
                                 while typing. They must be explicitly selected to
                                 be activated, but unlike InformationalMatch cause
                                 an action to be triggered.
                                表示不直接相关操作的匹配项
                                激活激活给定的搜索词，如搜索
                                在外部工具或命令学习触发器中。帮手
                                匹配往往是查询的泛型，而不应该
                                因为用户点击“回车”就自动激活
                                打字的时候。必须明确选择它们来
                                被激活，但不同于信息匹配原因
                                要触发的操作*/
            ExactMatch = 100 /**< An exact match to the query 与查询完全匹配 */
        };

        /**
         * Constructs a PossibleMatch associated with a given RunnerContext
         * and runner.
         * 构造与给定RunnerContext关联的PossibleMatch
         *
         * @param runner the runner this match belongs to
         */
        explicit QueryMatch(AbstractRunner *runner = nullptr);

        /**
         * Copy constructor
         */
        QueryMatch(const QueryMatch &other);

        ~QueryMatch();
        QueryMatch &operator=(const QueryMatch &other);
        bool operator==(const QueryMatch &other) const;
        bool operator!=(const QueryMatch &other) const;
        bool operator<(const QueryMatch &other) const;


        /**
         * @return the runner associated with this action
         * 与此操作关联的运行程序
         */
        AbstractRunner *runner() const;

        /**
         * Requests this match to activae using the given context
         *请求使用给定上下文激活此匹配项
         * @param context the context to use in conjunction with this run
         *上下文与此运行一起使用的上下文
         * @sa AbstractRunner::run
         */
        void run(const RunnerContext &context) const;

        /**
         * @return true if the match is valid and can therefore be run,
         *         an invalid match does not have an associated AbstractRunner
         * 如果匹配有效并且因此可以运行，则为true，无效匹配没有关联的AbstractRunner
         */
        bool isValid() const;

        /**
         * Sets the type of match this action represents.
         * 设置此操作表示的匹配类型
         */
        void setType(Type type);

        /**
         * The type of action this is. Defaults to PossibleMatch.
         * 动作就是这样。默认为PossibleMatch
         */
        Type type() const;

        /**
         * Sets information about the type of the match which can
         * be used to categorize the match.
         *
         * This string should be translated as it can be displayed
         * in an UI
         * 设置有关可用于分类匹配的匹配类型的信息。此字符串应转换为可以显示在UI中
         */
        void setMatchCategory(const QString& category);

        /**
         * Extra information about the match which can be used
         * to categorize the type.
         *
         * By default this returns the internal name of the runner
         * which returned this result
         */
        QString matchCategory() const;

        /**
         * Sets the relevance of this action for the search
         * it was created for.
         *
         * @param relevance a number between 0 and 1.
         */
        void setRelevance(qreal relevance);

        /**
         * The relevance of this action to the search. By default,
         * the relevance is 1.
         *
         * @return a number between 0 and 1
         */
        qreal relevance() const;

        /**
         * Sets data to be used internally by the associated
         * AbstractRunner.
         *
         * When set, it is also used to form
         * part of the id() for this match. If that is inappropriate
         * as an id, the runner may generate its own id and set that
         * with setId(const QString&) directly after calling setData
         */
        void setData(const QVariant &data);

        /**
         * @return the data associated with this match; usually runner-specific
         */
        QVariant data() const;

        /**
         * Sets the id for this match; useful if the id does not
         * match data().toString(). The id must be unique to all
         * matches from this runner, and should remain constant
         * for the same query for best results.
         *
         * @param id the new identifying string to use to refer
         *           to this entry
         */
        void setId(const QString &id);

        /**
         * @return a string that can be used as an ID for this match,
         * even between different queries. It is based in part
         * on the source of the match (the AbstractRunner) and
         * distinguishing information provided by the runner,
         * ensuring global uniqueness as well as consistency
         * between query matches.
         */
        QString id() const;

        /**
         * Sets the main title text for this match; should be short
         * enough to fit nicely on one line in a user interface
         *
         * @param text the text to use as the title
         */
        void setText(const QString &text);

        /**
         * @return the title text for this match
         */
        QString text() const;

        /**
         * Sets the descriptive text for this match; can be longer
         * than the main title text
         *
         * @param text the text to use as the description
         */
        void setSubtext(const QString &text);

        /**
         * @return the descriptive text for this match
         */
        QString subtext() const;

        /**
         * Sets the icon associated with this match
         *
         * Prefer using setIconName.
         *
         * @param icon the icon to show along with the match
         */
        void setIcon(const QIcon &icon);

        /**
         * @return the icon for this match
         */
        QIcon icon() const;

        /**
         * Sets the icon name associated with this match
         *
         * @param icon the name of the icon to show along with the match
         * @since 5.24
         */
        void setIconName(const QString &iconName);

        /**
         * @return the name of the icon for this match
         * @since 5.24
         */
        QString iconName() const;

        /**
         * Sets the MimeType, if any, associated with this match.
         * This overrides the MimeType provided by QueryContext, and should only be
         * set when it is different from the QueryContext MimeType
         */
        void setMimeType(const QString &mimeType);

        /**
         * @return the mimtype for this match, or QString() is none
         */
        QString mimeType() const;

        /**
         * Sets the urls, if any, associated with this match
         */
        void setUrls(const QList<QUrl> &urls);

        /**
         * @return the mimtype for this match, or QString() is none
         */
        QList<QUrl> urls() const;

        /**
         * Sets whether or not this match can be activited
         *
         * @param enable true if the match is enabled and therefore runnable
         */
        void setEnabled(bool enable);

        /**
         * @return true if the match is enabled and therefore runnable, otherwise false
         */
        bool isEnabled() const;

        /**
         * The current action.
         */
        QAction* selectedAction() const;

        /**
         * Sets the selected action
         */
        void setSelectedAction(QAction *action);

        /**
         * @return true if this match can be configured before being run
         * @since 4.3
         */
        bool hasConfigurationInterface() const;

        /**
         * If hasConfigurationInterface() returns true, this method may be called to get
         * a widget displaying the options the user can interact with to modify
         * the behaviour of what happens when the match is run.
         *
         * @param widget the parent of the options widgets.
         * @since 4.3
         */
        void createConfigurationInterface(QWidget *parent);

    private:
        QSharedDataPointer<QueryMatchPrivate> d;
};

}

#endif
