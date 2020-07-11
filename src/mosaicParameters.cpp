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

// Inline utility
template<class FULLTYPE, typename DATATYPE>
inline void ImGuiShowParamInfoOnRightClick(const FULLTYPE& _param){
    // Show unique ID when clicking on the label (tmp?)
    if(ImGui::IsMouseDown(0) && !ImGui::IsItemActive() && ImGui::IsItemHovered() && ImGui::IsWindowFocused() && 0 == ImGui::GetMouseDragDelta(0).x){
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted( _param.getUID().c_str() );

        ImGui::LabelText("HasOutlet", "%s", _param.hasOutlet()?"1":"0" );
        ImGui::LabelText("Modifiers", "%i", _param.getNumModifiers() );
        try {
            if( _param.template hasModifier< ParamInletModifier< DATATYPE > >() ){
                ParamInletModifier< DATATYPE >& paramInlet = const_cast<FULLTYPE&>( _param ).template getOrCreateModifier< ParamInletModifier< DATATYPE > >();
                ImGui::LabelText( "InletModifier", "Connected: %s", paramInlet.myLink.isConnected?"1":"0" );
            }
            else {
                 ImGui::LabelText( "InletModifier", "%s", "None" );
            }
        } catch(...) {
            ImGui::LabelText( "InletModifier", "%s", "Error/Unavailable" );
        }
        ImGui::Separator();
        std::ostringstream storedValue;     std::ostringstream outputValue;     std::ostringstream baseValue;
        storedValue << _param.dataValue;    outputValue << _param.getValue();   baseValue << _param.getBaseValue();
        ImGui::LabelText("storedValue", "%s",   storedValue.str().c_str() );
        ImGui::LabelText("baseValue", "%s",     baseValue.str().c_str());
        ImGui::LabelText("outputValue", "%s",   outputValue.str().c_str() );

        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }

};

// TYPE some parameters
template<>
void Parameter<int>::drawImGui() const {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?
    // todo: remove const_cast somehow ?
    ImGui::DragInt(this->getDisplayName().c_str(), const_cast<int*>(&this->getValue()) );
    ImGuiShowParamInfoOnRightClick< Parameter<int>, int >(*this);
    ImGui::PopID();
};
template<>
void Parameter<float>::drawImGui() const {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?
    // todo: remove const_cast somehow ?
    ImGui::DragFloat(this->getDisplayName().c_str(), const_cast<float*>(&this->getValue()) );//const_cast<float*>(&this->getValue()) );

    // tmp: show variable's address
    if( ImGui::IsItemHovered() && ImGui::IsMouseDown(1) ) {
        std::cout << "Parameter<float> " << this->getUID() <<
        " [value]=" << &this->getValue() <<
        " [base]=" << &this->getBaseValue() <<
        " [modifiers]=" << this->paramModifiers.size() <<
        //" [transformed]" << &this->paramModifiers.back()->transformValue( this->getBaseValue() ) <<
        std::endl;
    }

    ImGuiShowParamInfoOnRightClick< Parameter<float>, float >(*this);
    ImGui::PopID();
};
template<>
void Parameter<std::string>::drawImGui() const {
    ImGui::PushID(this->getUID().c_str()); // todo: do this before calling drawImGui() ?
    // todo: remove const_cast somehow ?
    ImGui::InputText(this->getDisplayName().c_str(), static_cast<std::string *>( const_cast<std::string*>(&this->getValue() )));
    ImGuiShowParamInfoOnRightClick< Parameter<std::string>, std::string >(*this);
    ImGui::PopID();
};
