#include "imgui.h"

#include "settings.h"
#include "widgets.h"

// settings widgets

bool Setting<bool>::Render()
{
   std::string label = m_name + "_label";
   std::string desc = m_name + "_desc";

   bool ret = ImGui::Checkbox(_(label.c_str()), &m_value);
   Widgets::Tooltip(_(desc.c_str()));

   return ret;
}