#pragma once

#include "ofMain.h"

#define GRID_SIZE 320
#define N_PARTICLES 128

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
        position = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);
        direction = ofRandom(TWO_PI);
        float r = 30;
        position += ofVec2f(r * cos(direction), r * sin(direction));
        
        ofColor c = ofColor::fromHsb(ofRandom(255), 255, 255);
        color = ofVec3f(c.r / 255.0, c.g / 255.0, c.b / 255.0);
    }
};

class ofApp : public ofBaseApp{
    
    ofEasyCam cam;
    
    array<array<ofVec3f, GRID_SIZE>, GRID_SIZE> grid;
    array<array<ofVec3f, GRID_SIZE>, GRID_SIZE> temp;
    
    float gridMult;
    float gridDiv;
    
    vector<Particle> particles;
    
    bool runSim;
    bool showSensors;
    bool showParticles;
    
public:
    void setup();
    void update();
    void updateSim();
    void draw();
    
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
