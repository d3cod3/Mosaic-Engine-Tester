// - - - - - - - - - -

// Notes on implementation choices :

// VIRTUALS and ABSTRACTS RELATIONSHIP
// To make abstracts (typed objects with common methods and a fixed footprint for holding them in lists), we need the virtual keyword to define an "entry door". Virtuals are resolved on run-time, which is not the fastest; with -02 compile optimisation, they become faster; to be tested/benched. In the optimal scenario, this (calling a method from an abstract) should be the only valid reason for using virtuals. Think `virtual myApiMethod();`. Intern functions better use static crtp.
// Alternatively, to prevent virtuals usage, we could use c++11 functors. On instantiation, you register a function address, then call it when you need to. More like a simplified static event notifier.

// TEMPLATES
// Most definitions need to be in the .h file because of Implicit template instantiation.
// Tool tor seeing how the compiler "builds" your templated code : https://cppinsights.io/

// - - - - - - - - - -

// Good reads :
// - https://www.fluentcpp.com/2017/05/16/what-the-crtp-brings-to-code/

// - - - - - - - - - -

// TODO // ROADMAP :

// 1 - prepare a code base for implementing ofxVPParams logic

// 2 - implement the most simple connection/disconnection (number cables?, we'll do that starting implementing HasUID, ParamAbstract and Parameter classes)

// 3 - implement all the other classes from the VPParams scheme you did, one by one

// 4 - implement more complex connections

// 5 - implement the load/save mechanism

// 6 - test it

// 7 - clean the code

// 8 - integrate in Mosaic/ofxVisualProgramming

// Perfectioning :
// - const correctness
// - private / protected / public
// - mark some members mutable ? (mutables can change when the [self/parent] instance is marked const)

// - - - - - - - - - -

#pragma once
#ifndef MOSAIC_PARAMETERS
#define MOSAIC_PARAMETERS

#include <iostream>
#include <list>

#include "ofxVPHasUid.h"
#include "imgui_stdlib.h"

// - - - - - - - - - -

// Define link types.
// Todo: make this a factory.
// all possible links
enum LinkType /*: unsigned char*/ { // unsigned char has a smaller footprint
    VP_LINK_UNDEFINED = 777, // NULLPTR / UNDEFINED_TYPE equivalence ??
    VP_LINK_NUMERIC = 0,
    VP_LINK_STRING = 1,
    VP_LINK_ARRAY = 2,
    VP_LINK_TEXTURE = 3,
    VP_LINK_AUDIO = 4,
    VP_LINK_SPECIAL = 5,
    VP_LINK_PIXELS = 6
};
// Convert enum to string
inline const char* ToString(const LinkType& v){
    switch (v){
        case VP_LINK_NUMERIC:       return "VP_LINK_NUMERIC";
        case VP_LINK_STRING:        return "VP_LINK_STRING";
        case VP_LINK_ARRAY:         return "VP_LINK_ARRAY";
        case VP_LINK_TEXTURE:       return "VP_LINK_TEXTURE";
        case VP_LINK_AUDIO:         return "VP_LINK_AUDIO";
        case VP_LINK_SPECIAL:       return "VP_LINK_SPECIAL";
        case VP_LINK_PIXELS:        return "VP_LINK_PIXELS";
        default:                    return "VP_LINK_UNDEFINED";
    }
};

// compile-time utility for converting type-values to assiociated linktype
// maybe ?
#define GET_LINKTYPE(X) _Generic( (X), \
    long double: VP_LINK_NUMERIC, \
    float: VP_LINK_NUMERIC, \
    int: VP_LINK_NUMERIC, \
    std::string : VP_LINK_STRING, \
    char[]: VP_LINK_STRING, \
    array: VP_LINK_ARRAY, \
    ofTexture: VP_LINK_TEXTURE, \
    ofSoundBuffer: VP_LINK_SPECIAL, \
    ofAudioBuffer: VP_LINK_AUDIO, \
    ofPixels: VP_LINK_PIXELS, \
    default: VP_LINK_UNDEFINED  \
)(X)
//struct LinkTypes {

//};
template<typename DATA_TYPE>
inline LinkType getLinkType() {
    return VP_LINK_UNDEFINED;
};
template<typename DATA_TYPE>
inline std::string getLinkName() {
    return "VP_LINK_UNDEFINED";
};

// - - - - - - - - - -
// To move to an included file ?
enum VPErrorCode_ {
    VPErrorCode_UNDEFINED = 0,
    VPErrorCode_LINK = 1,
    VPErrorCode_MODIFIER = 2,
};
enum VPErrorStatus_ {
    VPErrorStatus_UNDEFINED = 0,
    VPErrorStatus_NOTICE = 1,
    VPErrorStatus_WARNING = 2,
    VPErrorStatus_CRITICAL = 3,
};
struct VPError {
public:
    VPError( enum VPErrorCode_ _code=VPErrorCode_UNDEFINED, const VPErrorStatus_ _status=VPErrorStatus_UNDEFINED, std::string _message = "Unknown error.") : message(_message), code(_code), status(_status) {

    };
    const std::string message;
    const enum VPErrorCode_ code;
    const enum VPErrorStatus_ status;
    friend std::ostream & operator << (std::ostream& _out, const VPError& _e);
};
std::ostream& operator << (std::ostream& _out, const VPError& _e);
// - - - - - - - - - -

class AbstractHasInlet;
class AbstractHasOutlet;

class AbstractPinLink : public ofxVPHasUID { // todo: does this need an UID ?
public:
    AbstractPinLink( AbstractHasInlet& _parent ) : ofxVPHasUID("PinLink"), toPin(_parent) {

    };
    virtual ~AbstractPinLink(){};

    bool isConnected = false;
    //bool isEnabled; // needed ??
    AbstractHasOutlet* fromPin = nullptr;
    const AbstractHasInlet& toPin;
};

// - - - - - - - - - -

// A Parameter can be seen as an augmented data storage.
// Augmented: an (inlet) modifier mechanism, gui visualizations/controls, load/save/restore.
// Optional: They CAN be extended with data outlets.
class AbstractHasModifier;
class AbstractParameter : public ofxVPHasUID {
public:
    //AbstractParameter() : ofxVPHasUID("AbstractParameter") {};
    AbstractParameter( std::string _paramName = "AbstractParameter") : ofxVPHasUID(_paramName){
        // Remember / Register
        allParams.push_back(this);
        //std::cout << "New Abstract Param = " << this << std::endl;
    };

    virtual ~AbstractParameter(){
        // de-register param
        std::vector<AbstractParameter*>::iterator exists = std::find(allParams.begin(), allParams.end(), this );
        if( exists != allParams.end() ){
            allParams.erase(exists);
        }
    };

    // API methods
    virtual bool saveToXML() const = 0;
    virtual bool loadFromXML() = 0;
    virtual std::string serialize() = 0;
    virtual bool unserialize() = 0;
    //virtual void update() = 0; // needed ?
    virtual void drawImGuiEditable() = 0;
    virtual void drawImGui() = 0; // needed ?
    // these 2 prevent the need of virtual inheritance, which keeps things easier
    virtual AbstractHasModifier& getAsHasModifier() = 0;
    virtual bool hasOutlet() const = 0;
    virtual AbstractHasOutlet& getAsOutlet() = 0;
    //virtual void tmpTestFunc() = 0;

    const bool& dataHasChanged() const {
        return bFlagDataChanged;
    }
    bool bFlagDataChanged = false;

    // conversion
    AbstractParameter& getAsAbstract(){ return *this; };

    // Factory (tmp in here as a static)
    static const std::vector<AbstractParameter*>& getAllParams() { return allParams; };//const_cast< std::vector< AbstractParam* > >(allParams); };
protected:
    static std::vector<AbstractParameter*> allParams;

};

// - - - - - - - - - -
// Ensures an interface between outlets and inlets (which inherit from :haspin)
class HasPin {
public:
    HasPin(LinkType _linkType = VP_LINK_UNDEFINED) : linkType(_linkType) {
        //std::cout << "New Pin : " << ToString(_linkType) << " - " << std::endl;
    };
    virtual ~HasPin(){};

    // Pin event listeners
    virtual void onPinConnected()=0; // todo : add the link as argument here ?
    virtual void onPinDisconnected()=0; // todo : add the link as argument here ?

    ImVec2 pinPosition;
    const LinkType linkType;
    //ofxVPHasUID::stringKeyType owningObjectID; // needed ?

private :
    // This is an abstract you have to implement to detach pin data
    //virtual void detach() = 0;
};

// - - - - - - - - - -
template<typename ACCEPT_TYPE> class HasInlet;
template<typename ACCEPT_TYPE> class HasOutlet;
class AbstractHasInlet;
template<typename T>
class PinLink : public AbstractPinLink {
public:
    PinLink( HasInlet<T>& _parent ) : AbstractPinLink( _parent ), toPin(_parent){

    };
    virtual ~PinLink(){};

    HasOutlet<T>* fromPin = nullptr;
    const HasInlet<T>& toPin;
};


// - - - - - - - - - -
class AbstractHasInlet : public HasPin {
public:
    AbstractHasInlet( LinkType _linkType = VP_LINK_UNDEFINED ) : HasPin(_linkType) {};

    virtual bool acceptsLinkType( const LinkType& _linktype ) = 0;
    //virtual accept();
    virtual bool connectWithOutlet( AbstractHasOutlet& _outlet ) = 0; // return bool or PinLink&  ?
    virtual bool disconnectPin() = 0;

    // Events
    virtual void onPinConnected() override = 0;
    virtual void onPinDisconnected() override = 0;
    virtual void triggerValueChanged() = 0;

    std::shared_ptr<AbstractPinLink> connectedLink;
};

template<typename ACCEPT_TYPE>
class HasInlet : public AbstractHasInlet {
public:
    HasInlet() : AbstractHasInlet( getLinkType< ACCEPT_TYPE >() ) {};
    virtual ~HasInlet(){

    };
};

template<typename T> class ParamModifier;
template<typename T> class ParamInletModifier;

class AbstractHasOutlet : public HasPin {
public:
    AbstractHasOutlet( LinkType _linkType = VP_LINK_UNDEFINED ) : HasPin(_linkType) {};

    template<typename MODIFIER_TYPE>
    MODIFIER_TYPE& getTypedOutlet() {
        // Ensure correct usage by allowing only deriveds of AbstractHasOutlet
        static_assert(std::is_base_of<AbstractHasOutlet, MODIFIER_TYPE>::value, "MODIFIER_TYPE should inherit from AbstractHasOutlet* !!!");

        try {
            MODIFIER_TYPE* typedOutlet = dynamic_cast<MODIFIER_TYPE*>(this);
            if( !typedOutlet || typedOutlet==nullptr ){
                throw 888;
            };
            return *typedOutlet;
        } catch (...) {
            std::cout << "Catched WRONG TYPE CONVERSION! (normal behaviour)" << std::endl;
            throw 777;
        }
    };

    // todo
    //std::vector<AbstractPinLink*> pinLinks;

    //const std::string dataLabel;
    //const LinkType linkType;
    virtual void visualiseData() = 0;
    bool bFlagDataChanged;
};

template<typename ACCEPT_TYPE>
class HasOutlet : public AbstractHasOutlet {
public:
    HasOutlet() : AbstractHasOutlet( getLinkType< ACCEPT_TYPE >() ) {};
    // todo
    // store dataref ?
//    HasOutlet<ACCEPT_TYPE>& getAsOutlet(){
//        return *this;
//    };

    // todo: un-virtual this by assigning a constant reference to HasOutlet's data value (?)
    virtual const ACCEPT_TYPE& getOutputValue() = 0;
};

enum OFXVP_MODIFIER_ {
    OFXVP_MODIFIER_UNKNOWN = 0,
    OFXVP_MODIFIER_INLET = 1,
    OFXVP_MODIFIER_SCRIPTING = 2
};

class abstractParamModifier {
protected:
    abstractParamModifier( const AbstractHasModifier& _parent, const enum OFXVP_MODIFIER_& _modifierType ) : parent(_parent), modifierType(_modifierType) {

    }
public:
    virtual bool isEditable() const = 0;
    const AbstractHasModifier& parent;
    const enum OFXVP_MODIFIER_ modifierType;// = OFXVP_MODIFIER_UNKNOWN;
};

class AbstractHasModifier { // todo: rename to AbstractHasModifiers
public:
    virtual ~AbstractHasModifier(){};
    //virtual bool addModifier() = 0;
    const AbstractHasModifier& getAbstractModifier() {
        return static_cast<const AbstractHasModifier&>(*this);
    };

    // ABSTRACT API METHODS
    template<typename MODIFIER_TYPE>
    MODIFIER_TYPE& getOrCreateModifier_deletethisfunc() { // todo: Whole function to be removed ?
        // Ensure correct usage by allowing only deriveds of abstractParamModifier
        static_assert(std::is_base_of<abstractParamModifier, MODIFIER_TYPE>::value, "MODIFIER_TYPE should inherit from abstractParamModifier* !!!");

        // search existing ones
        for(abstractParamModifier* apm : abstractParamModifiers){
            if( apm->modifierType == MODIFIER_TYPE::modifierType ){// OFXVP_MODIFIER_INLET ){
                try {
                    return static_cast<MODIFIER_TYPE>(*apm);
                } catch (...) {
                    throw VPError(VPErrorCode_MODIFIER, VPErrorStatus_WARNING, "Could not cast from abstractModifier to the desired typed modifier.");
                    break;
                }
            }
        }

        // nothing found ? Create new modifier
//        auto* newModifierInstance = new MODIFIER_TYPE( *this );
//        abstractParamModifiers.push_back( newModifierInstance );
//        return *newModifierInstance;
        // From here, we cannot push to paramModifiers, only to abstractParamModifiers, which will lose information.
        throw VPError( VPErrorCode_MODIFIER, VPErrorStatus_NOTICE, "The modifier was not found and can not yet be added." );
    };
    template<typename MODIFIER_TYPE>
    bool hasModifier() const {
        // Ensure correct usage by allowing only deriveds of abstractParamModifier
        static_assert(std::is_base_of<abstractParamModifier, MODIFIER_TYPE>::value, "MODIFIER_TYPE should inherit from abstractParamModifier* !!!");

        //if( std::is_base_of<ParamInletModifier, MODIFIER_TYPE>::value ){
            for(abstractParamModifier* apm : abstractParamModifiers){
                if( apm->modifierType == MODIFIER_TYPE::modifierType ){
                    return true;
                    break;
                }
            }
            return false;
        //}
    };

    int getNumModifiers() const {
        return abstractParamModifiers.size();
    };

    // TYPED API METHODS
    virtual bool connectWithOutlet( AbstractHasOutlet& _outlet ) = 0; // to return bool or PinLink&  ?

    std::list<abstractParamModifier*> abstractParamModifiers;
};

template<typename ACCEPT_TYPE>
class HasModifier : public AbstractHasModifier {
public:
    //HasModifier() : AbstractHasModifier() {};
    virtual ~HasModifier(){
        for(ParamModifier<ACCEPT_TYPE>* modifier : paramModifiers){
            delete modifier;
        }
        paramModifiers.clear();
        abstractParamModifiers.clear();
    };
    virtual bool addModifier() /*override*/ { // todo : return bool ?
        // tmp, only inletmodifiers
        const AbstractHasModifier& tmp = this->getAbstractModifier();
        paramModifiers.push_back( new ParamInletModifier<ACCEPT_TYPE>( tmp ) );
        abstractParamModifiers.push_back( paramModifiers.back() ); // always keep abstracts synced
        return true;
    };
    virtual bool connectWithOutlet( AbstractHasOutlet& _outlet ) override {
        //std::cout << "HasModifier<T=" << getLinkName<ACCEPT_TYPE>() << ">::connectWithOutlet() --> connecting parameter" << std::endl;

        // pre-check if potentialy compatible. todo
        // Don't connect with self !
//        if( !acceptsLinkType( _outlet.linkType ) ){
//        if( ACCEPT_TYPE != _outlet.linkType ){
//            std::cout << "Parameter<T>::connectWithOutlet() --> incompatible link type !" << std::endl;
//            return false;
//        }

        // get inlet modifier
        ParamInletModifier<ACCEPT_TYPE>& inletModifier = this->getOrCreateModifier< ParamInletModifier<ACCEPT_TYPE> >();
        bool ret = inletModifier.connectWithOutlet(_outlet);

        //std::cout << "Parameter<T>::HasModifier::connectWithOutlet() --> succeeded = " << ret << std::endl;
        return ret;
    };

    template<typename MODIFIER_TYPE>
    MODIFIER_TYPE& getOrCreateModifier() {
        // Ensure correct usage by allowing only deriveds of abstractParamModifier
        static_assert(std::is_base_of<abstractParamModifier, MODIFIER_TYPE>::value, "MODIFIER_TYPE should inherit from abstractParamModifier* !!!");
        // Warning for uncompletely-implemented modifiers
        static_assert( ( MODIFIER_TYPE::modifierType ), "This modifier has no modifierType implemented. Please add one to yours.");

        // Search for any existing one
        for(ParamModifier<ACCEPT_TYPE>* pm : paramModifiers){
            if( pm->modifierType == MODIFIER_TYPE::modifierType ){
                return static_cast< MODIFIER_TYPE& >(*pm);
            }
        }

        // nothing found ? Create new modifier
        auto* newModifierInstance = new MODIFIER_TYPE( *this );
        paramModifiers.push_back( newModifierInstance );
        abstractParamModifiers.push_back( newModifierInstance );
        //std::cout << "Created a brand new modifier !" << std::endl;
        return *newModifierInstance;
    };

protected:
    std::list<ParamModifier<ACCEPT_TYPE>*> paramModifiers; // stores ptr but owns them. Be sure to clean memory!
};

template<typename DATA_TYPE, bool ENABLE_OUTLET=true>
//class Parameter : public HasInlet<DATA_TYPE>, public AbstractParameter {
class Parameter : public HasModifier<DATA_TYPE>, public std::conditional<ENABLE_OUTLET, HasOutlet<DATA_TYPE>, void>::type, public AbstractParameter {
public:

    Parameter( std::string _paramName = "Parameter", DATA_TYPE _value = DATA_TYPE() ) : AbstractParameter(_paramName), dataValue(_value) {
//# ifdef OFXVP_DEBUG_PARAMS
//        ofLogNotice(std::string(OFXVP_DEBUG_PARAMS_PREPEND)+"_primary") <<  __PRETTY_FUNCTION__;
//# endif
        //std::cout << "After " << this->getUID() << " (done)" << std::endl;
    };
    using HasModifier<DATA_TYPE>::paramModifiers;

    // - - - - - - - - - -
    // Abstract functions
    // From HasModifier<DATA_TYPE>
    // virtual void addModifier() override {};

    // - - - - - - - - - -
    // From HasOutlet
    virtual typename std::enable_if<ENABLE_OUTLET>::type
    visualiseData() override {};
    virtual void onPinConnected() override {};
    virtual void onPinDisconnected() override {};
    //virtual void triggerValueChanged() override {};
//    if ( ENABLE_OUTLET ) {
//        return true;
//    }
//    else {
//        return false;
//    }
    virtual const DATA_TYPE& getOutputValue() override final {
        //std::cout << "getOutputValue() [from]=" << &this->getBaseValue() << " [to]=" << &this->getValue() << std::endl;
        return this->getValue();
    };

    // From AbstractParameter
    bool saveToXML() const override {
        return false; // todo
    };
    bool loadFromXML() override {
        return false; // todo
    };
    std::string serialize() override {
        return ""; // todo
    };
    bool unserialize() override {
        return false; // todo
    };
    //void update() = 0; // needed ?
    void drawImGuiEditable() override {
        // Default falls back to uneditable method
        this->drawImGui();
    };
    void drawImGui() override {
        // Fallback behaviour for any type : Try to display without editing capabilities.
        std::ostringstream serialized;
        serialized << this->dataValue; // &this->dataValue; --> displays variable address
        if(serialized.tellp()>0){ // tellp = size()
            // Echo serialized value
            ImGui::LabelText( this->getDisplayName().c_str(), "%s", serialized.str().c_str() );
        }
        else {
            // echo variable type
            ImGui::LabelText( this->getDisplayName().c_str(), "%s (unable to display)", typeid(*this).name() );
        }
    };
    bool isImGuiEditable() { // to be virtual ?
        if( paramModifiers.size() > 0 ){
            return this->paramModifiers.back()->isEditable();
        }
        return true;
    };
    virtual AbstractHasModifier& getAsHasModifier() override {
        return *this;
    };
    virtual bool hasOutlet() const override final {
        return ENABLE_OUTLET;
    };
    // todo: enable only if : virtual typename std::enable_if<ENABLE_OUTLET>::type
    virtual AbstractHasOutlet& getAsOutlet() override final {
        if( this->hasOutlet() ){
            //return *((AbstractHasOutlet*) this);
            return (AbstractHasOutlet&) *this;
        }
        else {
            // impossible to parse non-outlet param to outlet param.
            throw VPError(VPErrorCode_LINK, VPErrorStatus_NOTICE, std::string("Cannot get outlet of parameter «") + this->getUID() + "» : it hasn't got an outlet !");
        }
    };

    // Non abstract API functions (typed)

    // Returns baseValue() which gets the constant parameter value from derived classes.
    // Eventually replace that reference with a modified one, if modifiers exist. (variable routing function)
    const DATA_TYPE& getValue() const {
        // todo: Check/Fix references, allow to re-assign references ? References should be re-assignable, but point to const values so they cannot be messed with.
        // Ressource: https://isocpp.org/wiki/faq/references#reseating-refs
        switch(this->paramModifiers.size()) {
            case 0: {
                // return base
                return this->getBaseValue();
                break;
            }
            case 1: {
                return this->paramModifiers.back()->transformValue( this->getBaseValue() );
                break;
            }
            default: {
                // chain modifiers
                const DATA_TYPE* retValue = &this->getBaseValue();// const_cast<const DATA_TYPE*>(&this->getBaseValue());
                for(ParamModifier<DATA_TYPE>* modifier : this->paramModifiers){
                    retValue = &modifier->transformValue(*retValue);
                }
                return *retValue;
                break;
            }
        }
        return dataValue;
    };
    DATA_TYPE getValueCopy() const {
        return dataValue;
    };
    virtual const DATA_TYPE& getBaseValue() const { // base value
        return dataValue;
    }
    bool setBaseValue( const DATA_TYPE& _newValue){
        dataValue = _newValue;
        return true;
    };
    // Experimental: alternative method to getValue() the value dereferencing the pointer
    const DATA_TYPE& operator->(){
        return getValue();
    }

    // Utilities
    // Cast Parameter<type> to type (assignment operator)
    const DATA_TYPE& operator=(const Parameter<DATA_TYPE, ENABLE_OUTLET>& _param) const{
        return getValue();
    }
    // Move assignment operator
//    DATA_TYPE& operator=(const Parameter<DATA_TYPE,ENABLE_OUTLET>&& _param){
//        return getValue();
//    }
    // implicit conversion operator
    operator const DATA_TYPE&() { return getValue(); }

    // todo: forbid copy constructor to prevent creating accidental copies and only allow refs ?

    // ImGui Helpers
protected:
    void ImGuiListenForParamDrop(){
        if ( ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VPPARAM_OUTLET_TO_INLET")){
                //IM_ASSERT(payload->DataSize == sizeof(ofxVPHasUID::stringKeyType::value_type*));
                ofxVPHasUID::stringKeyType outletParmName = static_cast<ofxVPHasUID::stringKeyType::value_type*>(payload->Data);//static_cast<std::string*>((*data));

                // try to connect
                if( ofxVPHasUID* outletHasUID = ofxVPHasUID::getInstanceByUID(outletParmName) ){
                    // Don't connect with self !
                    if( static_cast<ofxVPHasUID*>(this) == outletHasUID ){
                        ofLogNotice("Parameter") << "Notice: Not connecting the parameter with itself !" << std::endl;
                    }
                    else {
                        // try to parse it as a parameter
                        if( AbstractParameter* outletAbstract = dynamic_cast<AbstractParameter*>(outletHasUID)){
                            if(outletAbstract->hasOutlet()){
                                try {
                                    if( this->getAsHasModifier().connectWithOutlet( outletAbstract->getAsOutlet() ) ){
                                        // Connection done ! :)
                                    }
                                    else {
                                        ofLogNotice("Parameter") << "The inlet param " << this->getUID() << " (me) did not accept a connection with "<< outletParmName << ". Matbye it doesn't accept my data type ?" << std::endl;
                                    }
                                } catch (...) {
                                    ofLogNotice("Parameter") << "Could not parse outlet " << outletParmName << " as an outlet !" << std::endl;
                                }
                            }
                        }
                        else {
                            ofLogNotice("Parameter") << "Could not parse " << outletParmName << " to a parameter !" << std::endl;
                        }
                    }
                }
                else {
                    std::cout << "Cannot find an instance named: " << outletParmName << "." << std::endl;
                }
                std::cout << "Dropped target on source(outlet)=" << outletParmName << " --> target(inlet)=" << this->getUID() << std::endl;
            } // end VPPARAM_OUTLET_TO_INLET

            // Handle dropping a parameter connection request
            else if ( ENABLE_OUTLET ){
                if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("VPPARAM_INLET_TO_OUTLET")){
                    //IM_ASSERT(payload->DataSize == sizeof(ofxVPHasUID::stringKeyType::value_type*));
                    ofxVPHasUID::stringKeyType inletParmName = static_cast<ofxVPHasUID::stringKeyType::value_type*>(payload->Data);//static_cast<std::string*>((*data));

                    // try to connect
                    if( ofxVPHasUID* inletHasUID = ofxVPHasUID::getInstanceByUID(inletParmName) ){
                        // Don't connect with self !
                        if( static_cast<ofxVPHasUID*>(this) == inletHasUID ){
                            ofLogNotice("Parameter") << "Notice: Not connecting the parameter with itself !" << std::endl;
                        }
                        else {
                            // try to parse it as a parameter
                            if( AbstractParameter* inletAbstract = dynamic_cast<AbstractParameter*>(inletHasUID)){
                                try {
                                    if( inletAbstract->getAsHasModifier().connectWithOutlet( this->getAsOutlet() ) ){
                                        // Connection done ! :)
                                    }
                                    else {
                                        ofLogNotice("Parameter") << "The param inlet " << inletParmName << " did not accept a connection with me. Matbye it doesn't accept my data type ?" << std::endl;
                                    }
                                } catch (...) {
                                    ofLogNotice("Parameter") << "The param outlet " << this->getUID() << "(me) is not parsable as an outlet !" << std::endl;
                                }
                            }
                            else {
                                ofLogNotice("Parameter") << "Could not parse " << inletParmName << " to a parameter !" << std::endl;
                            }
                        }
                    }
                    else {
                        std::cout << "Cannot find an instance named :" << inletParmName << "." << std::endl;
                    }
                    std::cout << "Dropped target on source(outlet)=" << this->getUID() << " --> target(inlet)=" << inletParmName << std::endl;
                }
            } // end VPPARAM_INLET_TO_OUTLET

            ImGui::EndDragDropTarget();
        }
    };
    void ImGuiDrawParamConnector(const bool& _drawInlet=false) const {
        // Show source connect handle
        if( ENABLE_OUTLET ){ //this->hasOutlet() ){
            if(!_drawInlet) ImGui::SameLine();
            ImGui::Button(_drawInlet?"<--":"-->");
            if( ImGui::BeginDragDropSource(ImGuiDragDropFlags_None) ){
                //ImGui::SetDragDropPayload("TEST", ((std::string::value_type**) this->getUID().c_str() ), sizeof(std::string::value_type*), ImGuiCond_Once);    // Set payload to carry the index of our item (could be anything)
                const ofxVPHasUID::stringKeyType& UID = this->getUID();
                ImGui::SetDragDropPayload(_drawInlet?"VPPARAM_INLET_TO_OUTLET":"VPPARAM_OUTLET_TO_INLET", UID.data(), UID.size()+1, ImGuiCond_Once);    // Set payload to carry the index of our item (could be anything)
                ImGui::Text(_drawInlet?"Connect %s (inlet) with an outlet...":"Connect %s (outlet) with an inlet...", UID.c_str());
                ImGui::EndDragDropSource();
            }
            if(_drawInlet) ImGui::SameLine();
        }
    }

    // tmp method ?
    void ImGuiShowInfoMenu(){
        if( ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered() ) {
            isMenuOpen = !isMenuOpen;
        }
        if( isMenuOpen ){
            this->ImGuiPrintParameterInfo();
        }
    }

    void ImGuiPrintParameterInfo(){
        ImGui::TextUnformatted( this->getUID().c_str() );

        if( this->hasOutlet() ){
            try {
                HasOutlet<DATA_TYPE>& outlet = *this;//->getAsOutlet();
                ImGui::Text("Outlet information" );
                //ImGui::Text("Connected Inlets : %s", outlet );
            } catch(...) {
                ImGui::LabelText( "InletModifier", "%s", "Error/Unavailable" );
            }
        }
        else {
            ImGui::Text("No Outlet on this param" );
        }

        ImGui::Text("Modifiers information" );
        ImGui::LabelText("Num Modifiers", "%i", this->getNumModifiers() );
        try {
            if( this->template hasModifier< ParamInletModifier< DATA_TYPE > >() ){
                ParamInletModifier< DATA_TYPE >& paramInlet = const_cast< Parameter<DATA_TYPE,ENABLE_OUTLET>& >( *this ).template getOrCreateModifier< ParamInletModifier<DATA_TYPE> >();
                ImGui::LabelText( "InletModifier", "Connected: %s", paramInlet.myLink.isConnected?"yes":"no" );
                for(ParamModifier<DATA_TYPE>* modifier : paramModifiers){
                    //ImGui::Text("ModifierType: %i", modifier->modifierType);
                }
            }
            else {
                 ImGui::LabelText( "InletModifier", "%s", "None" );
            }
        } catch(...) {
            ImGui::LabelText( "InletModifier", "%s", "Error/Unavailable" );
        }
        ImGui::Separator();
        std::ostringstream storedValue;     std::ostringstream outputValue;     std::ostringstream baseValue;
        storedValue << this->dataValue;     outputValue << this->getValue();    baseValue << this->getBaseValue();
        ImGui::LabelText("storedValue", "%s",   storedValue.str().c_str() );
        ImGui::LabelText("baseValue", "%s",     baseValue.str().c_str());
        ImGui::LabelText("outputValue", "%s",   outputValue.str().c_str() );
    }

public:// private:
    DATA_TYPE dataValue; // base value
    DATA_TYPE storedValue; // for save/load
private:
     bool isMenuOpen = false;
};


// - - - - - - - - - -

template<typename T>
class ParamModifier : public abstractParamModifier {
public:
    ParamModifier(  const AbstractHasModifier& _parent, const enum OFXVP_MODIFIER_& _modifierType  ) : abstractParamModifier( _parent, _modifierType ){}
    virtual ~ParamModifier(){};
    virtual const inline T& transformValue(const T& _originalValue) const = 0;
};

template<typename MODIFIER_TYPE>
class ParamInletModifier : public ParamModifier<MODIFIER_TYPE>, public HasInlet<MODIFIER_TYPE> {
// todo : make constructor private to force deriving this class ?
public:
    ParamInletModifier( const AbstractHasModifier& _parent ) : ParamModifier<MODIFIER_TYPE>( _parent, OFXVP_MODIFIER_INLET /*getLinkName<MODIFIER_TYPE>() +" (Inlet)"*/), HasInlet<MODIFIER_TYPE>(), myLink(*this) {

    }
    virtual ~ParamInletModifier(){};

    // Note: inline is needed so it can be defined once later in the code. Defining it here lead to duplicated symbols linker errors.
    // Todo : move it to .cpp file ?
    virtual const inline MODIFIER_TYPE& transformValue(const MODIFIER_TYPE& _originalValue) const override {
        //std::cout << "ParamModifier<T>::transformValue() UNTYPED !!!!" << std::endl;
        if( myLink.isConnected ){
            return myLink.fromPin->getOutputValue();
        }

        // Or leave the value untouched
        return _originalValue;
    }

    virtual bool isEditable() const override {
        return !myLink.isConnected;
    }

    // - - - - - - - - - -
    // Virtuals from HasInlet
    bool acceptsLinkType( const LinkType& _linktype ) override {
        return this->AbstractHasInlet::linkType == _linktype;
        // todo : make this work for similar link types ? ( ex: int/float/double = numeric)
    };
    virtual bool connectWithOutlet( AbstractHasOutlet& _outlet ) override {
        //std::cout << "Modifier<T>::connectWithOutlet() --> connecting parameter" << std::endl;
        if( !acceptsLinkType( _outlet.linkType ) ){
            std::cout << "Modifier<T>::connectWithOutlet() --> incompatible link type !" << std::endl;
            return false;
        }

        // Disconnect (if connected)
        if(!disconnectPin()) return false;

        // Connect
        try {
            myLink.fromPin = &_outlet.getTypedOutlet< HasOutlet<MODIFIER_TYPE> >();
        } catch(...){
            std::cout << "Cannot get typed outlet, probably this is an uncompatible abstract, or it has no outlet." << std::endl;
            //disconnectPin();
            return false;
        }

        myLink.isConnected = true;

        // Trigger events
        myLink.fromPin->onPinConnected();

        //std::cout << "Modifier<T>::connectWithOutlet() succeeded ! " << myLink.fromPin->linkType << " --> " << myLink.toPin.linkType << std::endl;
        return true;
    };
    virtual bool disconnectPin() override { // maybe : return void, always succeed to disconnect ?
        if(myLink.isConnected){
            // notify outlet
            myLink.fromPin->onPinDisconnected();
            myLink.fromPin = nullptr;
            myLink.isConnected = false;
        }
        return true;
    }
    virtual void onPinConnected() override {
        // todo
    };
    virtual void onPinDisconnected() override {};
    virtual void triggerValueChanged() override {};

    // - - - - - - - - - -
    //mutable bool isConnected;
    PinLink<MODIFIER_TYPE> myLink;

    const static enum OFXVP_MODIFIER_ modifierType = OFXVP_MODIFIER_INLET;
};

#endif
