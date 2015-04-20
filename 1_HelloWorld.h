#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "Assignment.h"

#include "chai3d.h"

class fish {
private:
public:
    cVector3d pos;
    cVector3d fin1RelPos;
    cVector3d fin2RelPos;

    cVector3d vel;
    cVector3d f;

    cVector3d rot;
    cVector3d rotVel;
    cVector3d rotF;

    double m;


    cShapeSphere* body;

    //methods
    fish();

    cVector3d getFinPos(int finIndex) {
        if (finIndex == 0) {
            return (pos + fin1RelPos);
        } else if (finIndex == 1) {
            return (pos + fin2RelPos);
        }
    }

    void updateFishFins(double timeStep, cVector3d left, cVector3d right) {
        //calculates pushing force

        cVector3d forward = cVector3d(left.x + right.x, 0.0, 0.0);
        f += forward;

        //std::cout << "Left: " << left.str() << std::endl;
        std::cout << "vel: " << vel.str() << std::endl;
    }

    void updatePhysics(double timeStep) {
        vel += timeStep * f / m;
        f = cVector3d(0.0, 0.0, 0.0);

        pos += timeStep*vel;

        body->setPos(pos);
    }
};

fish::fish() {
    fin1RelPos = cVector3d(0.0, 0.03, 0.0);
    fin2RelPos = cVector3d(0.0, -0.03, 0.0);

    pos = cVector3d();
    vel = cVector3d();
    f = cVector3d();

    rot = cVector3d();
    rotVel = cVector3d();
    rotF = cVector3d();

    m = 1.0; //1.0 kg

    body = new cShapeSphere(0.01);
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

    fish *myFish;

public:
    virtual std::string getName() const { return "1: Hello World"; }

	virtual void initialize(cWorld* world, cCamera* camera);

    virtual cMesh* addTriangle(cVector3d pos, cVector3d p0, cVector3d p1, cVector3d p2, cColorf color);

	virtual void updateGraphics();
	virtual void updateHaptics(cGenericHapticDevice* hapticDevice, double timeStep, double totalTime);
};

void HelloWorld::initialize(cWorld* world, cCamera* camera)
{
    myWorld = world;
    myCamera = camera;

    //Change the background
    world->setBackgroundColor(0.0f, 0.4f, 0.0f);

	// Create a cursor with its radius set
	m_cursor = new cShapeSphere(0.01);
	// Add cursor to the world
	world->addChild(m_cursor);

	// Create a small line to illustrate velocity
	m_velocityVector = new cShapeLine(cVector3d(0, 0, 0), cVector3d(0, 0, 0));
	// Add line to the world
	world->addChild(m_velocityVector);

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

    world->addChild(myFish->body);



    //Trying to create the sea floor vertices
    //vector<unsigned int> terrainTriangleIndices
    //terrainTriangleIndices.reserve(100);


    cVector3d pos = cVector3d(-0.1,0,0);
    cVector3d p0 = cVector3d(0.0, 0.0, 0.0);
    cVector3d p1 = cVector3d(0.0, 0.1, 0.0);
    cVector3d p2 = cVector3d(0.0, 0.0, 0.1);
    cMesh* object = addTriangle(pos, p0, p1, p2, cColorf(1,0,0));
    terrainTriangleIndices.push_back(object);

    pos = cVector3d(-0.1,0,-0.1);
    object = addTriangle(pos, p0, p1, p2, cColorf(0.8,0,0));
    terrainTriangleIndices.push_back(object);
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
	cVector3d newPosition;
	hapticDevice->getPosition(newPosition);

	// Update position and orientation of cursor
	m_cursor->setPos(newPosition);

	// Read linear velocity from device
	cVector3d linearVelocity;
	hapticDevice->getLinearVelocity(linearVelocity);

	// Update the line showing velocity
	m_velocityVector->m_pointA = newPosition;
	m_velocityVector->m_pointB = newPosition + linearVelocity;

	// Read user button status
	bool buttonStatus;
	hapticDevice->getUserSwitch(0, buttonStatus);




    //Do stuff with the terrain
    for(int i = 0; i < terrainTriangleIndices.size(); ++i) {
        //std::cout << "before" << std::endl;
        //((cMesh*)myWorld->(terrainTriangleIndices[i]))->setPos(0.1*cSinRad(4*totalTime),0,0);

        //cVector3d v;


        //terrainTriangleIndices[i]->setPos(0.1*cSinRad(4*totalTime),0,0);
        //terrainTriangleIndices[i]->rotate(cVector3d(0,1,0), totalTime);
        cMatrix3d rotValue = cMatrix3d();
        rotValue.set(cVector3d(0,1,0), 2*totalTime);
        terrainTriangleIndices[i]->setRot(rotValue);
        //object->rotate(cVector3d(0,1,0), time);
        //std::cout << "after" << std::endl;
    }



    // Adjust the color of the cursor according to the status of
	// the user switch (ON = TRUE / OFF = FALSE)
	m_cursor->m_material = buttonStatus ? m_matCursorButtonON : m_matCursorButtonOFF;

    if (buttonStatus) {
        myFish->updateFishFins(timeStep, newPosition, newPosition);
    }

    myFish->updatePhysics(timeStep);

	cVector3d force(0, 0, 0);

	//Pull towards center
    //force = -5.0f * newPosition;

	//Set a force to the haptic device
	hapticDevice->setForce(force);
}

#endif
