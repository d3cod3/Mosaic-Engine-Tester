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

// 1 - prepare a code base for implementing ofxVPParams logic (i'm on it, i'm thinking in two imgui panels, one on the left simulating Mosaic outlets pins/params, and another on the right simulating Mosaic inlet pins/params)

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

#include <iostream>
#include <list>

#include "ofxVPHasUid.h"
#include "imgui_stdlib.h"

// - - - - - - - - - -

// Define link types.
// Todo: make this a factory.
// all possible links
enum LinkType /*: unsigned char*/ { // unsigned char has a smaller footprint
    //VP_LINK_UNDEFINED = 777, // NULLPTR / UNDEFINED_TYPE equivalence ??
    VP_LINK_NUMERIC = 0,
    VP_LINK_STRING = 1,
    VP_LINK_ARRAY = 2,
    VP_LINK_TEXTURE = 3,
    VP_LINK_AUDIO = 4,
    VP_LINK_SPECIAL = 5,
    VP_LINK_PIXELS = 6
};

// compile-time utility for converting type-values to assiociated linktype
// maybe ?
#define getLinkType(X) _Generic((X), \
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
    default: VP_LINK_NUMERIC  \
);

class AbstractHasInlet;
class AbstractHasOutlet;

class PinLink : ofxVPHasUID {
    PinLink( AbstractHasOutlet& _parent ) : ofxVPHasUID("PinLink"), fromPin(_parent) {

    };

    bool isConnected;
    //bool isEnabled; needed ??
    const AbstractHasOutlet& fromPin;
    AbstractHasInlet* toPin;
};


// - - - - - - - - - -

// A Parameter can be seen as an augmented data storage.
// Augmented: an (inlet) modifier mechanism, gui visualizations/controls, load/save/restore.
// Optional: They CAN be extended with data outlets.
class AbstractParameter : public ofxVPHasUID {
public:
    //AbstractParameter() : ofxVPHasUID("AbstractParameter") {};
    AbstractParameter( std::string _paramName = "AbstractParameter") : ofxVPHasUID(_paramName){
        // Remember / Register
        allParams.push_back(this);
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
    virtual void drawImGui() const = 0; // needed ?

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
    HasPin() {}
    virtual ~HasPin(){}

    // Pin event listeners
    virtual void onPinConnected()=0;
    virtual void onPinDisconnected()=0;

    ImVec2 pinPosition;
    //ofxVPHasUID::stringKeyType owningObjectID; // needed ?

private :
    // This is an abstract you have to implement to detach pin data
    //virtual void detach() = 0;
};

// - - - - - - - - - -

class AbstractHasInlet : public HasPin {
public:

    virtual bool acceptsLinkType() = 0;
    //virtual accept();
    virtual bool connectWithOutlet() = 0; // return bool or PinLink&  ?
    virtual bool disconnectPin() = 0;

    // Events
    virtual void onPinConnected() override = 0;
    virtual void onPinDisconnected() override = 0;
    virtual void triggerValueChanged() = 0;

    std::shared_ptr<PinLink> connectedLink;

};

template<typename T> class ParamModifier;
template<typename T> class ParamInletModifier;

template<typename ACCEPT_TYPE>
class HasInlet : public AbstractHasInlet {
public:
    HasInlet(){};
    virtual ~HasInlet(){
        for(ParamModifier<ACCEPT_TYPE>* modifier : paramModifiers){
            delete modifier;
        }
        paramModifiers.clear();
    };
    void addModifier(){
        // tmp, only inletmodifiers
        paramModifiers.push_back( new ParamInletModifier<ACCEPT_TYPE>() );
    }
protected:
    std::list<ParamModifier<ACCEPT_TYPE>*> paramModifiers; // stores ptr but owns them. Be sure to clean memory!
};

class AbstractHasOutlet : public HasPin {
public:
    // todo
    std::vector<PinLink> pinLinks;

    //const std::string dataLabel;
    //const LinkType linkType;
    virtual void visualiseData() = 0;
    bool bFlagDataChanged;
};

template<typename ACCEPT_TYPE>
class HasOutlet : public AbstractHasOutlet {
public:
    // todo
    // store dataref ?
};

template<typename DATA_TYPE, bool ENABLE_OUTLET=true>
//class Parameter : public HasInlet<DATA_TYPE>, public AbstractParameter {
class Parameter : public HasInlet<DATA_TYPE>, public std::conditional<ENABLE_OUTLET, HasOutlet<DATA_TYPE>, void>::type, public AbstractParameter {
public:
    Parameter( std::string _paramName = "Parameter" ) {
        Parameter(_paramName, DATA_TYPE() );
    };
    Parameter( std::string _paramName, DATA_TYPE _value ) : AbstractParameter(_paramName), dataValue(_value) {
//# ifdef OFXVP_DEBUG_PARAMS
//        ofLogNotice(std::string(OFXVP_DEBUG_PARAMS_PREPEND)+"_primary") <<  __PRETTY_FUNCTION__;
//# endif
    };
    using HasInlet<DATA_TYPE>::paramModifiers;

    // Abstract functions
    // From HasInlet
    bool acceptsLinkType() override {
        return false; // todo
    };
    bool connectWithOutlet() override {
        if ( ENABLE_OUTLET ) {
            return true; // todo
        }
        return false;
    };
    virtual bool disconnectPin() override {
        return true; // todo
    }
    virtual void onPinConnected() override {};
    virtual void onPinDisconnected() override {};
    virtual void triggerValueChanged() override {};

    // From HasOutlet
    virtual typename std::enable_if<ENABLE_OUTLET>::type
    visualiseData() override {};

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
    void drawImGui() const override {
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


    // Non abstract API functions (typed)
    // Maybe these need to be virtual methods ?
    const DATA_TYPE& getValue() const {
        // Returns baseValue() which gets the constant parameter value from derived classes.
        // Eventually replace that value with a modified one, if modifiers exist
        switch(this->paramModifiers.size()) {
            case 0: {
                // return base
                return this->baseValue();
                break;
            }
            case 1: {
                //std::cout << "ModifierStackSize=" << paramModifiers.size() << std::endl;
                return this->paramModifiers.back()->transformValue( this->baseValue() );
                break;
            }
            default: {
                // chain modifiers. Todo: define correct chaining order.
                // Const_cast is temp hack until const-correctness is OK. This is UNDEFINED BEHAVIOUR, non standard !
                // but as the function returns it as a const ref again, it should work.
                // Only because we cannot re-assign a const ref.
                DATA_TYPE& retValue = const_cast<DATA_TYPE&>(this->baseValue());
                //static int i; // static so it doesn't re-allocate on every getValue() call
                //i=0;
                for(ParamModifier<DATA_TYPE>* modifier : this->paramModifiers){
                    //if(i==0)
                    retValue = modifier->transformValue(retValue);
                    //i++;
                }
                return retValue;
                break;
            }
        }
        return dataValue;
    };
    DATA_TYPE getValueCopy() const {
        return dataValue;
    };
    virtual const DATA_TYPE& getBaseValue() const { // base (saved) value
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
    // Cast Parameter<type> to type.
    const DATA_TYPE& operator=(const Parameter<DATA_TYPE,ENABLE_OUTLET>& _param) const{
        return getValue();
    }
    // Move assignment operator
//    DATA_TYPE& operator=(const Parameter<DATA_TYPE,ENABLE_OUTLET>&& _param){
//        return getValue();
//    }
    // implicit conversion operator
    operator const DATA_TYPE&() { return getValue(); }


    // todo: forbid copy constructor to prevent creating accidental copies ?

// private:
    DATA_TYPE dataValue;
    DATA_TYPE storedValue; // for save/load
};

// - - - - - - - - - -
class abstractParamModifier  : public ofxVPHasUID {
//    abstractParamModifier() : ofxVPHasUID("ParamModifier") {
//    }
protected:
    abstractParamModifier(const std::string& _modifierName = "ParamModifier") : ofxVPHasUID(_modifierName) {

    }
    // Chaining ?
    //virtual void transformValue() = 0;
    //const AbstractParam& parent;
    //const std::string name;
};
template<typename T>
class ParamModifier : public abstractParamModifier {
public:
    ParamModifier(const std::string& _modifierName = "ParamModifier") : abstractParamModifier(_modifierName){}
    virtual ~ParamModifier(){};
    virtual const inline T& transformValue(const T& _prevValue) const = 0;
};

template<typename T>
class ParamInletModifier : public ParamModifier<T>, protected HasInlet<T> {
public:
    ParamInletModifier() : ParamModifier<T>("Pin Inlet"), HasInlet<T>() {

    }
    virtual ~ParamInletModifier(){};

    // Note: inline is needed so it can be defined once later in the code. Defining it here lead to duplicater symbols linker errors.
    virtual const inline T& transformValue(const T& _prevValue) const override {
        //std::cout << "ParamModifier<T>::transformValue() UNTYPED !!!!" << std::endl;
        return _prevValue;
    }
    //mutable bool isConnected;
    mutable PinLink* inlet;

    // Inherited
    virtual void triggerValueChanged() override {};
    virtual void onPinDisconnected() override {};
    virtual void onPinConnected() override {};
};
