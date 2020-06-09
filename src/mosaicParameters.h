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
// enum allLinkTypes;

class AbstractParameter;
class AbstractHasInlet;

class PinLink {
    bool isConnected;
    //bool isEnabled; needed ??
    AbstractParameter* fromPin;
    AbstractHasInlet* toPin;
};


// - - - - - - - - - -

// A Parameter can be seen as an augmented data storage.
// Augmented: an (inlet) modifier mechanism, gui visualizations/controls, load/save/restore.
// Optional: They CAN be extended with data outlets.
class AbstractParameter : public ofxVPHasUID {
public:
    //AbstractParameter() : ofxVPHasUID("AbstractParameter") {};
    AbstractParameter( std::string _paramName = "AbstractParameter") : ofxVPHasUID(_paramName){};

    virtual ~AbstractParameter(){};// = 0;

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

    // conversion
    AbstractParameter& getAsAbstract(){ return *this; };

protected:
    bool bFlagDataChanged = false;
};

// - - - - - - - - - -

class AbstractHasInlet {
public:

    virtual bool acceptsLinkType() = 0;
    //virtual accept();
    virtual bool connectWith() = 0;

};

template<typename ACCEPT_TYPE>
class HasInlet : public AbstractHasInlet {
public:
    // todo
};

class AbstractHasOutlet {
public:
    // todo
};

template<typename ACCEPT_TYPE>
class HasOutlet : public AbstractHasOutlet {
public:
    // todo
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

    // Abstract functions
    // From HasInlet
    bool acceptsLinkType() override {
        return false; // todo
    };
    bool connectWith() override{
        if ( ENABLE_OUTLET ) {
            return true; // todo
        }
        return false;
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
    DATA_TYPE getValue() const;
    bool setValue(DATA_TYPE _newValue);

    // todo: forbid copy constructor to prevent creating accidental copies ?

// private:
    std::list<PinLink*> links;
    DATA_TYPE dataValue;
};
