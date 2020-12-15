#include "ofApp.h"
#include <Tracy.hpp>


//--------------------------------------------------------------
void ofApp::setup(){
    ZoneScopedN("ofApp::Setup()");
    ofSetLogLevel(OF_LOG_NOTICE);
    ofSetFrameRate(60);

    this->gui.setup();

    // Set pan-zoom canvas
    canvas.disableMouseInput();
    canvas.setbMouseInputEnabled(true);
    canvas.toggleOfCam();
    //easyCam.enableOrtho();

    updateCanvasViewport();


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
    nodesMap[1]->connectFrom(nodesMap,2,1,1,VP_LINK_STRING);
    nodesMap[1]->connectFrom(nodesMap,2,0,1,VP_LINK_STRING);
    // new way ?
    //nodesMap[0]->parameters.front()->connectWith( nodesMap[1]->parameters.front() );

    TracyAppInfo("MosaicVersion 777", sizeof("MosaicVersion 777"));
}

//--------------------------------------------------------------
void ofApp::update(){
    ZoneScopedN("ofApp::Update()");
    updateCanvasViewport();
    ofSetWindowTitle("Mosaic Engine Tester");

    // update nodes
    {
        ZoneScopedN("node Objects update loop");
        for(map<int,shared_ptr<mosaicNode>>::iterator it = nodesMap.begin(); it != nodesMap.end(); it++ ){
            //ZoneScopedN("nodeObjectUpdate");
            it->second->update(nodesMap);
        }
    }

    TracyPlotConfig("MosaicFPS", tracy::PlotFormatType::Number);
    TracyPlot("MosaicFPS", ofGetFrameRate());
}

//--------------------------------------------------------------
void ofApp::draw(){
    ZoneScopedN("ofApp::Draw()");
    ofPushView();
    ofPushStyle();
    ofPushMatrix();

    // Init canvas
    nodeCanvas.SetTransform( canvas.getTranslation(), canvas.getScale() );//canvas.getScrollPosition(), canvas.getScale(true) );

    canvas.begin(canvasViewport);

    ofEnableAlphaBlending();
    ofSetCurveResolution(50);
    ofSetColor(255);
    ofSetLineWidth(1);

    this->gui.begin();
    ImGui::SetNextWindowPos(canvasViewport.getTopLeft(), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2(canvasViewport.width, canvasViewport.height), ImGuiCond_Always );
    bool isCanvasVisible = nodeCanvas.Begin("ofxVPNodeCanvas" );

    // END VP DRAW

    if ( isCanvasVisible ){
        ZoneScopedN("node Objects draw loop");
        // draw nodes (will be in PatchObject)
        for(map<int,shared_ptr<mosaicNode>>::iterator it = nodesMap.begin(); it != nodesMap.end(); it++ ){
            ZoneScopedN("nodeObjectDraw");
            shared_ptr<mosaicNode> node = it->second;

            // Let objects draw their own Gui
            //node->drawObjectNodeGui( _nodeCanvas );
            //node->draw();
            node->drawObjectNodeGui( nodeCanvas );

        }

    }

    // Close canvas
    if ( isCanvasVisible ) nodeCanvas.End();

    // We're done drawing to IMGUI
    this->gui.end();

    canvas.end();

    ofDisableAlphaBlending();

    ofPopMatrix();
    ofPopStyle();
    ofPopView();

    // Graphical Context
    canvas.update();

    // LIVE PATCHING SESSION
    //drawLivePatchingSession();

    FrameMark; // Tracy end of frame
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
void ofApp::mouseMoved(ofMouseEventArgs &e){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(ofMouseEventArgs &e){
    if(ImGui::IsAnyItemActive() || nodeCanvas.isAnyNodeHovered() || ImGui::IsAnyItemHovered() )// || ImGui::IsAnyWindowHovered())
        return;

    canvas.mouseDragged(e);
}

//--------------------------------------------------------------
void ofApp::mousePressed(ofMouseEventArgs &e){
    if(ImGui::IsAnyItemActive() || nodeCanvas.isAnyNodeHovered() || ImGui::IsAnyItemHovered() )
        return;

    canvas.mousePressed(e);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(ofMouseEventArgs &e){
    if(ImGui::IsAnyItemActive() || nodeCanvas.isAnyNodeHovered() || ImGui::IsAnyItemHovered() )
        return;

    canvas.mouseReleased(e);
}
void ofApp::mouseScrolled(ofMouseEventArgs &e){

    if(ImGui::IsAnyItemActive() || nodeCanvas.isAnyNodeHovered() || ImGui::IsAnyItemHovered())// | ImGui::IsAnyWindowHovered() )
        return;

    canvas.mouseScrolled(e);
}

void ofApp::updateCanvasViewport(){
    canvasViewport.set(0,0,ofGetWindowWidth(),ofGetWindowHeight());
}
