#include "mosaicNode.h"

//--------------------------------------------------------------
void mosaicNode::setup(){

    // fixed node content to one inlet/outlet/param for each type

    // VP_LINK_NUMERIC
    _inletParams[0] = new float();
    *(float *)&_inletParams[0] = 0.0f;

    _outletParams[0] = new float();
    *(float *)&_outletParams[0] = 0.0f;

    // VP_LINK_STRING
    _inletParams[1] = new std::string();
    *static_cast<std::string *>(_inletParams[1]) = "";

    _outletParams[1] = new std::string();
    *static_cast<std::string *>(_outletParams[1]) = "";

    // VP_LINK_ARRAY
    _inletParams[2] = new std::vector<float>();
    _outletParams[2] = new std::vector<float>();

    // VP_LINK_TEXTURE
    _inletParams[3] = new ofTexture();
    _outletParams[3] = new ofTexture();

    // VP_LINK_AUDIO
    _inletParams[4] = new ofSoundBuffer();
    _outletParams[4] = new ofSoundBuffer();

    // VP_LINK_PIXELS
    _inletParams[5] = new ofPixels();
    _outletParams[5] = new ofPixels();

    // init num inlets/outlets ( to remove in the future? probably)
    numInlets = 6;
    numOutlets = 6;


    // Empty vars init
    empty   = new std::vector<float>();
    kuro    = new ofImage();

    empty->assign(1,0);
    kuro->load("images/kuro.jpg");


    // TESTING INIT shared_ptr<void> params
    *(float *)&_inletParams[0] = 0.0f;

    *static_cast<std::string *>(_inletParams[1]) = "";

    *static_cast<std::vector<float> *>(_inletParams[2]) = *empty;

    *static_cast<ofTexture *>(_inletParams[3]) = kuro->getTexture();

    static_cast<ofTexture *>(_inletParams[3])->readToPixels(*static_cast<ofPixels *>(_inletParams[5]));

    static_cast<ofSoundBuffer *>(_inletParams[4])->set(0.0f);
    static_cast<ofSoundBuffer *>(_outletParams[4])->set(0.0f);

    *static_cast<ofPixels *>(_inletParams[5]) = kuro->getPixels();

    // - - - - - - - - - -
    // Setup parameters (as above, new way)
    //parameters.push_back( std::make_shared< AbstractParameter >( myFloatParam ) );
    parameters.push_back( &myFloatParam );
    parameters.push_back( &myStringParam );
    parameters.push_back( &myIntParam );

    // Other way for dynamic params
    myDynamicParams.resize(0);
    myDynamicParams.emplace_back( "MyDynamicParameter" );
    parameters.push_back( &myDynamicParams.at(0) );
}

//--------------------------------------------------------------
void mosaicNode::update(std::map<int,std::shared_ptr<mosaicNode>> &nodes){

    // update links ( connected params dataflow )
    for(int out=0;out<numOutlets;out++){
        for(int i=0;i<static_cast<int>(linksTo.size());i++){
            if(linksTo[i]->fromOutletID == out){
                // send data through links
                nodes[linksTo[i]->toObjectID]->_inletParams[linksTo[i]->toInletID] = _outletParams[out];
            }
        }
    }


    // bypass params from inlets to outlets (basic standard node operation)

    *(float *)&_outletParams[0] = *(float *)&_inletParams[0];

    *static_cast<std::string *>(_outletParams[1]) = *static_cast<std::string *>(_inletParams[1]);

    *static_cast<std::vector<float> *>(_outletParams[2]) = *static_cast<std::vector<float> *>(_inletParams[2]);

    *static_cast<ofTexture *>(_outletParams[3]) = *static_cast<ofTexture *>(_inletParams[3]);

    *static_cast<ofSoundBuffer *>(_outletParams[4]) = *static_cast<ofSoundBuffer *>(_inletParams[4]);

    *static_cast<ofPixels *>(_outletParams[5]) = *static_cast<ofPixels *>(_inletParams[5]);

}

//--------------------------------------------------------------
void mosaicNode::draw(){
    auto mainSettings = ofxImGui::Settings();

    std::string nodeName = "Node"+ofToString(this->_id);
    if (ofxImGui::BeginWindow(nodeName.c_str(), mainSettings, false))
    {
        ImGui::DragFloat("inlet float --> ", &*(float *)&_inletParams[0]);
        ImGui::SameLine();
        ImGui::Text("outlet float: %s",ofToString(*(float *)&_outletParams[0]).c_str());

        ImGui::InputText("inlet string --> ", static_cast<std::string *>(_inletParams[1]));
        ImGui::SameLine();
        ImGui::Text("outlet string: %s",static_cast<std::string *>(_outletParams[1])->c_str());


        // NEED TO ADD vector<float>

        // NEED TO ADD ofTexture

        // NEED TO ADD ofSoundBuffer

        // NEED TO ADD ofPixels

        // Automatic Parameters drawing.
        ImGui::Separator();

        for(auto it=parameters.begin(); it!=parameters.end(); ++it){
            (*it)->drawImGuiEditable();
        };
    }
    ofxImGui::EndWindow(mainSettings);
}

//--------------------------------------------------------------
bool mosaicNode::connectFrom(std::map<int,std::shared_ptr<mosaicNode>> &nodes, int fromObjectID, int fromOutlet, int toInlet, int linkType){

    // CONNECT FROM "SOME" OBJECT WITH THIS ONE (DRAGGING LINK FROM ANOTHER OBJECT TO SOME INLET OF THIS OBJECT)

    bool connected = false;

    if( (nodes[fromObjectID] != nullptr) && (fromObjectID!=this->_id) && (this->_id != -1) ){

        cout << "Mosaic :: "<< "Connect  " << ofToString(fromObjectID) << " to object " << ofToString(this->_id) << endl;

        shared_ptr<PatchLink> tempLink = shared_ptr<PatchLink>(new PatchLink());

        string tmpID = ofToString(fromObjectID)+ofToString(fromOutlet)+ofToString(this->_id)+ofToString(toInlet);

        tempLink->id            = stoi(tmpID);
        tempLink->type          = linkType;
        tempLink->fromOutletID  = fromOutlet;
        tempLink->toObjectID    = this->_id;
        tempLink->toInletID     = toInlet;

        nodes[fromObjectID]->linksTo.push_back(tempLink);

        if(tempLink->type == VP_LINK_NUMERIC){
            this->_inletParams[toInlet] = new float();
        }else if(tempLink->type == VP_LINK_STRING){
            this->_inletParams[toInlet] = new std::string();
        }else if(tempLink->type == VP_LINK_ARRAY){
            this->_inletParams[toInlet] = new std::vector<float>();
        }else if(tempLink->type == VP_LINK_PIXELS){
            this->_inletParams[toInlet] = new ofPixels();
        }else if(tempLink->type == VP_LINK_TEXTURE){
            this->_inletParams[toInlet] = new ofTexture();
        }else if(tempLink->type == VP_LINK_AUDIO){
            this->_inletParams[toInlet] = new ofSoundBuffer();
        }

        connected = true;

    }

    return connected;

}

//--------------------------------------------------------------
void mosaicNode::disconnectFrom(std::map<int,std::shared_ptr<mosaicNode>> &nodes, int objectInlet){

    // DISCONNECT EXISTING LINK FROM INLET (ONLY ONE LINK CAN CONNECT TO AN INLET)

    for(std::map<int,shared_ptr<mosaicNode>>::iterator it = nodes.begin(); it != nodes.end(); it++ ){
        for(int j=0;j<static_cast<int>(it->second->linksTo.size());j++){
            if(it->second->linksTo[j]->toObjectID == this->_id && it->second->linksTo[j]->toInletID == objectInlet){
                // remove link
                vector<bool> tempEraseLinks;
                for(int s=0;s<static_cast<int>(it->second->linksTo.size());s++){
                    if(it->second->linksTo[s]->toObjectID == this->_id && it->second->linksTo[s]->toInletID == objectInlet){
                        tempEraseLinks.push_back(true);
                    }else{
                        tempEraseLinks.push_back(false);
                    }
                }

                std::vector<std::shared_ptr<PatchLink>> tempBuffer;
                tempBuffer.reserve(it->second->linksTo.size()-tempEraseLinks.size());

                for(int s=0;s<static_cast<int>(it->second->linksTo.size());s++){
                    if(!tempEraseLinks[s]){
                        tempBuffer.push_back(it->second->linksTo[s]);
                    }
                }

                it->second->linksTo = tempBuffer;

                break;
            }

        }

    }
}

//--------------------------------------------------------------
void mosaicNode::disconnectLink(std::map<int,std::shared_ptr<mosaicNode>> &nodes, int linkID){

    // DISCONNECT SELECTED LINK (CLICK AND REMOVE)

    for(std::map<int,shared_ptr<mosaicNode>>::iterator it = nodes.begin(); it != nodes.end(); it++ ){
        for(int j=0;j<static_cast<int>(it->second->linksTo.size());j++){
            if(it->second->linksTo[j]->id == linkID){
                // remove link
                vector<bool> tempEraseLinks;
                for(int s=0;s<static_cast<int>(it->second->linksTo.size());s++){
                    if(it->second->linksTo[s]->id == linkID){
                        tempEraseLinks.push_back(true);
                    }else{
                        tempEraseLinks.push_back(false);
                    }
                }

                std::vector<std::shared_ptr<PatchLink>> tempBuffer;
                tempBuffer.reserve(it->second->linksTo.size()-tempEraseLinks.size());

                for(int s=0;s<static_cast<int>(it->second->linksTo.size());s++){
                    if(!tempEraseLinks[s]){
                        tempBuffer.push_back(it->second->linksTo[s]);
                    }
                }

                it->second->linksTo = tempBuffer;

                break;
            }

        }

    }
}
