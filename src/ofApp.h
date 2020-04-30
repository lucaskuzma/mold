#pragma once

#include "ofMain.h"

#define GRID_SIZE 320
#define N_PARTICLES 256

class Particle
{
public:
    ofVec2f position;
    ofVec2f sensor_l;
    ofVec2f sensor_r;
    ofVec3f color;
    float direction;
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
