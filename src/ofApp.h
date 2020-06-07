#pragma once

#include "ofMain.h"

#include "ofxImGui.h"

#include "mosaicNode.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);

    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);


    std::map<int,std::shared_ptr<mosaicNode>> nodesMap;

    // GUI
    ofxImGui::Gui gui;

};
