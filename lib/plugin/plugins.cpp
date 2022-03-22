#include "plugins.h"
#include "debug_oz.h"
#include "setting.h"

params_t make_plugin_Parameters(JsonObject list)
{
    params_t Parameters;
    for(auto item : list)
    {
        String key = item.key().c_str();
        String value = item.value();
        Parameters[key] = value;
    }
    return Parameters;
}

std::vector<plugin_base *> plugins_get_all()
{
    std::vector<plugin_base *> ret;

    auto plugin_definition = Database.get_plugin();

    for (JsonObject item : plugin_definition)
    {
        uint16_t key = item["key"];
        if(plugin_list.find(key) != plugin_list.end() )
        {
            plugin_base * newPlugin = plugin_list[key].ctor(make_plugin_Parameters(item));
            if(newPlugin->is_initialized())
            {
                OZ_LOGV("PLUG","Add new plugin key: %u",key);
                ret.push_back(newPlugin);
            }
        }
    }

    return ret;
}
