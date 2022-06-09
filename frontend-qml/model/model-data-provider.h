//
// Created by hxf on 22-6-8.
//

#ifndef UKUI_SEARCH_MODEL_DATA_PROVIDER_H
#define UKUI_SEARCH_MODEL_DATA_PROVIDER_H

#include <QObject>

#include "search-plugin-iface.h"

namespace UkuiSearch {

class ProviderRepeater : public QObject
{
    Q_OBJECT
public:
    explicit ProviderRepeater(QObject *parent = nullptr) : QObject(parent) {};

Q_SIGNALS:
    void sendData(const SearchPluginIface::ResultInfo&, const QString&);
    void resetKeywords();
};

class ModelDataProvider
{
public:
    explicit ModelDataProvider(QObject *parent = nullptr) {
        m_repeater = new ProviderRepeater(parent);
    }

    virtual ~ModelDataProvider() = default;

    //提供器的唯一ID
    virtual const QString id() = 0;
    virtual void startSearch(const QString &keyword) = 0;
    virtual void stopSearch() = 0;

    inline void sendData(const SearchPluginIface::ResultInfo& data, const QString& pluginId) const {
        Q_EMIT m_repeater->sendData(data, pluginId);
    }

    inline void resetKeywords() const {
        Q_EMIT m_repeater->resetKeywords();
    }

    inline const ProviderRepeater* getRepeater() const {
        return m_repeater;
    }

private:
    ProviderRepeater *m_repeater = nullptr;
};

}

#endif //UKUI_SEARCH_MODEL_DATA_PROVIDER_H
