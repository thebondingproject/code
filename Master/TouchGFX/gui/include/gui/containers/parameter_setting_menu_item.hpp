#ifndef PARAMETER_SETTING_MENU_ITEM_HPP
#define PARAMETER_SETTING_MENU_ITEM_HPP

#include <gui_generated/containers/parameter_setting_menu_itemBase.hpp>

class parameter_setting_menu_item : public parameter_setting_menu_itemBase
{
public:
    parameter_setting_menu_item();
    virtual ~parameter_setting_menu_item() {}

    virtual void initialize();
protected:
};

#endif // PARAMETER_SETTING_MENU_ITEM_HPP
