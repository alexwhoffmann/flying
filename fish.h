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
    vel = cVector3d(-4.5,0,5.5);
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
