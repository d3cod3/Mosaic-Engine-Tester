#include "mosaicParameters.h"

// Initialise static members
std::vector<AbstractParameter*> AbstractParameter::allParams = std::vector<AbstractParameter*>();

template<> LinkType getLinkType<long double>() { return VP_LINK_NUMERIC; };
template<> LinkType getLinkType<float>() { return VP_LINK_NUMERIC; };
template<> LinkType getLinkType<int>() { return VP_LINK_NUMERIC; };
template<> LinkType getLinkType<std::string>() { return VP_LINK_STRING; };
template<> LinkType getLinkType<char[]>() { return VP_LINK_STRING; };
//template<> LinkType getLinkType<array>() { return VP_LINK_ARRAY; };
//template<> LinkType getLinkType<ofTexture>() { return VP_LINK_TEXTURE; };
//template<> LinkType getLinkType<ofSoundBuffer>() { return VP_LINK_SPECIAL; };
//template<> LinkType getLinkType<ofAudioBuffer>() { return VP_LINK_AUDIO; };
//template<> LinkType getLinkType<ofPixels>() { return VP_LINK_PIXELS; };

template<> std::string getLinkName<long double>() { return "VP_LINK_NUMERIC"; };
template<> std::string getLinkName<float>() { return "VP_LINK_NUMERIC"; };
template<> std::string getLinkName<int>() { return "VP_LINK_NUMERIC"; };
template<> std::string getLinkName<std::string>() { return "VP_LINK_STRING"; };
template<> std::string getLinkName<char[]>() { return "VP_LINK_STRING"; };

std::ostream& operator << (std::ostream& _out, const VPError& _e){
    _out << "VPError=[" << _e.code << "/" << _e.status << "] " << _e.message << std::endl;
    return _out;
}

// TYPE some parameters
template<>
void Parameter<int>::drawImGui() {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?

    // inlet connector
    ImGuiDrawParamConnector(true);

    // todo: remove const_cast somehow ?
    ImGui::DragInt(this->getDisplayName().c_str(), const_cast<int*>(&this->getValue()) );

    // listen for GUI connections
    ImGuiListenForParamDrop();

    // tmp, menu
    ImGuiShowInfoMenu();

    // outlet connector
    ImGuiDrawParamConnector();

    ImGui::PopID();
};

template<>
void Parameter<float>::drawImGui() {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?

    // inlet connector
    ImGuiDrawParamConnector(true);

    // todo: remove const_cast somehow ? (undefined c++ behaviour)
    ImGui::DragFloat(this->getDisplayName().c_str(), const_cast<float*>(&this->getValue()) );//const_cast<float*>(&this->getValue()) );

    // listen for GUI connections
    ImGuiListenForParamDrop();

    // tmp, menu
    ImGuiShowInfoMenu();

    // outlet connector
    ImGuiDrawParamConnector();

    ImGui::PopID();
};
template<>
void Parameter<std::string>::drawImGui() {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?

    // inlet connector
    ImGuiDrawParamConnector(true);

    // todo: remove const_cast somehow ?
    ImGui::InputText(this->getDisplayName().c_str(), static_cast<std::string *>( const_cast<std::string*>(&this->getValue() )));

    // listen for GUI connections
    ImGuiListenForParamDrop();

    // tmp, menu
    ImGuiShowInfoMenu();

    // outlet connector
    ImGuiDrawParamConnector();

    ImGui::PopID();
};
