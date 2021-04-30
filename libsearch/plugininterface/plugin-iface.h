#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

namespace Zeeker {

class PluginInterface
{
public:
    enum PluginType
    {
        MenuPlugin,
        PreviewPagePlugin,
        SearchPlugin,
        Other
    };

    virtual ~PluginInterface() {}
    virtual PluginType pluginType() = 0;
    virtual const QString name() = 0;
    virtual const QString description() = 0;
    virtual const QIcon icon() = 0;
    virtual void setEnable(bool enable) = 0;
    virtual bool isEnable() = 0;
};

}
#endif // PLUGININTERFACE_H
