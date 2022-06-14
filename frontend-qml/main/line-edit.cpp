//
// Created by hxf on 22-5-20.
//

#include "line-edit.h"
#include "model-data-provider.h"

#include <QQmlError>
#include <QQuickItem>

using namespace UkuiSearch;

LineEdit::LineEdit() : QQuickView()
{
    initUI();
    initConnections();
}

void LineEdit::initUI()
{
    setMinimumHeight(50);
    setMinimumWidth(700);
    setHeight(50);
    setWidth(700);

    //KWindowSystem::setState(this->winId(),NET::SkipTaskbar | NET::SkipPager | NET::SkipSwitcher );

    setResizeMode(ResizeMode::SizeRootObjectToView);
    setSource(QUrl("qrc:/qml/LineEdit.qml"));
}

void LineEdit::initConnections()
{
    connect(this->rootObject(), SIGNAL(textChanged(QString)),
            this, SLOT(onTextChanged(QString)), Qt::QueuedConnection);
}

void LineEdit::onTextChanged(const QString &keyword)
{
    qDebug() << "qt received" << keyword;
    QHash<QString, ModelDataProvider*>::const_iterator iterator = m_providers.constBegin();
    while (iterator != m_providers.constEnd()) {
        (*iterator)->startSearch(keyword);
        ++iterator;
    }
}

void LineEdit::installDataProvider(ModelDataProvider *provider)
{
    if (provider && !m_providers.contains(provider->id())) {
        m_providers.insert(provider->id(), provider);
    }
}
