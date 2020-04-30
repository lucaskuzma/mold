#include "ofApp.h"

#define SIM_SPEED 1      // how many sim steps per frame
#define SENSOR_DISTANCE 16
#define SENSOR_ANGLE    0.785398
#define STEP_DISTANCE   1
#define STEP_ANGLE      0.392
#define ATTENUATION     0.99

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetVerticalSync(true);
    ofBackground(0, 0, 0);
    
    runSim = true;
    
    ofEnableDepthTest();
    glEnable(GL_POINT_SMOOTH);
    glPointSize(2);
    
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = 0;
            temp[i][j] = 0;
        }
    }
    
    for(int i = 0; i < N_PARTICLES; i++)
    {
        Particle p = Particle();
        p.position = ofVec2f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()));
        p.direction = ofRandom(TWO_PI);
        particles.push_back(p);
    }
    
    // assuming square window and square grid
    gridMult = GRID_SIZE / float(ofGetWidth());
    gridDiv = float(ofGetWidth()) / GRID_SIZE;
}

//--------------------------------------------------------------
void ofApp::update()
{
    // update multiple time steps per frame
    for (int i=0; i<SIM_SPEED && runSim; i++)
    {
        this->updateSim();
    }
}

//--------------------------------------------------------------
void ofApp::updateSim()
{
    // particles sense direction and move 1 step forward
    for(int i = 0; i < N_PARTICLES; i++)
    {
        Particle p = particles[i];
        
        // TODO vector math instead of trig functions
        
        // calculate sensor positions
        float r_sensor_x = SENSOR_DISTANCE * cos(p.direction + SENSOR_ANGLE);
        float r_sensor_y = SENSOR_DISTANCE * sin(p.direction + SENSOR_ANGLE);
        ofVec2f r_sensor_position = p.position + ofVec2f(r_sensor_x, r_sensor_y);
        
        float l_sensor_x = SENSOR_DISTANCE * cos(p.direction - SENSOR_ANGLE);
        float l_sensor_y = SENSOR_DISTANCE * sin(p.direction - SENSOR_ANGLE);
        ofVec2f l_sensor_position = p.position + ofVec2f(l_sensor_x, l_sensor_y);
        
        // get grid values at sensor position
        float r_grid_value = grid[r_sensor_position.x * gridMult][r_sensor_position.y * gridMult];
        float l_grid_value = grid[l_sensor_position.x * gridMult][l_sensor_position.y * gridMult];
        
        // rotate away from higher value
        if(l_grid_value < r_grid_value)
        {
            p.direction -= STEP_ANGLE;
        } else {
            p.direction += STEP_ANGLE;
        }
        
        // step forward
        float x_step = STEP_DISTANCE * cos(p.direction);
        float y_step = STEP_DISTANCE * sin(p.direction);
        p.position += ofVec2f(x_step, y_step);
        
        // respawn if off edge
        if( p.position.x < 0 || p.position.x >= ofGetWidth() || p.position.y < 0 || p.position.y >= ofGetHeight() ) {
            p.position = ofVec2f(ofRandom(0, ofGetWidth()), ofRandom(0, ofGetHeight()));
        }
        particles[i] = p;
    }

    // count particles in each cell
    for(int i = 0; i < N_PARTICLES; i++)
    {
        Particle p = particles[i];
        grid[ofClamp(p.position.x * gridMult, 0, GRID_SIZE - 1)][ofClamp(p.position.y * gridMult, 0, GRID_SIZE - 1)] += 1;
    }
    
    // blur grid
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            float tl = i == 0 || j == 0 ? 0 : grid[i - 1][j - 1];
            float to = j == 0 ? 0 : grid[i][j - 1];
            float tr = i == GRID_SIZE - 1 || j == 0 ? 0 : grid[i + 1][j - 1];
            
            float ri = i == GRID_SIZE - 1 ? 0 : grid[i + 1][j];
            
            float br = i == GRID_SIZE - 1 || j == GRID_SIZE - 1 ? 0 : grid[i + 1][j + 1];
            float bo = j == GRID_SIZE - 1 ? 0 : grid[i][j + 1];
            float bl = i == 0 || j == GRID_SIZE - 1 ? 0 : grid[i - 1][j + 1];
            
            float le = i == 0 ? 0 : grid[i-1][j];
            
            temp[i][j] = .125 * (tl + to + tr + ri + br + bo + bl + le);
        }
    }
    
    // attenuate grid
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            // copy from temp and attenuate in one step
            grid[i][j] = temp[i][j] * ATTENUATION;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofClear(0, 0, 0);
    ofFill();
    
    ofSetColor(255, 0, 0);
    for(int i = 0; i < N_PARTICLES; i++)
    {
        Particle p = particles[i];
        ofDrawRectangle(p.position.x - 1, p.position.y - 1, 3, 3);
    }
    
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            ofSetColor(ofFloatColor(grid[i][j]));
            ofDrawRectangle(i * gridDiv, j * gridDiv, gridDiv, gridDiv);
        }
    }
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

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
