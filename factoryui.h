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
    static std::unique_ptr<AbstractUI> create(SourceType type);

};

#endif // FACTORYUI_H
