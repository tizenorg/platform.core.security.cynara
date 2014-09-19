/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/**
 * @file        src/service/plugin/PluginManager.cpp
 * @author      Zofia Abramowska <z.abramowska@samsung.com>
 * @version     1.0
 * @brief       Definition of PluginManager class
 */

#define _BSD_SOURCE_
#include <log/log.h>
#include <plugin/PluginManager.h>

#include <cstdlib>
#include <dirent.h>
#include <dlfcn.h>
#include <functional>

namespace {
    int plugin_filter(const struct dirent *ent) {
#ifdef _DIRENT_HAVE_D_TYPE
        if (ent->d_type != DT_REG) {
            return 0;
        }
#endif
        if (ent->d_name[0] == '.') {
            return 0;
        }
        return 1;
    }
}

namespace Cynara {

PluginManager::PluginManager(const std::string &pluginDir) : m_dir(pluginDir) {
    loadPlugins();
}

PluginManager::~PluginManager() {
    for (auto plugin : m_pluginLibs) {
        dlclose(plugin);
    }
}

ExternalPluginPtr PluginManager::getPlugin(PolicyType pType) {
    return m_plugins[pType];
}

void PluginManager::loadPlugins(void) {
    struct dirent **nameList;
    int fileAmount = scandir(m_dir.c_str(), &nameList, plugin_filter, alphasort);

    if (fileAmount < 0) {
        auto error = strerror(errno);
        LOGE("Couldn't scan for plugins in <%s> : <%s>", m_dir.c_str(), error);
    }

    std::unique_ptr<dirent*, std::function<void(dirent**)>> direntPtr(nameList,
            [fileAmount](dirent** dirs) {
                for (int i = 0; i < fileAmount; i++) {
                    free(dirs[i]);
                }
                free(dirs);
                    });
    for (int i = 0; i < fileAmount; i++) {
        openPlugin(m_dir + nameList[i]->d_name);
    }
}

void PluginManager::openPlugin(const std::string &path) {
    void *handle = dlopen(path.c_str(), RTLD_LAZY);

    if (!handle) {
        LOGW("File could not be dlopened <%s> : <%s>", path.c_str(), dlerror());
        return;
    }
    std::unique_ptr<void, std::function<void (void*)>> handlePtr(handle, std::ptr_fun(dlclose));

    //Flush any previous errors
    dlerror();
    createPlugin func = reinterpret_cast<createPlugin>(dlsym(handle, "create"));

    char* error;
    if ((error = dlerror()) != NULL) {
        LOGE("Couldn't resolve symbol <createPlugin> from lib <%s> : <%s>", path.c_str(), error);
        return;
    }

    ExternalPluginPtr pluginPtr(func());

    PolicyTypes policies = pluginPtr->getSupportedPolicyTypes();
    if (policies.empty()) {
        LOGE("Plugin <%s> does not support any type!", path.c_str());
        return;
    }
    for (auto type : policies) {
        m_plugins[type] = pluginPtr;
    }

    m_pluginLibs.push_back(handlePtr.release());
}

}


