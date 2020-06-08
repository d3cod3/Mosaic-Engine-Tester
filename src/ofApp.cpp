#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetFrameRate(60);

    this->gui.setup();

    // TESTING SCENARIO, ADD 3 NODES

    nodesMap[0] = std::shared_ptr<mosaicNode>(new mosaicNode(0));
    nodesMap[1] = std::shared_ptr<mosaicNode>(new mosaicNode(1));
    nodesMap[2] = std::shared_ptr<mosaicNode>(new mosaicNode(2));

    // setup nodes
    for(map<int,shared_ptr<mosaicNode>>::iterator it = nodesMap.begin(); it != nodesMap.end(); it++ ){
        it->second->setup();
    }


    // TESTING CONNECT/DISCONNECT

    nodesMap[0]->connectFrom(nodesMap,2,0,0,VP_LINK_NUMERIC);
    nodesMap[1]->connectFrom(nodesMap,2,1,1,VP_LINK_ARRAY);

}

//--------------------------------------------------------------
void ofApp::update(){

    ofSetWindowTitle("Mosaic Engine Tester");

    // update nodes
    for(map<int,shared_ptr<mosaicNode>>::iterator it = nodesMap.begin(); it != nodesMap.end(); it++ ){
        it->second->update(nodesMap);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(20);

    this->gui.begin();

    {
        // draw nodes
        for(map<int,shared_ptr<mosaicNode>>::iterator it = nodesMap.begin(); it != nodesMap.end(); it++ ){
            it->second->draw();
        }

    }

    this->gui.end();
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}
