#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "Assignment.h"

#include "chai3d.h"

# define PI          3.141592653589793238462643383279502884L /* pi */

class HelloWorld : public Assignment
{
private:
    // A 3D cursor for the haptic device
    cShapeSphere* body;

    cShapeSphere* bodyFinR;
    cShapeSphere* bodyFinL;

    double finRadius;
    double fishRadius;
    double maxAngle;

   double positionY;


    // A line to display velocity of the haptic interface
    cShapeLine* m_velocityVector;

    // Material properties used to render the color of the cursors
    cMaterial m_matCursorButtonON;
    cMaterial m_matCursorButtonOFF;

	// A label used to demonstrate debug output
    cLabel* m_debugLabel;

public:
    virtual std::string getName() const { return "1: Hello World"; }

	virtual void initialize(cWorld* world, cCamera* camera);

	virtual void updateGraphics();
	virtual void updateHaptics(cGenericHapticDevice* hapticDevice, double timeStep, double totalTime);
};

void HelloWorld::initialize(cWorld* world, cCamera* camera)
{
	//Change the background
	world->setBackgroundColor(0.0f, 0.0f, 0.0f);

	// Create a cursor with its radius set

    finRadius = 0.005;
    fishRadius = 0.01;
    maxAngle = 60;

    body = new cShapeSphere(fishRadius);
    bodyFinR = new cShapeSphere(finRadius);
    bodyFinL = new cShapeSphere(finRadius);

    bodyFinR->setPos(0,0.015,0);
    bodyFinL->setPos(0,-0.015,0);


	// Add cursor to the world
    world->addChild(body);

    world->addChild(bodyFinR);
    world->addChild(bodyFinL);

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
    body->m_material = m_matCursorButtonOFF;

    bodyFinR-> m_material = m_matCursorButtonON;
    bodyFinL-> m_material = m_matCursorButtonOFF;

	// Create a label used to show how debug output can be handled
	m_debugLabel = new cLabel();

	// Labels need to be added to the camera instead of the world
	camera->m_front_2Dscene.addChild(m_debugLabel);
}

void HelloWorld::updateGraphics()
{
	std::stringstream ss;

    ss << "You can add debug output like this: " << body->getPos().length() * 1000.0
            << " mm (Distance from center)";
    //ss << "You can add debug output like this: " << m_cursor->getPos().length() * 1000.0
        //<< " mm (Distance from center)";

	m_debugLabel->m_string = ss.str();

	// Position the label
	m_debugLabel->setPos(30, 150, 0);
}

void HelloWorld::updateHaptics(cGenericHapticDevice* hapticDevice, double timeStep, double totalTime)
{
	//Read the current position of the haptic device
	cVector3d newPosition;
    hapticDevice->getPosition(newPosition);

    //normalize value of Y.
    if (newPosition.y <= 0.04 and newPosition.y >= -0.04){
        positionY = newPosition.y/0.04;
    } else if(newPosition.y > 0.04){
         positionY = 1.0;
    } else if(newPosition.y < -0.04){
        positionY = -1.0;
    }
    double newYRight, newZRight;
    double newYLeft, newZLeft;
    double radiant = positionY * -maxAngle * PI / 180.0 ;

    std::cout << "radiant = " << radiant << std::endl;
    newYRight = (cos(radiant) * (finRadius + fishRadius));
    newZRight = (sin(radiant) * (finRadius + fishRadius));
    newPosition.set(0, newYRight, newZRight);
    bodyFinR->setPos(newPosition);

    newYLeft = -(cos(radiant)*(finRadius+fishRadius));
    newZLeft = -(sin(radiant)*(finRadius+fishRadius));
    newPosition.set(0, newYLeft, newZLeft);
    bodyFinL->setPos(newPosition);
	// Update position and orientation of cursor


	// Read linear velocity from device
	cVector3d linearVelocity;
	hapticDevice->getLinearVelocity(linearVelocity);

	// Update the line showing velocity
	m_velocityVector->m_pointA = newPosition;
	m_velocityVector->m_pointB = newPosition + linearVelocity;

	// Read user button status
	bool buttonStatus;
	hapticDevice->getUserSwitch(0, buttonStatus);

	// Adjust the color of the cursor according to the status of
	// the user switch (ON = TRUE / OFF = FALSE)
    //m_cursor->m_material = buttonStatus ? m_matCursorButtonON : m_matCursorButtonOFF;
    body->m_material = buttonStatus ? m_matCursorButtonON : m_matCursorButtonOFF;

	cVector3d force(0, 0, 0);

	//Pull towards center
	force = -5.0f * newPosition;

	//Set a force to the haptic device
	hapticDevice->setForce(force);
}

#endif
