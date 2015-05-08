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
#define WATER_SURFACE_TRANSPARENCY_FROM_BELOW 0.3

#define BODY_MODEL_OFFSET 0.6

#include "fish.h"


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
    vector<cMesh*> mainShadowTriangles;
    vector<cMesh*> leftShadowTriangles;
    vector<cMesh*> rightShadowTriangles;

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
    virtual void postUpdateGraphics();
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

    cVector3d position = myFish->body->getPos() + 5*(1/myFish->vel.length())*myFish->vel;
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
    myFish->loadModel(myWorld);

    myFish->body->offsetVertices(cVector3d(0.0, 0.0, -15.0), false, false);

    //myFish->body->scale(cVector3d(0.01, 0.01, 0.01), false);
    //myFish->body->scaleObject(cVector3d(0.01, 0.01, 0.01));

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
/*

*/

void HelloWorld::createWaterSurface() {

    skyboxTopBitmap = new cTexture2D();
    if (!skyboxTopBitmap->loadFromFile("../flying/skybox_top.bmp")) {
        std::cout << "Couldn't load skybox_top.bmp" << std::endl;
    }

    double waterSurfaceXSize = 400.0;
    double waterSurfaceYSize = 400.0;

    double skySurfaceXSize = 800.0;
    double skySurfaceYSize = 800.0;

    cVector3d pos, p0, p1, p2;
    cMesh* object;

    /////////////
    // Skybox
    //
    double SKYBOX_HEIGHT = 22.0;
    pos = cVector3d(-skySurfaceXSize/2.0, -skySurfaceYSize/2.0, SKYBOX_HEIGHT);

    //FIRST TRIANGLE
    p0 = cVector3d(0.0, 0.0, 0.0);
    p1 = cVector3d(0.0, skySurfaceYSize, 0.0);
    p2 = cVector3d(skySurfaceXSize, 0.0, 0.0);
    object = addTriangle(pos, p0, p1, p2, cColorf(1.0, 1.0, 1.0), false);
    object->m_texture = skyboxTopBitmap;
    object->m_texture->setSphericalMappingEnabled(true);
    //object->m_texture->setWrapMode(GL_CLAMP, GL_CLAMP);
    object->m_texture->setWrapMode(GL_REPEAT, GL_REPEAT);
    //object->setUseVertexColors(false);
    object->setUseTexture(true);

    //SECOND TRIANGLE
    p0 = cVector3d(0.0, skySurfaceYSize, 0.0);
    p1 = cVector3d(skySurfaceXSize, skySurfaceYSize, 0.0);
    p2 = cVector3d(skySurfaceXSize, 0.0, 0.0);
    object = addTriangle(pos, p0, p1, p2, cColorf(1.0, 1.0, 1.0), false);
    object->m_texture = skyboxTopBitmap;
    object->m_texture->setSphericalMappingEnabled(true);
    //object->m_texture->setWrapMode(GL_CLAMP, GL_CLAMP);
    object->m_texture->setWrapMode(GL_REPEAT, GL_REPEAT);
    //object->setUseVertexColors(false);
    object->setUseTexture(true);


    ///////////////
    // Water Surface
    //

    //as seen from below
    pos = cVector3d(-waterSurfaceXSize/2.0, -waterSurfaceYSize/2.0, 0.0);
    p0 = cVector3d(0.0, 0.0, 0.0);
    p1 = cVector3d(0.0, waterSurfaceYSize, 0.0);
    p2 = cVector3d(waterSurfaceXSize, 0.0, 0.0);
    object = addTriangle(pos, p0, p1, p2, cColorf(0.1, 0.2, 0.8, WATER_SURFACE_TRANSPARENCY_FROM_BELOW), true);

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
    object = addTriangle(pos, p0, p1, p2, cColorf(0.1, 0.2, 0.85, WATER_SURFACE_TRANSPARENCY_FROM_BELOW), true);

    //as seen from above
    object = addTriangle(pos, p2, p1, p0, cColorf(0.1, 0.2, 0.8, WATER_SURFACE_TRANSPARENCY), true);
}

void HelloWorld::createShadow() {
    //draw a circle
    double r, r2;
    int N = 36;
    double a1, a2;
    for (int i = 0; i < N; i++) {
        r = 0.014;
        a1 = 2*PI * ((double)i)/N;
        a2 = 2*PI * ((double)(i+1))/N;
        cVector3d pos = cVector3d(0.0, 0.0, 0.00);
        cVector3d p0 = cVector3d(0.0, 0.0, 0.0);
        cVector3d p1 = cVector3d(r*cos(a1), r*sin(a1), 0.0);
        cVector3d p2 = cVector3d(r*cos(a2), r*sin(a2), 0.0);
        cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(0.0, 0.0, 0.2), false);
        mainShadowTriangles.push_back(object);

        r2 = 0.008;
        for (int dir = -1; dir <= 1; dir += 2) {
            pos = cVector3d(0.0, 1.3*r*dir, 0.00);
            p0 = cVector3d(0.0, 0.0, 0.0);
            p1 = cVector3d(r2*cos(a1), r2*sin(a1), 0.0);
            p2 = cVector3d(r2*cos(a2), r2*sin(a2), 0.0);
            object = addTriangle(pos, p0, p1, p2, cColorf(0.0, 0.0, 0.2), false);

            if (dir == -1) {
                leftShadowTriangles.push_back(object);
            } else {
                rightShadowTriangles.push_back(object);
            }
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
    //set camera
    myCamera->set(myFish->body->getPos() // camera position (eye)
                  //- 0.25 * (1/myFish->vel.length()) * myFish->vel //move it behind the fish
                  //- 0.35 * (1/myFish->vel.length()) * myFish->vel //move it behind the fish
                  - 3 * 0.25 * (1/myFish->vel.length()) * myFish->vel //move it behind the fish
                  + cVector3d(0,0,0.02), //move it up a bit
                  myFish->body->getPos() + 2.25 * (1/myFish->vel.length()) * myFish->vel ,    // lookat position (target)
        cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector    

    //set body rotation
    //fix mismatched initial wrong rotation
    /*cMatrix3d rotValue = cMatrix3d();
    rotValue.set(cVector3d(0.0, 0.0, 1.0), PI/2 + 0.0);
    myFish->body->setRot(rotValue);*/

    //move the body
    //myFish->body->setPos(myFish->body->getPos() + cVector3d(0.0,0.0,-BODY_MODEL_OFFSET));

    //update shadow position
    double shadowX, shadowY, shadowZ;
    for (int i = 0; i < mainShadowTriangles.size(); i++) {
        shadowX = myFish->body->getPos().x;
        shadowY = myFish->body->getPos().y;

        if (myFish->body->getPos().z < 0.0) //under the water
            shadowZ = -1000.0;
        else
            shadowZ = 0.001;

        //mainShadowTriangles[i]->setPos(shadowX, shadowY, shadowZ);
        //mainShadowTriangles[i]->scale(cVector3d(0.01, 0.01, 0.01));


        //leftShadowTriangles[i]->setPos(x,y,z);
        //rightShadowTriangles[i]->setPos(x,y,z);
        //leftShadowTriangles[i]->setRot(0.0);
        //rightShadowTriangles[i]->setRot(0.0);

    }

    // add new bubbles in the distance

    /*if (getRandom() < 0.1) {
        addNewBubble();
    }*/


    std::stringstream ss;
    ss << "Fish pos z: " << myFish->body->getPos().z;
	m_debugLabel->m_string = ss.str();
    m_debugLabel->setPos(10, 40, 0);
}

void HelloWorld::postUpdateGraphics()
{
    //move back fish

    //myFish->body->setPos(myFish->body->getPos() - cVector3d(0.0,0.0,-BODY_MODEL_OFFSET));
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
