#include "ofApp.h"

#define SIM_SPEED 1      // how many sim steps per frame
#define SENSOR_DISTANCE 16
#define SENSOR_ANGLE    .4 //0.785398
#define STEP_DISTANCE   4
#define STEP_ANGLE      0.20
#define ATTENUATION     0.993

#define SAVE_MESH_FRAMES 0

#pragma mark - setup
//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetVerticalSync(true);
    ofBackground(0, 0, 0);
    
    runSim = true;
    showSensors = true;
    showParticles = true;
    
    outputMesh.setMode(OF_PRIMITIVE_POINTS);
    
//    ofEnableDepthTest();
//    glEnable(GL_POINT_SMOOTH);
//    glPointSize(2);
    
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = ofVec3f(0, 0, 0);
            temp[i][j] = ofVec3f(0, 0, 0);
            dead[i][j] = 0;
        }
    }
    
    dead[GRID_SIZE/2][GRID_SIZE/2] = 1;
    
    for(int i = 0; i < N_PARTICLES; i++)
    {
        Particle p = Particle();
        particles.push_back(p);
    }
    
    // assuming square window and square grid
    gridMult = GRID_SIZE / float(ofGetWidth());
    gridDiv = float(ofGetWidth()) / GRID_SIZE;
}

#pragma mark - update
//--------------------------------------------------------------
void ofApp::update()
{
    // update multiple time steps per frame
    for (int i=0; i<SIM_SPEED && runSim; i++)
    {
        this->updateSim();
    }
}

#pragma mark - updateSim
//--------------------------------------------------------------
void ofApp::updateSim()
{
    int frame = ofGetFrameNum();
    if (SAVE_MESH_FRAMES > frame)
    {
        ofLogNotice() << "writing frame " << frame;
    } else if (SAVE_MESH_FRAMES == frame) {
        outputMesh.save("mesh.ply");
        ofLogNotice() << "saved mesh file";
    }
    
    // particles sense direction and move 1 step forward
    for(int i = 0; i < N_PARTICLES; i++)
    {
        Particle p = particles[i];
        
        // TODO vector math instead of trig functions
        
        // calculate sensor positions
        float r_sensor_x = SENSOR_DISTANCE * cos(p.direction + SENSOR_ANGLE);
        float r_sensor_y = SENSOR_DISTANCE * sin(p.direction + SENSOR_ANGLE);
        p.sensor_r = p.position + ofVec2f(r_sensor_x, r_sensor_y);
        
        float l_sensor_x = SENSOR_DISTANCE * cos(p.direction - SENSOR_ANGLE);
        float l_sensor_y = SENSOR_DISTANCE * sin(p.direction - SENSOR_ANGLE);
        p.sensor_l = p.position + ofVec2f(l_sensor_x, l_sensor_y);
        
        // get grid values at sensor position
        ofVec3f r_grid_value = grid[toGrid(p.sensor_r.x)][toGrid(p.sensor_r.y)];
        ofVec3f l_grid_value = grid[toGrid(p.sensor_l.x)][toGrid(p.sensor_l.y)];
        
        // rotate away from higher value
        if(l_grid_value.lengthSquared() < r_grid_value.lengthSquared())
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
            p.setup();
        }
        
        // get color from grid
        ofVec3f grid_value = grid[toGrid(p.position.x)][toGrid(p.position.y)];
        ofColor grid_color = ofColor(ofFloatColor(grid_value.x, grid_value.y, grid_value.z));
        ofColor p_color = ofColor(ofFloatColor(p.color.x, p.color.y, p.color.z));
        float grid_hue = grid_color.getHue();
        float p_hue = p_color.getHue();
        float hue = ofLerp(p_hue, grid_hue, .1);
        p_color.setHue(hue);
        p.color = ofVec3f(p_color.r / 255.0, p_color.g / 255.0, p_color.b / 255.0);
        
        // consider saving
        if(SAVE_MESH_FRAMES > frame)
        {
            outputMesh.addVertex(ofVec3f(p.position.x, p.position.y, frame * gridDiv));
            outputMesh.addColor(p_color);
        }

        // count particles in each cell
        grid[toGrid(p.position.x)][toGrid(p.position.y)] += p.color;
        
        // get dead values at sensor position
        float r_dead_value = dead[toGrid(p.sensor_r.x)][toGrid(p.sensor_r.y)];
        float l_dead_value = dead[toGrid(p.sensor_l.x)][toGrid(p.sensor_l.y)];
        
        if (r_dead_value > 0 || l_dead_value > 0)
        {
            dead[toGrid(p.position.x)][toGrid(p.position.y)] = 1;
            // respawn
            p.setup();
        }
        
        // copy back to array
        particles[i] = p;
    }
    
    // blur grid
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            ofVec3f zero = ofVec3f(0, 0, 0);
            ofVec3f tl = i == 0 || j == 0 ? zero : grid[i - 1][j - 1];
            ofVec3f to = j == 0 ? zero : grid[i][j - 1];
            ofVec3f tr = i == GRID_SIZE - 1 || j == 0 ? zero : grid[i + 1][j - 1];
            
            ofVec3f ri = i == GRID_SIZE - 1 ? zero : grid[i + 1][j];
            
            ofVec3f br = i == GRID_SIZE - 1 || j == GRID_SIZE - 1 ? zero : grid[i + 1][j + 1];
            ofVec3f bo = j == GRID_SIZE - 1 ? zero : grid[i][j + 1];
            ofVec3f bl = i == 0 || j == GRID_SIZE - 1 ? zero : grid[i - 1][j + 1];
            
            ofVec3f le = i == 0 ? zero : grid[i-1][j];
            
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

#pragma mark - util
//--------------------------------------------------------------
float ofApp::toGrid(float x)
{
    return ofClamp(x * gridMult, 0, GRID_SIZE - 1);
}

#pragma mark - draw
//--------------------------------------------------------------
void ofApp::draw()
{
    ofClear(0, 0, 0);
    ofFill();
    
//    ofColor c = ofColor::fromHsb(ofRandom(255), 255, 255);
//    ofSetColor(c);
//    ofDrawRectangle(10, 10, 100, 100);
//    return;
    
    for(int i = 0; i < GRID_SIZE; i++)
    {
        for(int j = 0; j < GRID_SIZE; j++)
        {
            ofSetColor(ofFloatColor(grid[i][j].x, grid[i][j].y, grid[i][j].z));
            ofDrawRectangle(i * gridDiv, j * gridDiv, gridDiv, gridDiv);
            
            if( dead[i][j] > 0 )
            {
                ofSetColor(ofFloatColor(dead[i][j]));
                ofDrawRectangle(i * gridDiv, j * gridDiv, gridDiv, gridDiv);
            }
        }
    }
    
    if( showParticles || showSensors )
    {
        for(int i = 0; i < N_PARTICLES; i++)
        {
            Particle p = particles[i];
            
            if( showParticles )
            {
                ofSetColor(ofFloatColor(p.color.x, p.color.y, p.color.z));
                ofDrawRectangle(p.position.x - 1, p.position.y - 1, 3, 3);
            }
            
            if( showSensors )
            {
                ofSetColor(255, 255, 255);
                ofDrawRectangle(p.sensor_l.x, p.sensor_l.y, 1, 1);
                ofDrawRectangle(p.sensor_r.x, p.sensor_r.y, 1, 1);
            }
        }
    }
}

#pragma mark - keyPressed
//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == ' ')
    {
        runSim = !runSim;
    }
    if (key == 's')
    {
        showSensors = !showSensors;
    }
    if (key == 'p')
    {
        showParticles = !showParticles;
    }
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
