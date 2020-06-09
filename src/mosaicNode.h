#pragma once

#include "ofMain.h"

#include "ofxImGui.h"
#include "imgui_stdlib.h"

#include "mosaicParameters.h"

#define MAX_PARAMS 36

enum LINK_TYPE {
    VP_LINK_NUMERIC,
    VP_LINK_STRING,
    VP_LINK_ARRAY,
    VP_LINK_TEXTURE,
    VP_LINK_AUDIO,
    VP_LINK_SPECIAL,
    VP_LINK_PIXELS
};

struct PatchLink{
    int                     id;
    int                     type;
    int                     fromOutletID;
    int                     toObjectID;
    int                     toInletID;
};

class mosaicNode {

public:

    mosaicNode() {
        mosaicNode(-1);
    }
    mosaicNode(int _id) : myFloatParam("myStringName"), myStringParam("myStringName"), myIntParam("myIntName") {
        this->_id = _id;
    }

    void setup();
    void update(std::map<int,std::shared_ptr<mosaicNode>> &nodes);
    void draw();

    bool connectFrom(std::map<int,std::shared_ptr<mosaicNode>> &nodes, int fromObjectID, int fromOutlet, int toInlet, int linkType);
    void disconnectFrom(std::map<int,std::shared_ptr<mosaicNode>> &nodes, int objectInlet);
    void disconnectLink(std::map<int,std::shared_ptr<mosaicNode>> &nodes, int linkID);


    // ID
    int                                             _id;

    // Links
    std::vector<std::shared_ptr<PatchLink>>         linksTo;

    // Params
    void*                                           _inletParams[MAX_PARAMS];
    void*                                           _outletParams[MAX_PARAMS];

    int                                             numInlets;
    int                                             numOutlets;


    // Empty params content
    std::vector<float>                              *empty;
    ofImage                                         *kuro;


    // Parameters
    //std::vector<std::shared_ptr<AbstractParameter> >parameters; // I tried with make_shared... which is a garbage collector, which in fact we don't really need as Parameters will ensure instances' validity ?
    std::vector<AbstractParameter*> parameters;

    // Customise this node (to be in a derived mosaicNode)
    // Static Parameters
    Parameter<float> myFloatParam;
    Parameter<std::string> myStringParam;
    Parameter<int> myIntParam;
    std::vector< Parameter<int> > myDynamicParams;
};
