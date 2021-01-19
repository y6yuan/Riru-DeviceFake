#pragma once

#include <map>
#include <string>
#include <unistd.h>

namespace Config {

    struct Property {

        std::string name;
        std::string value;

        Property(const char *name, const char *value) : name(name), value(value) {}
    };

    void Load();

    namespace Properties {

        Property *Find(const char *name);
    }

    namespace Builds {
        Property *Find(const char *name);
        std::map<std::string, Property *>::iterator Begin();
        std::map<std::string, Property *>::iterator End();
    }

    namespace Packages {

        bool Find(const char *name);
    }
}
