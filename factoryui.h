#ifndef FACTORYUI_H
#define FACTORYUI_H

#include <abstractui.h>
#include <fromcamui.h>
#include <fromfileui.h>

enum class SourceType
{
    File,
    Camera
};

class FactoryUI
{
public:
    FactoryUI();
    static std::unique_ptr<AbstractUI> create(SourceType type)
    {
        switch (type)
        {
        case SourceType::File:
            return std::make_unique<FromFileUI>();

        case SourceType::Camera:
            return std::make_unique<FromFileUI>();
        }

    }

};

#endif // FACTORYUI_H
