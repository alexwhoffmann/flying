#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "Assignment.h"
#include "chai3d.h"

#include <cstdlib>

# define PI          3.141592653589793238462643383279502884L /* pi */

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
        double relYPos;

        //normalize value of Y.
        if (hapticPosition.y <= 0.04 and hapticPosition.y >= -0.04){
            relYPos = hapticPosition.y / 0.04;
        } else if(hapticPosition.y > 0.04){
             relYPos = 1.0;
        } else if(hapticPosition.y < -0.04){
            relYPos = -1.0;
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
        if (hapticPosition.x < 0.0) {
            f.add(hapticPosition.x,0,0);
        }
    }

    void updatePhysics(double timeStep) {
        vel += timeStep * f / m;
        f = cVector3d(0.0, 0.0, 0.0);

        pos = body->getPos();
        pos += timeStep*vel;
        body->setPos(pos);
    }


};

fish::fish() {
    pos = cVector3d();
    vel = cVector3d();
    f = cVector3d();

    rot = cVector3d(-1, 0, 0);// assumed this is the direction he looks
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

/*
M = makeZeroMatrix(100,100)
for each value in M:
        randomly deviate the value a bit

for x, y in M:
    //firstPoint = x,y
    //secondPoint = x,y+1
    //thirdPoint = x+1,y+1
    triangle = makeTriangle(x,y,   x,y+1,  x+1,y+1)
  world->add(triangle)

*/



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

public:
    fish *myFish;

    virtual std::string getName() const { return "1: Hello World"; }

	virtual void initialize(cWorld* world, cCamera* camera);

    virtual cMesh* addTriangle(cVector3d pos, cVector3d p0, cVector3d p1, cVector3d p2, cColorf color);

	virtual void updateGraphics();
	virtual void updateHaptics(cGenericHapticDevice* hapticDevice, double timeStep, double totalTime);

    virtual void initBubbles();
    virtual void updateBubbles();
};

/*
*cVector3d getRandomCVector3d(double minX, double maxX, ) {
    cVector3d =
}
*/

/*
cBitmap* getBitmap(String filename) {

}
*/

void HelloWorld::initBubbles() {
    int numBubbles = 250;
    double x, y, z;


    for (int i = 0; i < numBubbles; i++) {

        //cBitmap bitmap

        cShapeSphere* bubble;
        bubble = new cShapeSphere(0.08);

        x = -(double)(rand() % 1000)/100.0; // 10 m to the front
        y = (double)(rand() % 1000)/100.0 - 5.0; // 5 m radius
        z = (double)(rand() % 1000)/100.0 - 5.0;

        bubble->setPos(cVector3d(x,y,z));
        myWorld->addChild(bubble);
    }
}

void HelloWorld::updateBubbles() {
    for( int a = 0; a < 3; a = a + 1 ){

    double y, z;
    cShapeSphere* bubble;
    bubble = new cShapeSphere(0.08);
    cVector3d direction = myFish->rot;
    cVector3d position = myFish->pos;
    // x is set below
    y = (double)(rand() % 1000)/100.0 - 5.0; // 5 m radius
    z = (double)(rand() % 1000)/100.0 - 5.0;
    bubble->setPos(cVector3d((position + (direction * 10)).x,y,z));
    myWorld->addChild(bubble);
    //std::cout << "New bubble at:" << bubble->getPos() << std::endl;
    }
}


void HelloWorld::initialize(cWorld* world, cCamera* camera)
{
    srand(42);

    myWorld = world;
    myCamera = camera;

    //Change the background
    myWorld->setBackgroundColor(0.0f, 0.4f, 0.0f);

	// Create a cursor with its radius set
    m_cursor = new cShapeSphere(0.0001);
	// Add cursor to the world
    myWorld->addChild(m_cursor);

    initBubbles();

	// Create a small line to illustrate velocity
	m_velocityVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
	// Add line to the world
    myWorld->addChild(m_velocityVector);

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

    //Trying to create the sea floor vertices
    //vector<unsigned int> terrainTriangleIndices
    //terrainTriangleIndices.reserve(100);


    /*
    cVector3d pos = cVector3d(-0.1,0,0);
    cVector3d p0 = cVector3d(0.0, 0.0, 0.0);
    cVector3d p1 = cVector3d(0.0, 0.1, 0.0);
    cVector3d p2 = cVector3d(0.0, 0.0, 0.1);
    cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(1,0,0));
    terrainTriangleIndices.push_back(object);

    pos = cVector3d(-0.1,0,-0.1);
    object = addTriangle(pos, p0, p1, p2, cColorf(0.8,0,0));
    terrainTriangleIndices.push_back(object);
    */
}

cMesh* HelloWorld::addTriangle(cVector3d pos, cVector3d p0, cVector3d p1, cVector3d p2, cColorf color) {
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

    return object;
}

void HelloWorld::updateGraphics()
{
    myCamera->set(myFish->body->getPos() + cVector3d(0.2, 0.0, 0.0),    // camera position (eye)
                  myFish->body->getPos(),    // lookat position (target)
        cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

    //myCamera->
    std::stringstream ss;

	ss << "You can add debug output like this: " << m_cursor->getPos().length() * 1000.0
		<< " mm (Distance from center)";

	m_debugLabel->m_string = ss.str();

	// Position the label
	m_debugLabel->setPos(30, 150, 0);
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
        cMatrix3d rotValue = cMatrix3d();
        rotValue.set(cVector3d(0,1,0), 2*totalTime);
        terrainTriangleIndices[i]->setRot(rotValue);
    }
    // add new bubbles in the distance
  /* float timeSpan;
   timeSpan = timeSpan + timeStep;
   if(timeSpan > 1.0){
        timeSpan = 0.0;
        updateBubbles();
    }

    */
    distance += timeStep*myFish->vel;
    if(distance.length() > 1.00){
        distance = cVector3d(0,0,0);
        updateBubbles();
    }
   /* vel += timeStep * f / m;
    f = cVector3d(0.0, 0.0, 0.0);

    pos = body->getPos();
    pos += timeStep*vel;
    body->setPos(pos);
*/
    // Adjust the color of the cursor according to the status of
	// the user switch (ON = TRUE / OFF = FALSE)
	m_cursor->m_material = buttonStatus ? m_matCursorButtonON : m_matCursorButtonOFF;

    myFish->updateFishFins(timeStep, hapticPosition);
    myFish->updatePhysics(timeStep);

	cVector3d force(0, 0, 0);
    //Set a force to the haptic device
	hapticDevice->setForce(force);
}

#endif
