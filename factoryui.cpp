#include "factoryui.h"

std::unique_ptr<AbstractUI> FactoryUI::create(SourceType type)
{
    if (type == SourceType::File)
    {
        return std::make_unique<FromFileUI>();
    }
    else if (type == SourceType::Camera)
    {
       return std::make_unique<FromCamUI>();
    }
    return nullptr;
}
