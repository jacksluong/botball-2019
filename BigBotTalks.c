#include <kipr/botball.h>

#define LEFT_IR 1
#define RIGHT_IR 0
#define LEFT_BUTTON 9
#define BUTTON 0
#define SHOULDER 0
#define ELBOW 2
#define HAND 3
#define RED_ORANGE_CHANNEL 0
#define YELLOW_CHANNEL 1
#define BLUE_CUBE_CHANNEL 2
#define BLUE_TAPE_CHANNEL 3

///////////////////////////
///    ENUMS/Globals    ///
///////////////////////////

typedef enum {
    SHOULDER_MINIMUM = 335,
    SHOULDER_SHORT_BUILDING_DROP = 1580,
    SHOULDER_SHORT_BUILDING_GRAB = 1600,
    SHOULDER_TALL_BUILDING_DROP  = 1670,
    SHOULDER_TALL_BUILDING_GRAB  = 1705
} ShoulderPosition;

typedef enum {
    ELBOW_START = 20,
    ELBOW_GROUND = 1975,
    ELBOW_CARRY = 320,
    ELBOW_SHORT_BUILDING_DROP = 900,
    ELBOW_SHORT_BUILDING_GRAB = 900,
    ELBOW_TALL_BUILDING_DROP  = 1530,
    ELBOW_TALL_BUILDING_GRAB  = 1560
} ElbowPosition;

typedef enum {
    HAND_OPEN = 1900,
    HAND_GRAB_CUBE = 1265,
    HAND_GRAB_PERSON = 850,
    HAND_GRAB_BIG_CYLINDER = 1140,
    HAND_GRAB_SMALL_CYLINDER = 890
} HandPosition;

typedef enum {
    CCW = 1,
    CW = -1,
    FORWARD = 1,
    BACKWARD = -1
} Direction;

typedef enum {
    false,
    true
} bool;

typedef enum {
    FRONT_SEES_BLACK,
    MID_SEES_BLACK,
    APPROACH_OBJECT
} ConditionType;

const bool SEEDING_VERSION = true;
const int BLACKTAPE = 2000;
const double LEFT_MULTIPLIER = 1, RIGHT_MULTIPLIER = 0.935; // to adjust for wheel performance inequalities
int BURNING_BUILDING; // determined at runtime
int turned; // used to see how much the Create turns when aligning with object



///////////////////////////////////
///    Function Declarations    ///
///////////////////////////////////

/***** Movements *****/

void set_wheel_velocities(int leftVelocity, int rightVelocity);
void quick_burst(int time, Direction direction);
void move_along_tape_until(ConditionType condition, Direction direction);
void move_along_tape_for(int distance, Direction direction);
void align_with_tape(Direction direction);
void distance(int distance, Direction direction);

void rescue(int buildingNumber);
void shift_building(int change);

/***** Rotation *****/

void turn(double degrees, Direction direction);
int align_with_object(int channel, Direction direction, int maxAngle);

/***** Servos *****/

void set_servo_to(int port, int endPos);
void deliver_to_ground();
void reset();

void deliver_big_blue(int buildingNumber);

/***** Camera *****/

bool get_colored_object(int channel, int minSize);

int identify_burning_skyscraper();

/***** Auxiliary *****/

int get_front_left_ir();
int get_front_right_ir();
int get_left_ir();
int get_right_ir();
int get_lbump();
int get_rbump();
double get_total_angle();
void move_along_tape(Direction direction);

/***** Debug *****/

void test_ir();
void test_front_ir();
void test_camera(int channel);

/***** Miscellaneous *****/

void left_side_action();
void right_side_action();



//////////////////
///    Main    ///
//////////////////

int main() {
    // Precondition: Front is facing the left, Water Rec is at the right end of Main Street on gray but not black
    // Starting position: Leftmost horizontal position in DRZ, with about an inch between BigBot and the back PVC 

    shut_down_in(120);
    // camera_open(HIGH_RES);

    printf("Connecting to Create\n");
    create_connect();

    enable_servos(1,2,3);
    reset();

    /*/ Testing area (asterisk between slashes = inactive; no asterisk = active)
    // test_camera(BLUE_TAPE_CHANNEL);

    // left_side_action();
    right_side_action();
    // move_along_tape_until(APPROACH_OBJECT, FORWARD);

    // align_with_object(RED_ORANGE_CHANNEL, CCW, 0);
    return 0;
    //*/

    //////////////////// Phase 1: Big Blue ////////////////////
    printf("\n---Phase 1: Big Blue---\n");

    printf("Identify burning skyscraper\n");
    BURNING_BUILDING = identify_burning_skyscraper();

    printf("Align with left edge of DRZ\n");
    distance(20, FORWARD);
    turn(85, CCW);
    align_with_tape(FORWARD);

    printf("Move to blue cube\n");
    distance(170, FORWARD);

    printf("Face blue cube\n");
    turned = align_with_object(BLUE_CUBE_CHANNEL, CCW, 25);

    printf("Turn around\n");
    turn(180, CCW);

    printf("Grab cube and carry\n");
    set_servo_to(HAND, HAND_OPEN - 100);
    set_servo_to(ELBOW, ELBOW_GROUND);
    distance(40, BACKWARD);
    set_servo_to(HAND, HAND_GRAB_CUBE);
    set_servo_to(ELBOW, ELBOW_CARRY);	

    printf("Realign with PVC\n");
    turn(turned - 2, CW);

    printf("Align with edge of DRZ ");
    if (BURNING_BUILDING == 3) {
        printf("on the far side");
        quick_burst(1700, FORWARD);
        align_with_tape(FORWARD);
    } else {
	    align_with_tape(FORWARD);
    	distance(20, BACKWARD);
    }

    printf("Turn to face skyscraper\n");
    turn(90, CCW);
    if (BURNING_BUILDING != 3) msleep(3700); // prev: 3500

    printf("Move into position to deliver big blue\n");
    distance(300,FORWARD);
    align_with_tape(FORWARD);
    if (BURNING_BUILDING == 2) shift_building(1);

    printf("Go as close to the building as possible\n");
    distance(175, FORWARD);

    printf("Deliver big blue\n");
    deliver_big_blue(BURNING_BUILDING);

    printf("Go back to Main Street\n");
    distance(50, BACKWARD);
    align_with_tape(BACKWARD);


    //////////////////// Phase 2: Rescue ////////////////////
    printf("\n---Phase 2: Rescue---\n");

    switch (BURNING_BUILDING) {
        case 1:
            printf("Move to building 2\n");
            shift_building(1);

            printf("Perform rescue");
            rescue(2);

            printf("Move to building 3\n");
            shift_building(1);

            printf("Perform rescue");
            rescue(3);
            break;
        case 2:
            printf("Move to building 3\n");
            shift_building(1);

            printf("Perform rescue");
            rescue(3);

            printf("Move to building 1\n");
            shift_building(-2);

            printf("Perform rescue");
            rescue(1);
            break;
        case 3:
            printf("Move to building 2\n");
            shift_building(-1);

            printf("Perform rescue");
            rescue(2);

            printf("Move to building 1\n");
            shift_building(-1);

            printf("Perform rescue");
            rescue(1);
    }


    //////////////////// Phase 3: Gas Valves //////////////////// about 75 seconds elapsed
    printf("\n---Phase 3: First Gas Valve---\n");
    // Current position: On Main Street, facing other board

    printf("Reset servo positions\n");
    reset();
    
    if (!SEEDING_VERSION && BURNING_BUILDING == 1) {
        printf("Turn 90 cw\n");
        turn(90, CW);

        printf("Conduct right side action for first gas valve\n");
        right_side_action();

        printf("Conduct left side action for second gas valve\n");
        left_side_action();
    } else {
        printf("Turn 90 ccw\n");
        turn(90, CCW);

        printf("Conduct left side action for first gas valve\n");
        left_side_action();

        printf("Conduct right side action for second gas valve\n");
        right_side_action();
    }

    //*/
    return 0;
}

/* The reused code for what to do when on the left side of the board for gas valve */
void left_side_action() {
    // Precondition: On Main Street, facing med buildings
    // Postcondition: On Main Street, facing our water rec
    printf("Go to med building at end of Main Street\n");
    move_along_tape_until(FRONT_SEES_BLACK, FORWARD);

    printf("Move towards water rec default position\n");
    distance(30, BACKWARD);
    turn(45, CW);
    distance(265, FORWARD);

    if (get_colored_object(BLUE_TAPE_CHANNEL, 400)) {
        printf("Found water rec! Align with it\n");
        turn(13, CW);
        turned = align_with_object(BLUE_TAPE_CHANNEL, CCW, 20);
        turn(183, CCW);

        printf("Grab it\n");
        distance(25, BACKWARD);
        set_servo_to(ELBOW, ELBOW_GROUND);
        set_servo_to(HAND, HAND_GRAB_BIG_CYLINDER);
        set_servo_to(ELBOW, ELBOW_CARRY);
        distance(25, FORWARD);

        printf("Drop it to the side\n");
        turn(122 + turned, CW);
        set_servo_to(ELBOW, ELBOW_GROUND);
        set_servo_to(HAND, HAND_OPEN);
        set_servo_to(ELBOW, ELBOW_CARRY);

        printf("Turn to face gas valve\n");
        turn(90, CW);
    } else {
        printf("Couldn't find water rec.\nTurn to face gas valve\n");
        turn(45, CW);
    }

    printf("Check if gas valve is there\n");
    distance(95, BACKWARD);
    if (get_colored_object(RED_ORANGE_CHANNEL, 800)) {
        printf("Pick up gas valve\n");
        turn(180, CCW);
        set_servo_to(ELBOW, ELBOW_GROUND);
        set_servo_to(HAND, HAND_GRAB_SMALL_CYLINDER);
        set_servo_to(ELBOW, ELBOW_CARRY);

        printf("Move back to Main Street\n");
        //distance(10, FORWARD);
        distance(80, BACKWARD);
        turn(45, CCW);
        distance(400, FORWARD);
        turn(45, CW);
        align_with_tape(BACKWARD);
        turn(90, CCW);

        printf("Move to other end of Main Street\n");
        move_along_tape_for(1125, FORWARD); // prev: 1000

        printf("Stop at very end of Main Street\n");
        move_along_tape_until(APPROACH_OBJECT, FORWARD);

        printf("Go to utility zone\n");
        turn(104, CCW);
        quick_burst(660, BACKWARD);

        printf("Deliver gas valve\n");
        deliver_to_ground();
        turn(19, CW);

        printf("Bring arm back in\n");
        reset();

        printf("Reset position\n");
        align_with_tape(FORWARD);
        turn(90, CW);
        distance(340, BACKWARD);
    } else {
        printf("No gas valve. Move back to Main Street\n");
        //distance(95, BACKWARD);
        distance(80, FORWARD);
        turn(45, CCW);
        distance(350, BACKWARD);
        turn(45, CW);
        align_with_tape(FORWARD);
        turn(90, CW);

        printf("Move to other end of Main Street\n");
        move_along_tape_for(1050, FORWARD); // 1050
    }  
}

/* The reused code for what to do when on the right side of the board for gas valve */
void right_side_action() {
    // Precondition: On Main Street, facing our water rec
    // Postcondition: On Main Street, facing med buildings
    printf("Move to the end\n");
    move_along_tape_for(200, FORWARD);
    move_along_tape_until(APPROACH_OBJECT, FORWARD);

    printf("Turn to face gas valve\n");
    turn(50, CCW);

    printf("Move towards gas valve\n");
    distance(95, FORWARD);

    printf("Face gas valve\n");
    if ((turned = align_with_object(RED_ORANGE_CHANNEL, CCW, 40)) < 39) {
        printf("Found gas valve! Turn around\n");
        turn(180, CCW);

        printf("Grab gas valve and carry\n");
        set_servo_to(HAND, HAND_OPEN);
        set_servo_to(ELBOW, ELBOW_GROUND);
        distance(85, BACKWARD);
        set_servo_to(HAND, HAND_GRAB_SMALL_CYLINDER);
        set_servo_to(ELBOW, ELBOW_CARRY);	

        printf("Move back to Main Street\n");
        turn(190 - turned, CCW);
        distance(90, BACKWARD);
        align_with_tape(BACKWARD);

        printf("Go to utility zone\n");
        turn(24, CCW);
        quick_burst(600, BACKWARD);

        printf("Deliver gas valve\n");
        deliver_to_ground();
        turn(25, CW);

        printf("Bring arm back in\n");
        reset();

        printf("Reset position\n");
        align_with_tape(FORWARD);
        turn(90, CCW);
    } else {
        printf("No gas valve. Reset position\n");
        distance(30, BACKWARD);
        align_with_tape(BACKWARD);
        turn(90, CCW);
    }
}



///////////////////////
///    Movements    ///
///////////////////////

/* Set velocities of the wheels (convenience method) */
void set_wheel_velocities(int leftVelocity, int rightVelocity) {
    //printf("left: %f, right: %f\n", leftVelocity * LEFT_MULTIPLIER, rightVelocity * RIGHT_MULTIPLIER);
    create_drive_direct(leftVelocity * LEFT_MULTIPLIER, rightVelocity * RIGHT_MULTIPLIER);
}

/* Move at maximum velocity in a certain direction for a specified period of time */
void quick_burst(int time, Direction direction) {
    set_wheel_velocities(500 * direction, 500 * direction);
    msleep(time);
    //    set_wheel_velocities((time > 250 ? 500 : 250) * direction, (time > 250 ? 500 : 250) * direction);
    //    msleep(time * (1 + time <= 250));
    create_stop();
}

/* Move in a direction along a tape until a particular condition is met */
void move_along_tape_until(ConditionType condition, Direction direction) {
    switch (condition) {
        case FRONT_SEES_BLACK:
            while (get_front_left_ir() >= BLACKTAPE || get_front_right_ir() >= BLACKTAPE) {
                move_along_tape(direction);
            }
            break;
        case MID_SEES_BLACK:
            while (get_left_ir() >= BLACKTAPE || get_right_ir() >= BLACKTAPE) {
                move_along_tape(direction);
            }
            break;
        case APPROACH_OBJECT: // approach the water rec at the end of Main Street
            camera_open();
            msleep(500);
            camera_update();
            int x = get_object_center(BLUE_TAPE_CHANNEL, 0).x;
            int elapsedTime = 0; // not in millis
            while (!(x != -1 && x > 75 && get_object_area(BLUE_TAPE_CHANNEL, 0) > 1650) && elapsedTime < 80) {
                if (get_front_left_ir() < BLACKTAPE) {
                    set_wheel_velocities(21, 62);
                } else if (get_front_right_ir() < BLACKTAPE) {
                    set_wheel_velocities(62, 21);
                } else {
                    set_wheel_velocities(62, 62);
                }
                elapsedTime += 3;
                camera_update();
                printf("X: %d, Area: %d, Time: %d\n", get_object_center(BLUE_TAPE_CHANNEL, 0).x, get_object_area(BLUE_TAPE_CHANNEL, 0), elapsedTime);
                x = get_object_center(BLUE_TAPE_CHANNEL, 0).x;
            }
            msleep(1400);
            camera_close();
    }

    create_stop();
}

/* Move in a direction along a tape for a specified distance */
void move_along_tape_for(int distance, Direction direction) {
    if (direction == BACKWARD) turn(175, CW);
    set_create_distance(0);

    while (get_create_distance() < distance) {
        move_along_tape(FORWARD);
    }

    create_stop();
}

/* Align with a line of tape horizontally */
void align_with_tape(Direction direction) {
    int left = get_left_ir();
    int right = get_right_ir();
    // double lastDirection;

    while (left >= BLACKTAPE || right >= BLACKTAPE) {
        if (left < BLACKTAPE) {
            set_wheel_velocities(-10 * direction, 45 * direction);
            // lastDirection = CCW;
        } else if (right < BLACKTAPE) { // pretty consistent
            set_wheel_velocities(50 * direction, -15 * direction);
            // lastDirection = CW;
        } else {
            set_wheel_velocities(50 * direction, 50 * direction);
            // lastDirection = -2.5;
        }

        left = get_left_ir();
        right = get_right_ir();
    }

    // need to move a little farther since sensors are little in front of the middle of the Create
    //msleep(65 * (2.5 + lastDirection));
    set_wheel_velocities(200, 200);
    msleep(330);
    create_stop();
}

/* An alternative move function based on distance rather than power and time */
void distance(int distance, Direction direction) {
    set_create_distance(0);
    while (get_create_distance() * direction < distance) {
        set_wheel_velocities((distance > 200 ? 500 : 225) * direction, (distance > 200 ? 500 : 225) * direction);
    }
    create_stop();
}

////////////////////////////// 2019-ONLY //////////////////////////////

/* Approach a building and bring its damsel in distress to safety (DRZ) */
void rescue(int buildingNumber) {
    bool useTall = buildingNumber == 2;
    set_servo_to(HAND, HAND_OPEN);
    set_servo_to(SHOULDER, useTall ? SHOULDER_TALL_BUILDING_GRAB : SHOULDER_SHORT_BUILDING_GRAB);
    set_servo_to(ELBOW, useTall ? ELBOW_TALL_BUILDING_GRAB : ELBOW_SHORT_BUILDING_GRAB);

    distance(160, FORWARD); // move as close to building as possible

    set_servo_to(HAND, HAND_GRAB_PERSON); // bring arm into deliver position so less strain on the servos
    set_servo_to(SHOULDER, SHOULDER_MINIMUM); // ^^ that's why we don't use the deliver_to_ground() method here
    set_servo_to(ELBOW, ELBOW_GROUND);
    
    distance(50, BACKWARD);

    // go deliver
    switch (buildingNumber) {
        case 1:
            align_with_tape(BACKWARD);
            turn(22, CCW);
            distance(200, BACKWARD);
            break;
        case 2:
            distance(120, BACKWARD);
            align_with_tape(BACKWARD);
            distance(100, BACKWARD);
            break;
        case 3:
            align_with_tape(BACKWARD);
            turn(20, CW);
            distance(200, BACKWARD);
    }

    // drop off cargo
    set_servo_to(HAND, HAND_OPEN);
    set_servo_to(ELBOW, ELBOW_CARRY);

    // go back to main street (completely opposite movements as previous part)
    switch (buildingNumber) {
        case 1:
            distance(200, FORWARD);
            turn(22, CW);
            break;
        case 2:
            align_with_tape(FORWARD);
            break;
        case 3:
            distance(200, FORWARD);
            turn(20, CCW);
    }
}

/* Move to another building based on the change in building number */
void shift_building(int change) {
    if (change > 0) {
        turn(90, CW);
        move_along_tape_for(330 * change, FORWARD);
        turn(90, CCW);
    } else {
        turn(90, CCW);
        move_along_tape_for(330 * change * -1, FORWARD);
        turn(90, CW);
    }
    distance(40, FORWARD);
    align_with_tape(BACKWARD);
}


//////////////////////
///    Rotation    ///
//////////////////////

/* Rotate a certain angle in a specified direction */
void turn(double degrees, Direction direction) {
    if (direction < 0) {
        create_spin_CW(200);
    } else {
        create_spin_CCW(200);
    }
    double baseTimeFor90 = direction == CW ? 3760.0 : 3805.0; // cw rotates very slightly quicker than ccw 
    msleep(baseTimeFor90 * degrees / 360.0);
    create_stop();
}

/* Rotate in a direction until the Create faces an object of the specified color, returning the angle turned */
int align_with_object(int channel, Direction direction, int maxAngle) {
    camera_open(HIGH_RES);
    msleep(750);
    if (maxAngle == 0) maxAngle = 360;
    camera_update();
    set_create_total_angle(0);

    int xcoord = get_object_center(channel, 0).x;
    while ((xcoord == -1 || xcoord >= 64 || xcoord <= 49 || get_object_area(channel, 0) < 60)
           && (get_total_angle() < maxAngle)) {
        if (direction < 0) {
            create_spin_CW(13);
        } else {
            create_spin_CCW(13);
        }
        camera_update();
        xcoord = get_object_center(channel, 0).x;
        msleep(2);
    }

    camera_close();
    create_stop();
    return get_total_angle() * direction; // only returns positive numbers
}


////////////////////
///    Servos    ///
////////////////////

/* Move a servo to a specified position gradually */
void set_servo_to(int port, int endPos) {
    int currentPos = get_servo_position(port);
    bool testGreaterThan = currentPos > endPos; // continue loop while startPos is greater than
    int increment = (testGreaterThan ? -1 : 1) * (port == SHOULDER ? 2 : 3);

    while ((testGreaterThan && (int) currentPos > endPos) ||
           (!testGreaterThan && (int) currentPos < endPos)) {
        currentPos += increment;
        set_servo_position(port, (int) currentPos);
        msleep(3);
    }
}

/* While carrying anything, drop it off at ground level with the arm extended */
void deliver_to_ground() {
    set_servo_to(SHOULDER, SHOULDER_MINIMUM);
    set_servo_to(ELBOW, ELBOW_GROUND);
    set_servo_to(HAND, HAND_OPEN);
    set_servo_to(ELBOW, ELBOW_CARRY);
}

/* Reset servos to initial position */
void reset() {
    enable_servo(HAND);
    set_servo_to(HAND, HAND_OPEN);
    msleep(350);

    enable_servo(ELBOW);
    set_servo_to(ELBOW, ELBOW_START);
    msleep(350);

    enable_servo(SHOULDER);
    set_servo_to(SHOULDER, SHOULDER_MINIMUM);
    msleep(350);
}

////////////////////////////// 2019-ONLY //////////////////////////////

/* While carrying the blue cube, drop it off at the top of the identified burning skyscraper */
void deliver_big_blue(int buildingNumber) {
    bool useTall = buildingNumber == 2;
    set_servo_to(ELBOW, useTall ? ELBOW_TALL_BUILDING_DROP : ELBOW_SHORT_BUILDING_DROP);
    set_servo_to(SHOULDER, useTall ? SHOULDER_TALL_BUILDING_DROP : SHOULDER_SHORT_BUILDING_DROP);
    msleep(1000);
    set_servo_to(HAND, HAND_OPEN);
}


////////////////////
///    Camera    ///
////////////////////

/* Return whether or not an object of the specified color can be seen in the camera's current view */
bool get_colored_object(int channel, int minSize) {
    // if no minSize to test for, argument should be -1
    bool minSizeSatisfied = minSize == -1;
    camera_open(HIGH_RES);
    camera_update();
    msleep(750);

    int i = 0;
    int negatives = 0;

    while (i < 40) { // tests 40 times
        camera_update();
        if (get_object_area(channel, 0) > 4000) continue;
        if (get_object_center(channel, 0).x < 0) negatives++; // when no object, -1 is returned
        if (!minSizeSatisfied) minSizeSatisfied = get_object_area(channel, 0) >= minSize;
        i++;
    }

    camera_close();
    printf("Negatives: %d\n", negatives);
    return negatives < 20 && minSizeSatisfied; // any few incorrect readings will be overwritten by the majority
}

////////////////////////////// 2019-ONLY //////////////////////////////

int identify_burning_skyscraper() {
    camera_open(HIGH_RES);
    camera_update();
    //msleep(700);
    while (get_object_area(YELLOW_CHANNEL, 0) >= 300) { 
        msleep(3);
        camera_update();
    }

    int i = 0;
    int lefts = 0;
    int rights = 0;

    while (i < 30) {
        camera_update();
        if (get_object_area(YELLOW_CHANNEL, 0) >= 42) {
            int x = get_object_center(YELLOW_CHANNEL, 0).x;
            if (x < 70) lefts++;
            else rights++;
        }

        i++;
    }

    camera_close();
    printf("Lefts: %d, Rights: %d\n", lefts, rights);
    return lefts >= 12 ? 1 : (rights >= 12 ? 2 : 3);
}


///////////////////////
///    Auxiliary    ///
///////////////////////

/* Getter */
int get_front_left_ir() {
    return get_create_lfcliff_amt();
}

/* Getter */
int get_front_right_ir() {
    return get_create_rfcliff_amt();
}

/* Getter */
int get_left_ir() {
    return get_create_lcliff_amt();
}

/* Getter */
int get_right_ir() {
    return get_create_rcliff_amt();
}

/* Getter */
int get_lbump() {
    return get_create_lbump();
}

/* Getter */
int get_rbump() {
    return get_create_rbump();
}

/* Getter that adjusts for hardware inaccuracy */
double get_total_angle() {
    return get_create_total_angle() * (360.0 / 385);
    // it reads 411 for a physical 360 degrees, but slightly overshoots anytime I call turn for lesser angles
    // the current multiplier is the average adjustment between accurate 90 and 360 degree turns
}

/* The helper shared function used to move along tape in move_along_tape_for and move_along_tape_until */
void move_along_tape(Direction direction) {
    // NOT TO BE USED manually
    if (get_front_left_ir() < BLACKTAPE) {
        set_wheel_velocities(275 * direction, 350 * direction);
    } else if (get_front_right_ir() < BLACKTAPE) {
        set_wheel_velocities(350 * direction, 275 * direction);
    } else {
        set_wheel_velocities(350 * direction, 350 * direction);
    }
    msleep(1);
}


///////////////////
///    Debug    ///
///////////////////

/* Repeatedly prints the IR readings of the middle sensors */
void test_ir() {
    int left = get_left_ir();
    int right = get_right_ir();

    int i = 0;
    while (1) {
        if (i % 100 == 0) {
            printf("left = %d \n", left);
            printf("right = %d \n", right);
        }
        msleep(2);
        i++;

        left = get_left_ir();
        right = get_right_ir();
    }
    create_stop();

}

/* Repeatedly prints the IR readings of the front sensors */
void test_front_ir() {
    int left = get_front_left_ir();
    int right = get_front_right_ir();

    int i = 0;
    while (1) {
        if(i % 100 == 0){
            printf("left = %d \n", left);
            printf("right = %d \n", right);
        }
        msleep(2);
        i++;

        left = get_front_left_ir();
        right = get_front_right_ir();
    }
    create_stop();

}

/* Repeatedly prints the camera readings for a channel */
void test_camera(int channel) {
    camera_open(HIGH_RES);
    msleep(750);
    while (true) {
        camera_update();
        printf("x: %d, area: %d\n", get_object_center(channel, 0).x, get_object_area(channel, 0));
        msleep(300);
    }
}
