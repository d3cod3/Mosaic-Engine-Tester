#pragma once

#include "ofMain.h"

#include "ofxImGui.h"
#include "imgui_stdlib.h"

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

    mosaicNode() {}
    mosaicNode(int _id) { this->_id = _id; }

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


};
