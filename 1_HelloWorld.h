#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "Assignment.h"
#include "chai3d.h"

#include <cstdlib>

# define PI          3.141592653589793238462643383279502884L /* pi */
string resourceRoot;
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())

#define seaFloorZLevel -14.0
#define numTilesX 80
#define numTilesY 80
#define triangleSize 5.5

#define WATER_SURFACE_TRANSPARENCY 0.6

class fish {
private:
public:
    cVector3d pos;

    cShapeSphere* bodyFinR;
    cShapeSphere* bodyFinL;

    double finRadius;
    double fishRadius;
    double maxAngle;

    cVector3d vel;
    cVector3d f;

    cVector3d rot; //yaw, pitch, roll
    cVector3d rotVel;
    cVector3d rotF;

    double m;


    cShapeSphere* body;

    fish();

    void updateFishFins(double timeStep, cVector3d hapticPosition) {

        //Update the graphical position of the fish fins
        cVector3d newFinPos;

        double relXPos;
        //normalize value of X.
        if (hapticPosition.x <= 0.035 && hapticPosition.x >= 0){
            relXPos = hapticPosition.x / 0.035;
        } else if (hapticPosition.x >= -0.04 && hapticPosition.x <= 0){
            relXPos = hapticPosition.x / 0.04;
        } else if(hapticPosition.x > 0.035){
             relXPos = 1.0;
        } else if(hapticPosition.x < -0.04){
            relXPos = -1.0;
        }

        double relYPos;
        //normalize value of Y.
        if (hapticPosition.y <= 0.04 and hapticPosition.y >= -0.04){
            relYPos = hapticPosition.y / 0.04;
        } else if(hapticPosition.y > 0.04){
             relYPos = 1.0;
        } else if(hapticPosition.y < -0.04){
            relYPos = -1.0;
        }

        double relZPos;
        //normalize value of Z.
        if (hapticPosition.z <= 0.06 && hapticPosition.z >= 0){
            relZPos = hapticPosition.z / 0.06;
        } else if (hapticPosition.z >= -0.06 && hapticPosition.z <= 0){
            relZPos = hapticPosition.z / 0.06;
        } else if(hapticPosition.z > 0.06){
             relZPos = 1.0;
        } else if(hapticPosition.z < -0.06){
            relZPos = -1.0;
        }


        double newYRight, newZRight;
        double newYLeft, newZLeft;
        double radians = relYPos * -maxAngle * PI / 180.0 ;

        //std::cout << "radians = " << radians << std::endl;
        newYRight = (cos(radians) * (finRadius + fishRadius));
        newZRight = (sin(radians) * (finRadius + fishRadius));
        bodyFinR->setPos(body->getPos() + cVector3d(0, newYRight, newZRight));

        newYLeft = -(cos(radians)*(finRadius+fishRadius));
        newZLeft = -(sin(radians)*(finRadius+fishRadius));
        bodyFinL->setPos(body->getPos() + cVector3d(0, newYLeft, newZLeft));

        //change rot
        //cMatrix3d rotValue = cMatrix3d();
        //rotValue.set(, );
        //rot->setRot(rotValue);

        //add forces to fish
        //forward/backward movement
        double pushStrength = timeStep * 2900*relXPos;
        cVector3d pushDirection = -pushStrength * ((1/vel.length())*vel);
        //cVertex

        if (hapticPosition.x < 0.0) {
            f.add(pushDirection);
            //f.add(0,0,0);
        } else {
            if (vel.length() > 0.1) {
                vel *= 0.9993;
            } else {
                //f.add(timeStep * 1500*relXPos,0,0);
                f.add(pushStrength * ((1/vel.length())*vel));
            }
        }

        if (vel.length() > 0.03) {
            //rotation around z axis
            double fx = 1.8 * relYPos * pow(vel.y / vel.length(), 1.0);
            double fy = 1.8 * relYPos * pow(-vel.x / vel.length(), 1.0);
            f.add(fx, fy, 0);


            //moving up/down
            if (body->getPos().z < 0.0) { //in water
                if (relZPos > 0.1 || relZPos < -0.1) {

                    if (vel.length() < 1.5) {
                        f.add(0, 0, 0.1 * relZPos); //weaker when slow
                    } else {
                        f.add(0, 0, 2.3 * relZPos);
                    }

                    if (vel.z < 0.5) {
                        f.add(0, 0, 0.1 * vel.length() * relZPos);
                    }
                }
            } else { //in air
                if (relZPos > 0.1 || relZPos < -0.1) {
                    f.add(0, 0, 0.7 * relZPos);
                }
            }
        }
    }

    cVector3d updatePhysics(double timeStep, double sfm[numTilesX][numTilesY]) {
        cVector3d hapticForceVector;

        //add gravity
        if (this->body->getPos().z > 0.0) {
            if (vel.z > 0) { //flying upwards, add gravity
                cVector3d g = cVector3d(0.0, 0.0, -0.2*9.82);
                f += m * g;
            } else { //flying downwards, constant downvard velocity
                vel += cVector3d(0.0, 0.0, -timeStep*1.5);
            }
        } else { //might be below sea floor

            //seaFloorZLevel
            //#define numTilesX 80
            //#define numTilesY 80
            //#define triangleSize = 5.5;

            //int i = (int)((body->getPos().x / triangleSize) - ((double)numTilesX)/2);
            //int t = (int)((body->getPos().y / triangleSize) - ((double)numTilesY)/2);

            //double z = seaFloorZLevel + sfm[t][i];
            double z = seaFloorZLevel + 3.5;

            if (body->getPos().z < z) { //there was a collision with the seafloor
                f += cVector3d(0.0, 0.0, 7.0);
                hapticForceVector += cVector3d(0.0, 0.0, 9.0);
            }
        }

        rot.z = vel.z;

        vel += timeStep * f / m;
        vel *= 0.9998;
        f.set(0,0,0);

        /*if ((rand() % 250) == 0) {
            std::cout << "vx, vy: " << vel.x << ", " << vel.y << std::endl;
        }*/

        pos = body->getPos();
        pos += timeStep*vel;
        body->setPos(pos);

        hapticForceVector += cVector3d(0.0, 0.0, -1*this->vel.z);

        return hapticForceVector;
    }


};

fish::fish() {
    pos = cVector3d(0.0, 0.0, 3.0);
    vel = cVector3d(-4.5,0,-1.5);
    f = cVector3d();

    rot = cVector3d(-1, 0.0, 0);// assumed this is the direction the fish looks
    rotVel = cVector3d();
    rotF = cVector3d();

    m = 1.0; //1.0 kg

    finRadius = 0.005;
    fishRadius = 0.01;
    maxAngle = 60;

    body = new cShapeSphere(fishRadius);

    bodyFinR = new cShapeSphere(finRadius);
    bodyFinL = new cShapeSphere(finRadius);

    bodyFinR->setPos(0,0.015,0);
    bodyFinL->setPos(0,-0.015,0);
}

class HelloWorld : public Assignment
{
private:
    // A 3D cursor for the haptic device
    cShapeSphere* m_cursor;

    // A line to display velocity of the haptic interface
    cShapeLine* m_velocityVector;

    // Material properties used to render the color of the cursors
    cMaterial m_matCursorButtonON;
    cMaterial m_matCursorButtonOFF;

	// A label used to demonstrate debug output
    cLabel* m_debugLabel;

    //save the world pointer
    cWorld* myWorld;
    cCamera* myCamera;

    //TERRAIN STUFF
    vector<cMesh*> terrainTriangleIndices;

    //FISH STUFF
    cShapeSphere *sphere;

    // moved distance of the fish
    cVector3d distance;

    // bitmap of the  bubbles
    cTexture2D* bubbleBitmap;

    cTexture2D* seafloorBitmap;
    cTexture2D* skyboxTopBitmap;

    double sfm[numTilesX][numTilesY]; //sea floor matrix

public:
    fish *myFish;

    virtual std::string getName() const { return ""; }

	virtual void initialize(cWorld* world, cCamera* camera);

    virtual cMesh* addTriangle(cVector3d pos, cVector3d p0, cVector3d p1, cVector3d p2, cColorf color, bool transparent);

	virtual void updateGraphics();
	virtual void updateHaptics(cGenericHapticDevice* hapticDevice, double timeStep, double totalTime);

    virtual void initBubbles();
    virtual void addNewBubble();
    virtual void applyTextureToBubble(cShapeSphere* bubble);
    virtual double getRandom();
    virtual void createSeaFloor();
    virtual void createWaterSurface();
    virtual void createShadow();
};

void HelloWorld::initBubbles() {
    // load the bubble bitmap
    bubbleBitmap = new cTexture2D();
    bool fileload = bubbleBitmap->loadFromFile(RESOURCE_PATH("bubble.bmp"));
    if (!fileload)
    {
        #if defined(_MSVC)
        fileload = bubbleBitmap->loadFromFile("../haptics_lab1/bubble.bmp");
        #endif
    }
	
    int numBubbles = 0;
    double x, y, z;


    for (int i = 0; i < numBubbles; i++) {
        cShapeSphere* bubble;
        bubble = new cShapeSphere(0.08);

        //x = -(double)(rand() % 1000)/50.0;
        x = (2*getRandom()-1.0) * 95;
        y = (2*getRandom()-1.0) * 95;
        z = -getRandom() * 8.0;

        //y = (double)(rand() % 1000)/25.0 - 25.0;
        //z = (double)(rand() % 1000)/100.0 - 5.0;

        bubble->setPos(cVector3d(x,y,z));
        applyTextureToBubble(bubble);
        myWorld->addChild(bubble);
    }
}

double HelloWorld::getRandom() { //returns a value between 0.0 and 1.0
    return ((double)(rand() % 10000)/10000.0);
}

void HelloWorld::addNewBubble() {
    double x, y, z;
    cShapeSphere* bubble;
    bubble = new cShapeSphere(0.08);


    cVector3d position = myFish->pos + 5*(1/myFish->vel.length())*myFish->vel;
    double deviationDistance = 20.0;
    double dx = deviationDistance * (2*getRandom()-0.5);
    double dy = deviationDistance * (2*getRandom()-0.5);
    double dz = deviationDistance * (2*getRandom()-0.5);
    cVector3d deviation = cVector3d(dx,dy,dz); //move it +- deviationDistance in all three directions
    position += deviation;

    //applyTextureToBubble(bubble);
    bubble->setPos(position);
    myWorld->addChild(bubble);
    std::cout << "New bubble at:" << bubble->getPos() << std::endl;
}

// load the bitmap and add it to the spheres
void HelloWorld::applyTextureToBubble(cShapeSphere* bubble)
{
    // create a texture file
    bubble->m_texture = bubbleBitmap;
    bubble->m_texture->setSphericalMappingEnabled(true);
    bubble->setUseTexture(true);
    bubble->setTransparencyLevel(0.6, true, true);
    //std::cout << "Texture applied:" << std::endl;
}


void HelloWorld::initialize(cWorld* world, cCamera* camera)
{
    srand(42);

    myWorld = world;
    myCamera = camera;

    //Change the background
    myWorld->setBackgroundColor(0.0f, 0.412f, 0.58f);

	// Create a cursor with its radius set
    m_cursor = new cShapeSphere(0.0001);
	// Add cursor to the world
    myWorld->addChild(m_cursor);

    initBubbles();

	// Create a small line to illustrate velocity
	m_velocityVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
	// Add line to the world
    myWorld->addChild(m_velocityVector);

    //2D STUFF

    //bubblesBitmap = new cBitmap();

	// Here we define the material properties of the cursor when the
	// user button of the device end-effector is engaged (ON) or released (OFF)

	// A light orange material color
	m_matCursorButtonOFF.m_ambient.set(0.5, 0.2, 0.0);
	m_matCursorButtonOFF.m_diffuse.set(1.0, 0.5, 0.0);
	m_matCursorButtonOFF.m_specular.set(1.0, 1.0, 1.0);

	// A blue material color
	m_matCursorButtonON.m_ambient.set(0.1, 0.1, 0.4);
	m_matCursorButtonON.m_diffuse.set(0.3, 0.3, 0.8);
	m_matCursorButtonON.m_specular.set(1.0, 1.0, 1.0);

	// Apply the 'off' material to the cursor
	m_cursor->m_material = m_matCursorButtonOFF;

	// Create a label used to show how debug output can be handled
    m_debugLabel = new cLabel();

	// Labels need to be added to the camera instead of the world
	camera->m_front_2Dscene.addChild(m_debugLabel);

    myFish = new fish();

    //sphere = new cShapeSphere(0.01);
    //cShapeLine *myLine = new cShapeLine(cVector3d(0,0.02,1),cVector3d(0,0.02,-1));
    //world->addChild(sphere);


    //cShapeLine *myLine = new cShapeLine(cVector3d(0,0.02,1),cVector3d(0,0.02,-1));
    //world->addChild();

    myWorld->addChild(myFish->body);

    myWorld->addChild(myFish->bodyFinR);
    myWorld->addChild(myFish->bodyFinL);

    createSeaFloor();
    createWaterSurface();
    createShadow();
}

void HelloWorld::createSeaFloor() {
    //texture stuff
    seafloorBitmap = new cTexture2D();
    if (!seafloorBitmap->loadFromFile("../flying/out_snd1.bmp")) {
        std::cout << "Couldn't load snd" << std::endl;
    }

    //create matrix to be used for terrain triangles
    //int numTilesX = 80;
    //int numTilesY = 80;
    //double sfm[numTilesX][numTilesY]; //sea floor matrix

    for (int i = 0; i < numTilesX; i++) {
        for (int t = 0; t < numTilesY; t++) {
            for (int its = 0; its < 10; its++) {
                if (its == 0) {
                    sfm[i][t] = 0.0;
                } else { //perturb it
                    sfm[i][t] += 0.6*getRandom();
                }
            }
        }
    }

    for (int its = 0; its < 25; its++) {
        int x = (rand() % (numTilesX-1))-1;
        int y = (rand() % (numTilesY-1))-1;
        sfm[x][y] += 2.3 + 1.9*getRandom();

        double smallBump = 1.5;

        sfm[x+1][y] += smallBump + 0.7*getRandom();
        sfm[x][y+1] += smallBump + 0.7*getRandom();
        sfm[x-1][y] += smallBump + 0.7*getRandom();
        sfm[x][y-1] += smallBump + 0.7*getRandom();
    }


    //adds a bunch of triangles
    //double triangleSize = 5.5;
    //double seaFloorZLevel = -15.0;

    double seaFloorOffsetX = -(numTilesX * triangleSize) / 2;
    double seaFloorOffsetY = -(numTilesY * triangleSize) / 2;

    for (int i = 0; i < (numTilesX-1); i++) {
        for (int t = 0; t < (numTilesY-1); t++) {
            cVector3d pos = cVector3d(seaFloorOffsetX+i*triangleSize,
                                      seaFloorOffsetY+t*triangleSize,
                                      seaFloorZLevel);

            cVector3d p0 = cVector3d(0.0, 0.0, sfm[t][i]);
            cVector3d p1 = cVector3d(triangleSize, 0.0, sfm[t][i+1]);
            cVector3d p2 = cVector3d(0.0, triangleSize, sfm[t+1][i]);

            cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(0.51,0.26,0.073), false);

            /*object->m_texture = seafloorBitmap;
            object->m_texture->setSphericalMappingEnabled(true);
            object->m_texture->setWrapMode(GL_CLAMP, GL_CLAMP);
            object->setUseTexture(true);*/

            terrainTriangleIndices.push_back(object);

            //add opposite side
            p0 = cVector3d(triangleSize, 0.0, sfm[t][i+1]);
            p1 = cVector3d(triangleSize, triangleSize, sfm[t+1][i+1]);
            p2 = cVector3d(0.0, triangleSize, sfm[t+1][i]);

            object = addTriangle(pos, p0, p1, p2, cColorf(0.44,0.17,0.035), false);
            /*object->m_texture = seafloorBitmap;
            object->m_texture->setSphericalMappingEnabled(true);
            object->setUseTexture(true);*/
            terrainTriangleIndices.push_back(object);
        }
    }
}

void HelloWorld::createWaterSurface() {

    skyboxTopBitmap = new cTexture2D();
    if (!skyboxTopBitmap->loadFromFile("../flying/skybox_midpart.bmp")) {
        std::cout << "Couldn't load skybox_midpart.bmp" << std::endl;
    }

    double waterSurfaceXSize = 400.0;
    double waterSurfaceYSize = 400.0;

    //as seen from below
    cVector3d pos = cVector3d(-waterSurfaceXSize/2.0, -waterSurfaceYSize/2.0, 0.0);
    cVector3d p0 = cVector3d(0.0, 0.0, 0.0);
    cVector3d p1 = cVector3d(0.0, waterSurfaceYSize, 0.0);
    cVector3d p2 = cVector3d(waterSurfaceXSize, 0.0, 0.0);
    cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(0.1, 0.2, 0.8), false);

    //as seen from above
    object = addTriangle(pos, p2, p1, p0, cColorf(0.1, 0.2, 0.8, WATER_SURFACE_TRANSPARENCY), true);
    //object->setTransparencyRenderMode(true, false);
    //object->setTransparencyLevel(0.1, false, true);

    /*
    object->m_texture = skyboxTopBitmap;
    object->m_texture->setSphericalMappingEnabled(true);
    object->m_texture->setWrapMode(GL_CLAMP, GL_CLAMP);
    object->setUseTexture(true);*/

    //as seen from below
    pos = cVector3d(-waterSurfaceXSize/2.0, -waterSurfaceYSize/2.0, 0.0);
    p0 = cVector3d(0.0, waterSurfaceYSize, 0.0);
    p1 = cVector3d(waterSurfaceXSize, waterSurfaceYSize, 0.0);
    p2 = cVector3d(waterSurfaceXSize, 0.0, 0.0);
    object = addTriangle(pos, p0, p1, p2, cColorf(0.1, 0.2, 0.85), false);

    //as seen from above
    object = addTriangle(pos, p2, p1, p0, cColorf(0.1, 0.2, 0.8, WATER_SURFACE_TRANSPARENCY), true);
}

void HelloWorld::createShadow() {
    /*
    cVector3d pos = cVector3d(0.0, 0.0, 0.0);
    cVector3d p0 = cVector3d(0.0, 0.0, 0.0);
    cVector3d p1 = cVector3d(0.0, r, 0.0);
    cVector3d p2 = cVector3d(-r, 0.0, 0.0);
    cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(1.0, 0.3, 0.0), false);
    */

    //draw a circle
    double r, r2;
    int N = 36;
    double a1, a2;
    for (int i = 0; i < N; i++) {
        r = 0.011;
        a1 = 2*PI * ((double)i)/N;
        a2 = 2*PI * ((double)(i+1))/N;
        cVector3d pos = cVector3d(0.0, 0.0, 0.00);
        cVector3d p0 = cVector3d(0.0, 0.0, 0.0);
        cVector3d p1 = cVector3d(r*cos(a1), r*sin(a1), 0.0);
        cVector3d p2 = cVector3d(r*cos(a2), r*sin(a2), 0.0);
        cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(1.0, 0.3, 0.0), false);

        r2 = 0.006;
        for (int dir = -1; dir <= 1; dir += 2) {
            pos = cVector3d(0.0, 1.3*r*dir, 0.00);
            p0 = cVector3d(0.0, 0.0, 0.0);
            p1 = cVector3d(r2*cos(a1), r2*sin(a1), 0.0);
            p2 = cVector3d(r2*cos(a2), r2*sin(a2), 0.0);
            object = addTriangle(pos, p0, p1, p2, cColorf(1.0, 0.3, 0.0), false);
        }
    }
}

cMesh* HelloWorld::addTriangle(cVector3d pos, cVector3d p0, cVector3d p1, cVector3d p2, cColorf color, bool transparent) {
    cMesh* object = new cMesh(myWorld);

    myWorld->addChild(object);
    object->setPos(pos);

    unsigned int index = object->newTriangle(p0, p1, p2);
    cTriangle* triangle = object->getTriangle(index);

    cVertex* vertex0 = triangle->getVertex0();
    cVertex* vertex1 = triangle->getVertex1();
    cVertex* vertex2 = triangle->getVertex2();

    vertex0->setColor(color);
    vertex1->setColor(color);
    vertex2->setColor(color);

    // we indicate that we ware rendering the triangle by using the specific
    // colors for each of them (see abov)
    object->setUseVertexColors(true);
    if (transparent) {
        object->setUseTransparency(true, false);
    }

    return object;
}

void HelloWorld::updateGraphics()
{
    myCamera->set(myFish->body->getPos() // camera position (eye)
                  - 0.25 * (1/myFish->vel.length()) * myFish->vel //move it behind the fish
                  + cVector3d(0,0,0.02), //move it up a bit
                  myFish->body->getPos() + 2.25 * (1/myFish->vel.length()) * myFish->vel ,    // lookat position (target)
        cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector    

    // add new bubbles in the distance

    /*if (getRandom() < 0.1) {
        addNewBubble();
    }*/


    std::stringstream ss;
    ss << "Fish pos z: " << myFish->body->getPos().z;
	m_debugLabel->m_string = ss.str();
    m_debugLabel->setPos(10, 40, 0);
}

void HelloWorld::updateHaptics(cGenericHapticDevice* hapticDevice, double timeStep, double totalTime)
{
	//Read the current position of the haptic device
    cVector3d hapticPosition;
    hapticDevice->getPosition(hapticPosition);

	// Update position and orientation of cursor
    m_cursor->setPos(hapticPosition);

	// Read linear velocity from device
	cVector3d linearVelocity;
	hapticDevice->getLinearVelocity(linearVelocity);

	// Update the line showing velocity
    m_velocityVector->m_pointA = hapticPosition;
    m_velocityVector->m_pointB = hapticPosition + linearVelocity*0.2;

	// Read user button status
	bool buttonStatus;
	hapticDevice->getUserSwitch(0, buttonStatus);



    //Do stuff with the terrain
    for(int i = 0; i < terrainTriangleIndices.size(); ++i) {
        //cMatrix3d rotValue = cMatrix3d();
        //rotValue.set(cVector3d(0,1,0), 2*totalTime);
        //terrainTriangleIndices[i]->setRot(rotValue);
        //terrainTriangleIndices[i]->setPos(terrainTriangleIndices[i]->getPos() + cVector3d(1e-3, 0, 0));
    }

    // Adjust the color of the cursor according to the status of
	// the user switch (ON = TRUE / OFF = FALSE)
	m_cursor->m_material = buttonStatus ? m_matCursorButtonON : m_matCursorButtonOFF;

    myFish->updateFishFins(timeStep, hapticPosition);
    cVector3d hapticForceVector = myFish->updatePhysics(timeStep, sfm);

    double spdMultiplier = 14 * myFish->vel.length();
    double x, y;
    double exponent = 0.7;
    if (hapticPosition.x > 0.0)
        x = pow(hapticPosition.x, exponent);
    else
        x = -pow(-hapticPosition.x, exponent);

    if (hapticPosition.y > 0.0)
        y = pow(hapticPosition.y, exponent);
    else
        y = -pow(-hapticPosition.y, exponent);

    if (myFish->body->getPos().z > 0.0) { //weaker force feedback when above water
        spdMultiplier *= 0.15;
    }

    hapticForceVector += cVector3d(-0.3*spdMultiplier*x,
                                   -spdMultiplier*y,
                                   0.0);

    hapticDevice->setForce(hapticForceVector);
}

#endif
