#include <LuaIntf.h>
#include <mm2_common.h>
#include <vector>
#include <string>

#include "..\imgui.h"
#include "..\..\implot\implot.h"
#include "..\misc\cpp\imgui_stdlib.h"
#include <mm2_gfx.h>

using namespace LuaIntf;

//Helpers for us
template <class destType>
bool TableValuesToArray(LuaRef table, LuaTypeID typeId, std::vector<destType>& results)
{
    if (!table.isValid() || !table.isTable())
    {
        MM2::Errorf("TableValuesToArray got non table, or invalid input.");
        return false;
    }

    for (auto& e : table) {
        std::string key = e.key<std::string>();
        LuaRef value = e.value<LuaRef>();
        if (value.type() == typeId)
        {
            auto valueAsStr = e.value<destType>();
            results.push_back(valueAsStr);
        }
    }

    return true;
}

static const char* ProcessNullableString(const char* string) 
{
    if (strlen(string) == 0)
        return nullptr;
    return string;
}

//
static bool ImGuiShowDemoWindowLua(bool show) 
{
    ImGui::ShowDemoWindow(&show);
    return show;
}

//
static bool ImGuiBegin2Lua(const char* name, ImGuiWindowFlags flags = 0) 
{
    return ImGui::Begin(name, nullptr, flags);    
}

static std::tuple<bool, bool> ImGuiBeginLua(const char* name, bool bOpen, ImGuiWindowFlags flags = 0)
{
    bool draw =  ImGui::Begin(name, &bOpen, flags);
    return std::make_tuple(draw, bOpen);
}

static int ImGuiVSliderIntLua(const char* label, const ImVec2& size, int value, int min, int max, ImGuiSliderFlags flags)
{
    ImGui::VSliderInt(label, size, &value, min, max, "%d", flags);
    return value;
}

static float ImGuiVSliderFloatLua(const char* label, const ImVec2& size, float value, float min, float max, ImGuiSliderFlags flags)
{
    ImGui::VSliderFloat(label, size, &value, min, max, "%.3f", flags);
    return value;
}

static int ImGuiSliderIntLua(const char* label, int value, int min, int max, ImGuiSliderFlags flags)
{
    ImGui::SliderInt(label, &value, min, max, "%d", flags);
    return value;
}

static std::tuple<int, int> ImGuiSliderInt2Lua(const char* label, int x, int y, int min, int max, ImGuiSliderFlags flags)
{
    int intArray[2] = { x, y };
    ImGui::SliderInt2(label, intArray, min, max, "%d", flags);
    return std::make_tuple(intArray[0], intArray[1]);
}

static std::tuple<int, int, int> ImGuiSliderInt3Lua(const char* label, int x, int y, int z, int min, int max, ImGuiSliderFlags flags)
{
    int intArray[3] = { x, y, z };
    ImGui::SliderInt3(label, intArray, min, max, "%d", flags);
    return std::make_tuple(intArray[0], intArray[1], intArray[2]);
}

static std::tuple<int, int, int, int> ImGuiSliderInt4Lua(const char* label, int x, int y, int z, int w, int min, int max, ImGuiSliderFlags flags)
{
    int intArray[4] = { x, y, z, w};
    ImGui::SliderInt4(label, intArray, min, max, "%d", flags);
    return std::make_tuple(intArray[0], intArray[1], intArray[2], intArray[3]);
}

static float ImGuiSliderFloatLua(const char* label, float value, float min, float max, ImGuiSliderFlags flags) 
{
    ImGui::SliderFloat(label, &value, min, max, "%.3f", flags);
    return value;
}

static std::tuple<float, float> ImGuiSliderFloat2Lua(const char* label, float x, float y, float min, float max, ImGuiSliderFlags flags)
{
    float floatArray[2] = { x, y };
    ImGui::SliderFloat2(label, floatArray, min, max, "%.3f", flags);
    return std::make_tuple(floatArray[0], floatArray[1]);
}

static std::tuple<float, float, float> ImGuiSliderFloat3Lua(const char* label, float x, float y, float z, float min, float max, ImGuiSliderFlags flags)
{
    float floatArray[3] = { x, y ,z };
    ImGui::SliderFloat3(label, floatArray, min, max, "%.3f", flags);
    return std::make_tuple(floatArray[0], floatArray[1], floatArray[2]);
}

static std::tuple<float, float, float, float> ImGuiSliderFloat4Lua(const char* label, float x, float y, float z, float w, float min, float max, ImGuiSliderFlags flags)
{
    float floatArray[4] = { x, y ,z,w };
    ImGui::SliderFloat4(label, floatArray, min, max, "%.3f", flags);
    return std::make_tuple(floatArray[0], floatArray[1], floatArray[2], floatArray[3]);
}

static int ImGuiRadioButtonLua(const char* label, int selectedId, int id) 
{
    ImGui::RadioButton(label, &selectedId, id);
    return selectedId;
}

static void ImGuiPushIDLua(int id) 
{
    ImGui::PushID(id);
}

static int ImGuiComboLua(const char* label, int currentItem, LuaRef items) 
{
    std::vector<const char*> values;
    if (!TableValuesToArray<const char*>(items, LuaTypeID::STRING, values))
        return currentItem;

    ImGui::Combo(label, &currentItem, values.data(), static_cast<int>(values.size()));
    return currentItem;
}

static int ImGuiListBoxLua(const char* label, int currentItem, LuaRef items, int heightInItems) 
{
    std::vector<const char*> values;
    if (!TableValuesToArray<const char*>(items, LuaTypeID::STRING, values))
        return currentItem;
    
    ImGui::ListBox(label, &currentItem, values.data(), static_cast<int>(values.size()), heightInItems);
    return currentItem;
}

static bool ImGuiCollapsingHeaderLua(const char* label, ImGuiTreeNodeFlags flags) 
{
    return ImGui::CollapsingHeader(label, flags);
}

static bool ImGuiCheckboxLua(const char* label, bool checked)
{
    ImGui::Checkbox(label, &checked);
    return checked;
}

static bool ImGuiLeftArrowButtonLua() {
    return ImGui::ArrowButton("##left", ImGuiDir_Left);
}

static bool ImGuiRightArrowButtonLua() {
    return ImGui::ArrowButton("##right", ImGuiDir_Right);
}

static bool ImGuiUpArrowButtonLua() {
    return ImGui::ArrowButton("##up", ImGuiDir_Up);
}

static bool ImGuiDownArrowButtonLua() {
    return ImGui::ArrowButton("##down", ImGuiDir_Down);
}

static float ImGuiDragFloatLua(const char* label, float value, float speed, float min, float max, ImGuiSliderFlags flags)
{
    ImGui::DragFloat(label, &value, speed, min, max, "%.3f", flags);
    return value;
}

static int ImGuiDragIntLua(const char* label, int value, float speed, int min, int max, ImGuiSliderFlags flags)
{
    ImGui::DragInt(label, &value, speed, min, max, "%d", flags);
    return value;
}

static void ImGuiLabelTextLua(const char* label, const char* text) 
{
    ImGui::LabelText(label, text);
}

static void ImGuiTextLua(const char* text)
{
    ImGui::Text(text);
}

static void ImGuiTextWrappedLua(const char* text)
{
    ImGui::TextWrapped(text);
}

static void ImGuiTextColoredLua(const ImVec4& color, const char* text)
{
    ImGui::TextColored(color, text);
}

static void ImGuiBulletTextLua(const char* text)
{
    ImGui::BulletText(text);
}

static void ImGuiSetTooltipLua(const char* tooltip)
{
    ImGui::SetTooltip(tooltip);
}

static std::tuple<bool, bool> ImGuiSelectableLua(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
{
    bool clicked = ImGui::Selectable(label, &selected, flags, size);
    return std::make_tuple(clicked, selected);
}

static std::tuple<bool, bool> ImGuiMenuItemLua(const char* label, const char* shortcut, bool selected, bool enabled)
{
    bool clicked = ImGui::MenuItem(label, ProcessNullableString(shortcut), &selected, enabled);
    return std::make_tuple(clicked, selected);
}

static bool ImGuiBeginPopupContextItemLua(const char* str_id, ImGuiPopupFlags flags) 
{
    return ImGui::BeginPopupContextItem(ProcessNullableString(str_id), flags);
}

static bool ImGuiBeginPopupContextWindowLua(const char* str_id, ImGuiPopupFlags flags) 
{
    return ImGui::BeginPopupContextWindow(ProcessNullableString(str_id), flags);
}

static bool ImGuiBeginPopupContextVoidLua(const char* str_id, ImGuiPopupFlags flags)
{
    return ImGui::BeginPopupContextVoid(ProcessNullableString(str_id), flags);
}

static bool ImGuiBeginPopupModalLua(const char* name, bool open, ImGuiWindowFlags flags)
{
    return ImGui::BeginPopupModal(name, &open, flags);
}

static void ImGuiOpenPopupLua(const char* str_id, ImGuiPopupFlags flags)
{
    ImGui::OpenPopup(str_id, flags);
}

static void ImGuiPushStyleVarLuaVec2(ImGuiStyleVar idx, const ImVec2& val)
{
    ImGui::PushStyleVar(idx, val);
}

static void ImGuiPushStyleVarLuaFloat(ImGuiStyleVar idx, float val)
{
    ImGui::PushStyleVar(idx, val);
}

static void ImGuiPushStyleColorLua(ImGuiCol idx, const ImVec4& val)
{
    ImGui::PushStyleColor(idx, val);
}

static bool ImGuiColorButtonLua(const char* desc_id, float r, float g, float b, float a, ImGuiColorEditFlags flags, ImVec2& size)
{
    ImVec4 vec = ImVec4(r, g, b, a);
    return ImGui::ColorButton(desc_id, vec, flags, size);
}

static std::tuple<float, float, float, float>ImGuiColorEdit4Lua(const char* label, float r, float g, float b, float a, ImGuiColorEditFlags flags) 
{
    float colorArray[4] = { r, g, b, a };
    ImGui::ColorEdit4(label, colorArray, flags);
    return std::make_tuple(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
}

static std::tuple<float, float, float>ImGuiColorEdit3Lua(const char* label, float r, float g, float b, ImGuiColorEditFlags flags)
{
    float colorArray[3] = { r, g, b };
    ImGui::ColorEdit3(label, colorArray, flags);
    return std::make_tuple(colorArray[0], colorArray[1], colorArray[2]);
}

static std::tuple<float, float, float, float>ImGuiDragFloat4Lua(const char* label, float x, float y, float z, float w, float speed, float min, float max, ImGuiSliderFlags flags)
{
    float floatArray[4] = { x, y, z, w };
    bool changed = ImGui::DragFloat4(label, floatArray, speed, min, max, "%.3f", flags);
    return std::make_tuple(floatArray[0], floatArray[1], floatArray[2], floatArray[3]);
}


static std::tuple<float, float, float>ImGuiDragFloat3Lua(const char* label, float x, float y, float z, float speed, float min, float max, ImGuiSliderFlags flags)
{
    float floatArray[3] = { x, y, z };
    bool changed = ImGui::DragFloat3(label, floatArray, speed, min, max, "%.3f", flags);
    return std::make_tuple(floatArray[0], floatArray[1], floatArray[2]);
}

static std::tuple<float, float>ImGuiDragFloat2Lua(const char* label, float x, float y, float speed, float min, float max, ImGuiSliderFlags flags)
{
    float floatArray[2] = { x, y };
    bool changed = ImGui::DragFloat2(label, floatArray, speed, min, max, "%.3f", flags);
    return std::make_tuple(floatArray[0], floatArray[1]);
}

static std::tuple<int, int, int, int>ImGuiDragInt4Lua(const char* label, int x, int y, int z, int w, float speed, int min, int max, ImGuiSliderFlags flags)
{
    int intArray[4] = { x, y, z , w };
    bool changed = ImGui::DragInt4(label, intArray, speed, min, max, "%d", flags);
    return std::make_tuple(intArray[0], intArray[1], intArray[2], intArray[3]);
}

static std::tuple<int, int, int>ImGuiDragInt3Lua(const char* label, int x, int y, int z, float speed, int min, int max, ImGuiSliderFlags flags)
{
    int intArray[3] = { x, y, z };
    bool changed = ImGui::DragInt3(label, intArray, speed, min, max, "%d", flags);
    return std::make_tuple(intArray[0], intArray[1], intArray[2]);
}

static std::tuple<int, int>ImGuiDragInt2Lua(const char* label, int x, int y, float speed, int min, int max, ImGuiSliderFlags flags)
{
    int intArray[2] = { x, y };
    bool changed = ImGui::DragInt2(label, intArray, speed, min, max, "%d", flags);
    return std::make_tuple(intArray[0], intArray[1]);
}

static bool ImGuiBeginChildLua(const char* str_id, const ImVec2& size_arg, bool border, ImGuiWindowFlags extra_flags)
{
    return ImGui::BeginChild(str_id, size_arg, border, extra_flags);
}

static std::string ImGuiInputTextLua(const char* label, std::string str, ImGuiInputTextFlags flags) 
{
    ImGui::InputText(label, &str, flags);
    return str;
}

static std::string ImGuiInputTextMultilineLua(const char* label, std::string str, const ImVec2& size, ImGuiInputTextFlags flags)
{
    ImGui::InputTextMultiline(label, &str, size, flags);
    return str;
}

static std::string ImGuiInputTextWithHintLua(const char* label, const char* hint, std::string str, ImGuiInputTextFlags flags)
{
    ImGui::InputTextWithHint(label, hint, &str, flags);
    return str;
}

static bool ImGuiTreeNodeLua(const char* label) 
{
    return ImGui::TreeNode(label);
}

static int ImGuiInputIntLua(const char* label, int v, int step, int step_fast, ImGuiInputTextFlags flags)
{
    ImGui::InputInt(label, &v, step, step_fast, flags);
    return v;
}

static float ImGuiInputFloatLua(const char* label, float v, float step, float step_fast, ImGuiInputTextFlags flags)
{
    ImGui::InputFloat(label, &v, step, step_fast, "%.3f", flags);
    return v;
}

static void ImGuiPlotHistogramLua(const char* label, LuaRef values, int offset, const char* overlayText, float scale_min, float scale_max, ImVec2 size, int stride)
{
    std::vector<float> valuesVector;
    if (!TableValuesToArray<float>(values, LuaTypeID::NUMBER, valuesVector))
        return;

    ImGui::PlotHistogram(label, valuesVector.data(), static_cast<int>(valuesVector.size()), offset, ProcessNullableString(overlayText), scale_min, scale_max, size, stride);
}

static void ImGuiPlotLinesLua(const char* label, LuaRef values, int offset, const char* overlayText, float scale_min, float scale_max, ImVec2 size, int stride) 
{
    std::vector<float> valuesVector;
    if (!TableValuesToArray<float>(values, LuaTypeID::NUMBER, valuesVector))
        return;

    ImGui::PlotLines(label, valuesVector.data(), static_cast<int>(valuesVector.size()), offset, ProcessNullableString(overlayText), scale_min, scale_max, size, stride);
}

static void ImGuiProgressBar(float fraction, const ImVec2& size, const char* overlay)
{
    ImGui::ProgressBar(fraction, size, ProcessNullableString(overlay));
}

static void ImGuiImageLua(MM2::gfxTexture* texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tintColor, const ImVec4& borderColor) {
    ImGui::Image((void*)texture, size, uv0, uv1, tintColor, borderColor);
}

//Helpers copied from the demo
static void CenterNextWindow()
{
    ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
}

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static bool ImGuiBeginTabItemLua(const char* label, ImGuiTabItemFlags flags) 
{
    return ImGui::BeginTabItem(label, NULL, flags);
}

static ImVec2 ImGuiCalcTextSizeLua(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
{
    text_end = ProcessNullableString(text_end);
    return ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
}

static void ImGuiColumns(int count, const char* id, bool border)
{
    id = ProcessNullableString(id);
    ImGui::Columns(count, id, border);
}

///////////////////
//IMPLOT BINDINGS//
///////////////////
static bool ImPlot_BeginPlot(const char* title_id, const char* x_label, const char* y_label, ImVec2 size, ImPlotFlags flags ,
                            ImPlotAxisFlags x_flags, ImPlotAxisFlags y_flags, ImPlotAxisFlags y2_flags, ImPlotAxisFlags y3_flags) {

    return ImPlot::BeginPlot(title_id, ProcessNullableString(x_label), ProcessNullableString(y_label), size, flags, x_flags, y_flags, y2_flags, y3_flags);
}

static void ImPlot_EndPlot() {
    ImPlot::EndPlot();
}

static void ImPlot_PlotText(const char* text, double x, double y, ImVec2 pix_offset, ImPlotTextFlags flags) {
    ImPlot::PlotText(text, x, y, pix_offset, flags);
}

static void ImPlot_PlotBars(const char* label_id, LuaRef xValues, LuaRef yValues, double width, int offset) {
    std::vector<float> xAxisVector;
    if (!TableValuesToArray<float>(xValues, LuaTypeID::NUMBER, xAxisVector))
        return;

    std::vector<float> yAxisVector;
    if (!TableValuesToArray<float>(yValues, LuaTypeID::NUMBER, yAxisVector))
        return;

    int count = min(xAxisVector.size(), yAxisVector.size());
    ImPlot::PlotBars(label_id, xAxisVector.data(), yAxisVector.data(), count, width, offset);
}

static void ImPlot_PlotScatter(const char* label_id, LuaRef xValues, LuaRef yValues, int offset) {
    std::vector<float> xAxisVector;
    if (!TableValuesToArray<float>(xValues, LuaTypeID::NUMBER, xAxisVector))
        return;

    std::vector<float> yAxisVector;
    if (!TableValuesToArray<float>(yValues, LuaTypeID::NUMBER, yAxisVector))
        return;

    int count = min(xAxisVector.size(), yAxisVector.size());
    ImPlot::PlotScatter(label_id, xAxisVector.data(), yAxisVector.data(), count, offset);
}

template <typename T>
static void ImPlot_SharedPlotFnSignature(const char* label, LuaRef xValues, LuaRef yValues, int offset, void(*plotFunction)(const char* label, T* x, T* y, int count, int offset)) 
{
    std::vector<float> xAxisVector;
    if (!TableValuesToArray<float>(xValues, LuaTypeID::NUMBER, xAxisVector))
        return;

    std::vector<float> yAxisVector;
    if (!TableValuesToArray<float>(yValues, LuaTypeID::NUMBER, yAxisVector))
        return;

    int count = min(xAxisVector.size(), yAxisVector.size());
    plotFunction(label, xAxisVector.data(), yAxisVector.data(), count, offset);
}

static void ImPlot_PlotLine(const char* label, LuaRef xValues, LuaRef yValues, int offset) {
    std::vector<float> xAxisVector;
    if (!TableValuesToArray<float>(xValues, LuaTypeID::NUMBER, xAxisVector))
        return;

    std::vector<float> yAxisVector;
    if (!TableValuesToArray<float>(yValues, LuaTypeID::NUMBER, yAxisVector))
        return;

    int count = min(xAxisVector.size(), yAxisVector.size());
    ImPlot::PlotLine(label, xAxisVector.data(), yAxisVector.data(), count, offset);
}

static void ImPlot_PlotDigital(const char* label, LuaRef xValues, LuaRef yValues, int offset) {
    std::vector<float> xAxisVector;
    if (!TableValuesToArray<float>(xValues, LuaTypeID::NUMBER, xAxisVector))
        return;

    std::vector<float> yAxisVector;
    if (!TableValuesToArray<float>(yValues, LuaTypeID::NUMBER, yAxisVector))
        return;

    int count = min(xAxisVector.size(), yAxisVector.size());
    ImPlot::PlotDigital(label, xAxisVector.data(), yAxisVector.data(), count, offset);
}

// ImGuiStyle helpers
static void ImGuiStyle_SetColor(ImGuiStyle& style, int index, ImVec4 color) {
    if(index < ImGuiCol_COUNT)
        style.Colors[index] = color;
}

static ImVec4 ImGuiStyle_GetColor(ImGuiStyle& style, int index) {
    if (index < ImGuiCol_COUNT)
        return style.Colors[index];
    else
        return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
}

// ImFontAtlas helpers
static ImFont* ImFontAtlas_AddFontFromFileTTF(ImFontAtlas& atlas, const char* file, float size_pixels)
{
    return atlas.AddFontFromFileTTF(file, size_pixels);
}

static ImFont* ImFontAtlas_AddFontFromMemoryTTF(ImFontAtlas& atlas, const char* data, int dataSize, float size_pixels)
{
    auto ownedFontData = IM_ALLOC(dataSize);
    memcpy(ownedFontData, data, dataSize);
    return atlas.AddFontFromMemoryTTF(ownedFontData, dataSize, size_pixels);
}

static ImFont* ImFontAtlas_AddFontFromMemoryCompressedBase85TTF(ImFontAtlas& atlas, const char* data, float size_pixels)
{
    return atlas.AddFontFromMemoryCompressedBase85TTF(data, size_pixels);
}

//
static void ImguiBindLua(LuaState L) {
    LuaBinding(L).beginClass<ImPlotPoint>("ImPlotPoint")
        .addFactory([](double x = 0.0, double y = 0.0) {
            auto vec = new ImPlotPoint();
            vec->x = x;
            vec->y = y;
            return vec;
        }, LUA_ARGS(_opt<double>, _opt<double>))
        .addVariable("x", &ImPlotPoint::x)
        .addVariable("y", &ImPlotPoint::y)
        .endClass();

    LuaBinding(L).beginClass<ImVec2>("ImVec2")
        .addFactory([](float x = 0.0, float y = 0.0) {
            auto vec = new ImVec2;
            vec->x = x;
            vec->y = y;
            return vec;
        }, LUA_ARGS(_opt<float>, _opt<float>))
        .addVariable("x", &ImVec2::x)
        .addVariable("y", &ImVec2::y)
        .endClass();

    LuaBinding(L).beginClass<ImVec4>("ImVec4")
        .addFactory([](float x = 0.0, float y = 0.0, float z = 0.0, float w = 0.0) {
            auto vec = new ImVec4;
            vec->x = x;
            vec->y = y;
            vec->z = z;
            vec->w = w;
            return vec;
        }, LUA_ARGS(_opt<float>, _opt<float>, _opt<float>, _opt<float>))
        .addVariable("x", &ImVec4::x)
        .addVariable("y", &ImVec4::y)
        .addVariable("z", &ImVec4::z)
        .addVariable("w", &ImVec4::w)
        .endClass();
    
    LuaBinding(L).beginClass<ImFont>("ImFont")
        .endClass();

    LuaBinding(L).beginClass<ImFontConfig>("ImFontConfig")
        .endClass();

    LuaBinding(L).beginClass<ImFontAtlas>("ImFontAtlas")
        .addFunction("Build", &ImFontAtlas::Build)
        .addMetaFunction("AddFontFromFileTTF", &ImFontAtlas_AddFontFromFileTTF)
        .addMetaFunction("AddFontFromMemoryTTF", &ImFontAtlas_AddFontFromMemoryTTF)
        .addMetaFunction("AddFontFromMemoryCompressedBase85TTF", &ImFontAtlas_AddFontFromMemoryCompressedBase85TTF)
        .endClass();

    LuaBinding(L).beginClass<ImGuiIO>("ImGuiIO")
        .addVariable("DisplaySize", &ImGuiIO::DisplaySize)
        .addVariable("DeltaTime", &ImGuiIO::DeltaTime)
        .addVariable("IniSavingRate", &ImGuiIO::IniSavingRate)
        .addVariable("IniFilename", &ImGuiIO::IniFilename)
        .addVariable("LogFilename", &ImGuiIO::LogFilename)
        .addVariable("MouseDoubleClickTime", &ImGuiIO::MouseDoubleClickTime)
        .addVariable("MouseDoubleClickMaxDist", &ImGuiIO::MouseDoubleClickMaxDist)
        .addVariable("MouseDragThreshold", &ImGuiIO::MouseDragThreshold)
        .addVariable("KeyRepeatDelay", &ImGuiIO::KeyRepeatDelay)
        .addVariable("KeyRepeatRate", &ImGuiIO::KeyRepeatRate)
        .addVariable("FontGlobalScale", &ImGuiIO::FontGlobalScale)
        .addVariable("FontAllowUserScaling", &ImGuiIO::FontAllowUserScaling)
        .addVariable("DisplayFramebufferScale", &ImGuiIO::DisplayFramebufferScale)
        .addVariable("ConfigDockingNoSplit", &ImGuiIO::ConfigDockingNoSplit)
        .addVariable("ConfigDockingWithShift", &ImGuiIO::ConfigDockingWithShift)
        .addVariable("ConfigDockingAlwaysTabBar", &ImGuiIO::ConfigDockingAlwaysTabBar)
        .addVariable("ConfigDockingTransparentPayload", &ImGuiIO::ConfigDockingTransparentPayload)
        .addVariable("ConfigViewportsNoAutoMerge", &ImGuiIO::ConfigViewportsNoAutoMerge)
        .addVariable("ConfigViewportsNoTaskBarIcon", &ImGuiIO::ConfigViewportsNoTaskBarIcon)
        .addVariable("ConfigViewportsNoDecoration", &ImGuiIO::ConfigViewportsNoDecoration)
        .addVariable("ConfigViewportsNoDefaultParent", &ImGuiIO::ConfigViewportsNoDefaultParent)
        .addVariable("MouseDrawCursor", &ImGuiIO::MouseDrawCursor)
        .addVariable("ConfigMacOSXBehaviors", &ImGuiIO::ConfigMacOSXBehaviors)
        .addVariable("ConfigInputTextCursorBlink", &ImGuiIO::ConfigInputTextCursorBlink)
        .addVariable("ConfigWindowsResizeFromEdges", &ImGuiIO::ConfigWindowsResizeFromEdges)
        .addVariable("ConfigWindowsMoveFromTitleBarOnly", &ImGuiIO::ConfigWindowsMoveFromTitleBarOnly)
        .addVariableRef("Fonts", &ImGuiIO::Fonts, false)
        .addVariable("MousePos", &ImGuiIO::MousePos)
        .addVariable("MouseWheel", &ImGuiIO::MouseWheel)
        .addVariable("MouseWheelH", &ImGuiIO::MouseWheelH)
        .addVariable("MouseHoveredViewport", &ImGuiIO::MouseHoveredViewport)
        .addVariable("KeyCtrl", &ImGuiIO::KeyCtrl)
        .addVariable("KeyShift", &ImGuiIO::KeyShift)
        .addVariable("KeyAlt", &ImGuiIO::KeyAlt)
        .addVariable("KeySuper", &ImGuiIO::KeySuper)
        .addVariable("WantCaptureMouse", &ImGuiIO::WantCaptureMouse)
        .addVariable("WantCaptureKeyboard", &ImGuiIO::WantCaptureKeyboard)
        .addVariable("WantTextInput", &ImGuiIO::WantTextInput)
        .addVariable("WantSetMousePos", &ImGuiIO::WantSetMousePos)
        .addVariable("WantSaveIniSettings", &ImGuiIO::WantSaveIniSettings)
        .addVariable("NavActive", &ImGuiIO::NavActive)
        .addVariable("NavVisible", &ImGuiIO::NavVisible)
        .addVariable("Framerate", &ImGuiIO::Framerate)
        .addVariable("MetricsRenderVertices", &ImGuiIO::MetricsRenderVertices)
        .addVariable("MetricsRenderIndices", &ImGuiIO::MetricsRenderIndices)
        .addVariable("MetricsRenderWindows", &ImGuiIO::MetricsRenderWindows)
        .addVariable("MetricsActiveWindows", &ImGuiIO::MetricsActiveWindows)
        .addVariable("MetricsActiveAllocations", &ImGuiIO::MetricsActiveAllocations)
        .addVariable("MouseDelta", &ImGuiIO::MouseDelta)
        .endClass();

    LuaBinding(L).beginClass<ImGuiStyle>("ImGuiStyle")
        .addVariable("Alpha", &ImGuiStyle::Alpha)
        .addVariable("WindowPadding", &ImGuiStyle::WindowPadding)
        .addVariable("WindowRounding", &ImGuiStyle::WindowRounding)
        .addVariable("WindowBorderSize", &ImGuiStyle::WindowBorderSize)
        .addVariable("WindowMinSize", &ImGuiStyle::WindowMinSize)
        .addVariable("WindowTitleAlign", &ImGuiStyle::WindowTitleAlign)
        .addVariable("WindowMenuButtonPosition", &ImGuiStyle::WindowMenuButtonPosition)
        .addVariable("ChildRounding", &ImGuiStyle::ChildRounding)
        .addVariable("ChildBorderSize", &ImGuiStyle::ChildBorderSize)
        .addVariable("PopupRounding", &ImGuiStyle::PopupRounding)
        .addVariable("PopupBorderSize", &ImGuiStyle::PopupBorderSize)
        .addVariable("FramePadding", &ImGuiStyle::FramePadding)
        .addVariable("FrameRounding", &ImGuiStyle::FrameRounding)
        .addVariable("FrameBorderSize", &ImGuiStyle::FrameBorderSize)
        .addVariable("ItemSpacing", &ImGuiStyle::ItemSpacing)
        .addVariable("ItemInnerSpacing", &ImGuiStyle::ItemInnerSpacing)
        .addVariable("TouchExtraPadding", &ImGuiStyle::TouchExtraPadding)
        .addVariable("IndentSpacing", &ImGuiStyle::IndentSpacing)
        .addVariable("ColumnsMinSpacing", &ImGuiStyle::ColumnsMinSpacing)
        .addVariable("ScrollbarSize", &ImGuiStyle::ScrollbarSize)
        .addVariable("ScrollbarRounding", &ImGuiStyle::ScrollbarRounding)
        .addVariable("GrabMinSize", &ImGuiStyle::GrabMinSize)
        .addVariable("GrabRounding", &ImGuiStyle::GrabRounding)
        .addVariable("LogSliderDeadzone", &ImGuiStyle::LogSliderDeadzone)
        .addVariable("TabRounding", &ImGuiStyle::TabRounding)
        .addVariable("TabBorderSize", &ImGuiStyle::TabBorderSize)
        .addVariable("ColorButtonPosition", &ImGuiStyle::ColorButtonPosition)
        .addVariable("ButtonTextAlign", &ImGuiStyle::ButtonTextAlign)
        .addVariable("SelectableTextAlign", &ImGuiStyle::SelectableTextAlign)
        .addVariable("DisplayWindowPadding", &ImGuiStyle::DisplayWindowPadding)
        .addVariable("DisplaySafeAreaPadding", &ImGuiStyle::DisplaySafeAreaPadding)
        .addVariable("MouseCursorScale", &ImGuiStyle::MouseCursorScale)
        .addVariable("AntiAliasedLines", &ImGuiStyle::AntiAliasedLines)
        .addVariable("AntiAliasedLinesUseTex", &ImGuiStyle::AntiAliasedLinesUseTex)
        .addVariable("AntiAliasedFill", &ImGuiStyle::AntiAliasedFill)
        .addVariable("CurveTessellationTol", &ImGuiStyle::CurveTessellationTol)
        .addFunction("ScaleAllSizes", &ImGuiStyle::ScaleAllSizes)
        .addMetaFunction("GetColor", &ImGuiStyle_GetColor)
        .addMetaFunction("SetColor", &ImGuiStyle_SetColor)
        .endClass();

    LuaBinding(L).beginClass<ImDrawList>("ImDrawList")
        .addFunction("AddLine", &ImDrawList::AddLine, LUA_ARGS(const ImVec2 &, const ImVec2 &, ImU32, _def<float, 1>))
        .addFunction("AddTriangle", &ImDrawList::AddTriangle, LUA_ARGS(const ImVec2&, const ImVec2&, const ImVec2&, ImU32, _def<float, 1>))
        .addFunction("AddTriangleFilled", &ImDrawList::AddTriangleFilled, LUA_ARGS(const ImVec2&, const ImVec2&, const ImVec2&, ImU32))
        .addFunction("AddQuad", &ImDrawList::AddQuad, LUA_ARGS(const ImVec2&, const ImVec2&, const ImVec2&, const ImVec2&, ImU32, _def<float, 1>))
        .addFunction("AddQuadFilled", &ImDrawList::AddQuadFilled, LUA_ARGS(const ImVec2&, const ImVec2&, const ImVec2&, const ImVec2&, ImU32))
        .addFunction("AddCircle", &ImDrawList::AddCircle, LUA_ARGS(const ImVec2&, float, ImU32, _def<int, 0>, _def<float, 1>))
        .addFunction("AddCircleFilled", &ImDrawList::AddCircleFilled, LUA_ARGS(const ImVec2&, float, ImU32, _def<int, 0>))
        .addFunction("AddNgon", &ImDrawList::AddNgon, LUA_ARGS(const ImVec2&, float, ImU32, _def<int, 0>, _def<float, 1>))
        .addFunction("AddNgonFilled", &ImDrawList::AddNgonFilled, LUA_ARGS(const ImVec2&, float, ImU32, _def<int, 0>))
        .addFunction("AddRect", &ImDrawList::AddRect, LUA_ARGS(const ImVec2&, const ImVec2&, ImU32, _def<float, 0>, _def<int, 0>, _def<float, 1>))
        .addFunction("AddRectFilled", &ImDrawList::AddRectFilled, LUA_ARGS(const ImVec2&, const ImVec2&, ImU32, _def<float, 0>, _def<int, 0>))
        .addFunction("AddRectFilledMultiColor", &ImDrawList::AddRectFilledMultiColor, LUA_ARGS(const ImVec2&, const ImVec2&, ImU32, ImU32, ImU32, ImU32))
        .endClass();

    LuaBinding(L).beginModule("ImPlot")
        .addFunction("BeginPlot", &ImPlot_BeginPlot)
        .addFunction("EndPlot", &ImPlot_EndPlot)
        .addFunction("PlotLine", &ImPlot_PlotLine)
        .addFunction("PlotBars", &ImPlot_PlotBars)
        .addFunction("PlotScatter", &ImPlot_PlotScatter)
        .addFunction("PlotDigital", &ImPlot_PlotDigital)
        .addFunction("PlotText", &ImPlot_PlotText)
        .endModule();

    LuaBinding(L).beginModule("ImGui")
        .addFunction("Button", &ImGui::Button)
        .addFunction("InvisibleButton", &ImGui::InvisibleButton)
        .addFunction("SmallButton", &ImGui::SmallButton)
        .addFunction("RadioButton", &ImGuiRadioButtonLua)
        .addFunction("Checkbox", &ImGuiCheckboxLua)
        .addFunction("ProgressBar", &ImGuiProgressBar)

        .addFunction("SliderFloat", &ImGuiSliderFloatLua)  
        .addFunction("SliderInt", &ImGuiSliderIntLua)
        .addFunction("VSliderFloat", &ImGuiVSliderFloatLua)
        .addFunction("VSliderInt", &ImGuiVSliderIntLua)
        .addFunction("SliderFloat2", &ImGuiSliderFloat2Lua)
        .addFunction("SliderFloat3", &ImGuiSliderFloat3Lua)
        .addFunction("SliderFloat4", &ImGuiSliderFloat4Lua)
        .addFunction("SliderInt2", &ImGuiSliderInt2Lua)
        .addFunction("SliderInt3", &ImGuiSliderInt3Lua)
        .addFunction("SliderInt4", &ImGuiSliderInt4Lua)
        
        .addFunction("ColorButton", &ImGuiColorButtonLua)
        .addFunction("ColorEdit3", &ImGuiColorEdit3Lua)
        .addFunction("ColorEdit4", &ImGuiColorEdit4Lua)
        .addFunction("DragFloat", &ImGuiDragFloatLua)
        .addFunction("DragFloat2", &ImGuiDragFloat2Lua)
        .addFunction("DragFloat3", &ImGuiDragFloat3Lua)
        .addFunction("DragFloat4", &ImGuiDragFloat4Lua)
        .addFunction("DragInt", &ImGuiDragIntLua)
        .addFunction("DragInt2", &ImGuiDragInt2Lua)
        .addFunction("DragInt3", &ImGuiDragInt3Lua)
        .addFunction("DragInt4", &ImGuiDragInt4Lua)

        .addFunction("InputInt", &ImGuiInputIntLua)
        .addFunction("InputFloat", &ImGuiInputFloatLua)
        .addFunction("InputText", &ImGuiInputTextLua)
        .addFunction("InputTextMultiline", &ImGuiInputTextMultilineLua)
        .addFunction("InputTextWithHint", &ImGuiInputTextWithHintLua)

        .addFunction("Image", &ImGuiImageLua)

        .addFunction("BeginListBox", &ImGui::BeginListBox)
        .addFunction("EndListBox", &ImGui::EndListBox)
        .addFunction("ListBox", &ImGuiListBoxLua)
        .addFunction("Combo", &ImGuiComboLua)
        
        .addFunction("Text", &ImGuiTextLua)
        .addFunction("TextWrapped", &ImGuiTextWrappedLua)
        .addFunction("TextColored", &ImGuiTextColoredLua)
        .addFunction("LabelText", &ImGuiLabelTextLua)
        .addFunction("BulletText", &ImGuiBulletTextLua)
        
        .addFunction("LeftArrowButton", &ImGuiLeftArrowButtonLua)
        .addFunction("RightArrowButton", &ImGuiRightArrowButtonLua)
        .addFunction("UpArrowButton", &ImGuiUpArrowButtonLua)
        .addFunction("DownArrowButton", &ImGuiDownArrowButtonLua)

        .addFunction("CloseCurrentPopup", &ImGui::CloseCurrentPopup)
        .addFunction("OpenPopup", &ImGuiOpenPopupLua)
        .addFunction("EndPopup", &ImGui::EndPopup)
        .addFunction("BeginPopupModal", &ImGuiBeginPopupModalLua)
        .addFunction("BeginPopupContextItem", &ImGuiBeginPopupContextItemLua)
        .addFunction("BeginPopupContextWindow", &ImGuiBeginPopupContextWindowLua)
        .addFunction("BeginPopupContextVoid", &ImGuiBeginPopupContextVoidLua)
        .addFunction("Selectable", &ImGuiSelectableLua)
        .addFunction("MenuItem", &ImGuiMenuItemLua)

        .addFunction("CalcTextSize", &ImGuiCalcTextSizeLua)
        .addFunction("GetItemRectMax", &ImGui::GetItemRectMax)
        .addFunction("GetItemRectMin", &ImGui::GetItemRectMin)
        .addFunction("GetItemRectSize", &ImGui::GetItemRectSize)

        .addFunction("IsItemEdited", &ImGui::IsItemEdited)
        .addFunction("IsWindowFocused", &ImGui::IsWindowFocused)
        .addFunction("IsWindowHovered", &ImGui::IsWindowHovered)
        .addFunction("IsItemHovered", &ImGui::IsItemHovered)
        .addFunction("IsItemActive", &ImGui::IsItemActive)
        .addFunction("IsItemActivated", &ImGui::IsItemActivated)
        .addFunction("IsItemVisible", &ImGui::IsItemVisible)
        .addFunction("IsItemFocused", &ImGui::IsItemFocused)
        .addFunction("IsAnyItemActive", &ImGui::IsAnyItemActive)
        .addFunction("IsAnyItemFocused", &ImGui::IsAnyItemFocused)
        .addFunction("IsAnyItemHovered", &ImGui::IsAnyItemHovered)
        .addFunction("IsItemDeactivated", &ImGui::IsItemDeactivated)
        .addFunction("IsItemDeactivatedAfterEdit", &ImGui::IsItemDeactivatedAfterEdit)
        .addFunction("IsItemClicked", &ImGui::IsItemClicked)
        .addFunction("IsItemToggledOpen", &ImGui::IsItemToggledOpen)

        .addFunction("BeginDisabled", &ImGui::BeginDisabled)
        .addFunction("EndDisabled", &ImGui::EndDisabled)

        .addFunction("BeginTooltip", &ImGui::BeginTooltip)
        .addFunction("EndTooltip", &ImGui::EndTooltip)
        .addFunction("SetTooltip", &ImGuiSetTooltipLua)
        .addFunction("HelpMarker", &HelpMarker)

        .addFunction("CollapsingHeader", &ImGuiCollapsingHeaderLua)
        
        .addFunction("BeginChild", &ImGuiBeginChildLua)
        .addFunction("EndChild", &ImGui::EndChild)

        .addFunction("BeginMenuBar", &ImGui::BeginMenuBar)
        .addFunction("BeginMainMenuBar", &ImGui::BeginMainMenuBar)
        .addFunction("EndMenuBar", &ImGui::EndMenuBar)
        .addFunction("EndMainMenuBar", &ImGui::EndMainMenuBar)
        .addFunction("BeginMenu", &ImGui::BeginMenu)
        .addFunction("EndMenu", &ImGui::EndMenu)

        .addFunction("PushItemWidth", &ImGui::PushItemWidth)
        .addFunction("PopItemWidth", &ImGui::PopItemWidth)

        .addFunction("TreeNode", &ImGuiTreeNodeLua)
        .addFunction("TreePop", &ImGui::TreePop)

        .addFunction("Indent", &ImGui::Indent)
        .addFunction("Unindent", &ImGui::Unindent)

        .addFunction("SeparatorText", &ImGui::SeparatorText)
        .addFunction("Separator", &ImGui::Separator)
        .addFunction("Dummy", &ImGui::Dummy)
        .addFunction("Spacing", &ImGui::Spacing)

        .addFunction("PlotLines", &ImGuiPlotLinesLua)
        .addFunction("PlotHistogram", &ImGuiPlotHistogramLua)

        .addFunction("BeginTabBar", &ImGui::BeginTabBar)
        .addFunction("EndTabBar", &ImGui::EndTabBar)
        .addFunction("BeginTabItem", &ImGuiBeginTabItemLua)
        .addFunction("EndTabItem", &ImGui::EndTabItem)

        .addFunction("Begin", &ImGuiBeginLua)
        .addFunction("Begin2", &ImGuiBegin2Lua)
        .addFunction("End", &ImGui::End)

        .addFunction("Columns", &ImGuiColumns)
        .addFunction("GetColumnIndex", &ImGui::GetColumnIndex)
        .addFunction("GetColumnOffset", &ImGui::GetColumnOffset)
        .addFunction("GetColumnsCount", &ImGui::GetColumnsCount)
        .addFunction("GetColumnWidth", &ImGui::GetColumnWidth)
        .addFunction("NextColumn", &ImGui::NextColumn)
        .addFunction("SetColumnOffset", &ImGui::SetColumnOffset)
        .addFunction("SetColumnWidth", &ImGui::SetColumnWidth)

        .addFunction("SetNextItemWidth", &ImGui::SetNextItemWidth)
        .addFunction("SetItemDefaultFocus", &ImGui::SetItemDefaultFocus)
        .addFunction("SameLine", &ImGui::SameLine)
        .addFunction("AlignTextToFramePadding", &ImGui::AlignTextToFramePadding)
        .addFunction("GetTextLineHeight", &ImGui::GetTextLineHeight)
        .addFunction("CalcItemWidth", &ImGui::CalcItemWidth)
        .addFunction("GetFrameCount", &ImGui::GetFrameCount)
        .addFunction("GetFrameHeight", &ImGui::GetFrameHeight)
        .addFunction("GetFrameHeightWithSpacing", &ImGui::GetFrameHeightWithSpacing)
        .addFunction("GetWindowSize", &ImGui::GetWindowSize)
        .addFunction("GetWindowPos", &ImGui::GetWindowPos)
        .addFunction("SetNextWindowBgAlpha", &ImGui::SetNextWindowBgAlpha)
        .addFunction("SetNextWindowSize", &ImGui::SetNextWindowSize)
        .addFunction("SetNextWindowPos", &ImGui::SetNextWindowPos)
        .addFunction("SetNextWindowContentSize", &ImGui::SetNextWindowContentSize)
        .addFunction("CenterNextWindow", &CenterNextWindow)

        .addFunction("PushButtonRepeat", &ImGui::PushButtonRepeat)
        .addFunction("PopButtonRepeat", &ImGui::PopButtonRepeat)

        .addFunction("GetFont", &ImGui::GetFont)
        .addFunction("GetFontSize", &ImGui::GetFontSize)
        .addFunction("SetWindowFontScale", &ImGui::SetWindowFontScale)
        .addFunction("PushFont", &ImGui::PushFont)
        .addFunction("PopFont", &ImGui::PopFont)

        .addFunction("GetCursorPos", &ImGui::GetCursorPos)
        .addFunction("GetCursorPosX", &ImGui::GetCursorPosX)
        .addFunction("GetCursorPosY", &ImGui::GetCursorPosY)
        .addFunction("SetCursorPos", &ImGui::SetCursorPos)
        .addFunction("SetCursorPosX", &ImGui::SetCursorPosX)
        .addFunction("SetCursorPosY", &ImGui::SetCursorPosY)
        
        .addFunction("PushTextWrapPos", &ImGui::PushTextWrapPos)
        .addFunction("PopTextWrapPos", &ImGui::PopTextWrapPos)

        .addFunction("GetCursorScreenPos", &ImGui::GetCursorScreenPos)
        .addFunction("GetCursorScreenPos", &ImGui::GetCursorStartPos)
        .addFunction("SetScrollHereX", &ImGui::SetScrollHereX)
        .addFunction("SetScrollHereY", &ImGui::SetScrollHereY)
        .addFunction("SetScrollX", &ImGui::SetScrollX)
        .addFunction("SetScrollY", &ImGui::SetScrollY)
        .addFunction("SetScrollFromPosX", &ImGui::SetScrollFromPosX)
        .addFunction("SetScrollFromPosY", &ImGui::SetScrollFromPosY)
        .addFunction("GetScrollX", &ImGui::GetScrollX)
        .addFunction("GetScrollY", &ImGui::GetScrollY)
        .addFunction("GetScrollMaxX", &ImGui::GetScrollMaxX)
        .addFunction("GetScrollMaxY", &ImGui::GetScrollMaxY)

        .addFunction("GetMousePos", &ImGui::GetMousePos)
        .addFunction("IsMouseDown", &ImGui::IsMouseDown)
        .addFunction("IsMouseReleased", &ImGui::IsMouseReleased)
        .addFunction("IsAnyMouseDown", &ImGui::IsAnyMouseDown)
        .addFunction("GetMousePosOnOpeningCurrentPopup", &ImGui::GetMousePosOnOpeningCurrentPopup)
        .addFunction("IsMouseClicked",&ImGui::IsMouseClicked)
        .addFunction("GetMouseDragDelta", &ImGui::GetMouseDragDelta)
        .addFunction("IsMouseDragging", &ImGui::IsMouseDragging)

        .addFunction("GetWindowDrawList", &ImGui::GetWindowDrawList)

        .addFunction("ColorConvertU32ToFloat4", &ImGui::ColorConvertU32ToFloat4)
        .addFunction("ColorConvertFloat4ToU32", &ImGui::ColorConvertFloat4ToU32)

        .addFunction("GetClipboardText", &ImGui::GetClipboardText)
        .addFunction("SetClipboardText", &ImGui::SetClipboardText)

        .addFunction("PushStyleVarFloat", &ImGuiPushStyleVarLuaFloat)
        .addFunction("PushStyleVarVec2", &ImGuiPushStyleVarLuaVec2)
        .addFunction("PopStyleVar", &ImGui::PopStyleVar)

        .addFunction("PushStyleColor", &ImGuiPushStyleColorLua)
        .addFunction("PopStyleColor", &ImGui::PopStyleColor)

        .addFunction("PushID", &ImGuiPushIDLua)
        .addFunction("PopID", &ImGui::PopID)

        .addFunction("ShowDemoWindow", &ImGuiShowDemoWindowLua)
        .addFunction("GetIO", &ImGui::GetIO)
        .addFunction("GetStyle", &ImGui::GetStyle)
        .endModule();
}