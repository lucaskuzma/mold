#pragma once

#include "ofMain.h"

#define GRID_SIZE 320
#define N_PARTICLES 512

class Particle
{
public:
    ofVec2f position;
    ofVec2f sensor_l;
    ofVec2f sensor_r;
    ofVec3f color;
    float direction;
    
    Particle(){
        setup();
    }

    void setup()
    {
//        position = ofVec2f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()));
//        position = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);

        direction = ofRandom(TWO_PI);
        
        // cirlce spawn
        float r = ofGetWidth()/2 - 10;
        float antiDirection = direction + PI;
        ofVec2f center = ofVec2f(ofGetWidth()/2, ofGetHeight()/2);
        position = center + ofVec2f(r * cos(antiDirection), r * sin(antiDirection));
        
        ofColor c = ofColor::fromHsb(ofRandom(64), 255, 255);
        color = ofVec3f(c.r / 255.0, c.g / 255.0, c.b / 255.0);
    }
};

class ofApp : public ofBaseApp{
    
    ofEasyCam cam;
    
    array<array<ofVec3f, GRID_SIZE>, GRID_SIZE> grid;
    array<array<ofVec3f, GRID_SIZE>, GRID_SIZE> temp;
    array<array<float, GRID_SIZE>, GRID_SIZE> dead;
    
    float gridMult;
    float gridDiv;
    
    vector<Particle> particles;
    
    bool runSim;
    bool showSensors;
    bool showParticles;
    
    ofMesh outputMesh;
    
public:
    void setup();
    void update();
    void updateSim();
    void draw();
    
    float toGrid(float x);
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
};
