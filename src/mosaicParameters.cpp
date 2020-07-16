#include "mosaicParameters.h"
#include<string>

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

template<> const char* getLinkName<long double>() { return "VP_LINK_NUMERIC"; };
template<> const char* getLinkName<float>() { return "VP_LINK_NUMERIC"; };
template<> const char* getLinkName<int>() { return "VP_LINK_NUMERIC"; };
template<> const char* getLinkName<std::string>() { return "VP_LINK_STRING"; };
template<> const char* getLinkName<char[]>() { return "VP_LINK_STRING"; };

std::ostream& operator << (std::ostream& _out, const VPError& _e){
    _out << "VPError=[" << _e.code << "/" << _e.status << "] " << _e.message << std::endl;
    return _out;
}

// - - - - - - - - - -
// TYPE some parameters
// - - - - - - - - - -
// INT SPACIALISATIONS
template<>
void Parameter<int>::drawImGui() {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?

    ImGui::AlignTextToFramePadding();

    // inlet connector
    ImGuiDrawParamConnector(true);

    if(!this->getIsEditable()){
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); // todo: wrap this inner ImGui function into a small ImGuiEx
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    //ImGui::SetNextItemWidth(-1); // sets item to take full witdh. Not rendered well for now.
    // todo: remove const_cast somehow ?
    ImGui::DragInt( this->getDisplayName().c_str(), const_cast<int*>(&this->getValue()) );

    if(!this->getIsEditable()){
        //ImGui::PopItemFlag(); // todo: same as above
        ImGui::PopStyleVar();
    }

    // listen for GUI connections
    ImGuiListenForParamDrop();

    // tmp, menu
    ImGuiShowInfoMenu();

    // outlet connector
    ImGuiDrawParamConnector();

    ImGui::PopID();
};
template<>
std::string Parameter<int>::serialize(const bool& _serializeStoredValue) const {
    return std::to_string( _serializeStoredValue ? this->storedValue : this->dataValue );
};
template<>
bool Parameter<int>::unserialize( const std::string& _value, const bool& _unserializeToStoredValue ) {
    if(_unserializeToStoredValue) this->storedValue = std::stoi(_value);
    this->dataValue = std::stoi(_value);
    return true;
};
// - - - - - - - - - -
// FLOAT SPACIALISATIONS
template<>
void Parameter<float>::drawImGui() {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?

    // inlet connector
    ImGuiDrawParamConnector(true);

    if(!this->getIsEditable()){
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); // todo: wrap this inner ImGui function into a small ImGuiEx
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    // todo: remove const_cast somehow ? (undefined c++ behaviour)
    ImGui::DragFloat(this->getDisplayName().c_str(), const_cast<float*>(&this->getValue()) );//const_cast<float*>(&this->getValue()) );
    if(!this->getIsEditable()){
        //ImGui::PopItemFlag(); // todo: same as above
        ImGui::PopStyleVar();
    }

    // listen for GUI connections
    ImGuiListenForParamDrop();

    // tmp, menu
    ImGuiShowInfoMenu();

    // outlet connector
    ImGuiDrawParamConnector();

    ImGui::PopID();
};
template<>
std::string Parameter<float>::serialize(const bool& _serializeStoredValue) const {
    return std::to_string( _serializeStoredValue ? this->storedValue : this->dataValue );
};
template<>
bool Parameter<float>::unserialize( const std::string& _value, const bool& _unserializeToStoredValue ) {
    if(_unserializeToStoredValue) this->storedValue = std::stof(_value);
    this->dataValue = std::stof(_value);

    // todo: bflagDataChanged

    return true;
};
// - - - - - - - - - -
// STRING SPACIALISATIONS
template<>
void Parameter<std::string>::drawImGui() {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?

    // inlet connector
    ImGuiDrawParamConnector(true);

    if(!this->getIsEditable()){
        //ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true); // todo: wrap this inner ImGui function into a small ImGuiEx
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }
    // todo: remove const_cast somehow ?
    ImGui::InputText(this->getDisplayName().c_str(), static_cast<std::string *>( const_cast<std::string*>(&this->getValue() )));
    if(!this->getIsEditable()){
        //ImGui::PopItemFlag(); // todo: same as above
        ImGui::PopStyleVar();
    }

    // listen for GUI connections
    ImGuiListenForParamDrop();

    // tmp, menu
    ImGuiShowInfoMenu();

    // outlet connector
    ImGuiDrawParamConnector();

    ImGui::PopID();
};
template<>
std::string Parameter<std::string>::serialize(const bool& _serializeStoredValue) const {
    return _serializeStoredValue ? this->storedValue : this->dataValue;
};
template<>
bool Parameter<std::string>::unserialize( const std::string& _value, const bool& _unserializeToStoredValue ) {
    if(_unserializeToStoredValue) this->storedValue = _value;
    else this->dataValue = _value;
    return true;
};
