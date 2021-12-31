#include <kipr/botball.h>
#define LEFT_MOTOR 0
#define RIGHT_MOTOR 1
#define ARM 1 // make sure this matches with actual ports
#define CLAW 0
#define FRONT_LEFT_IR 0
#define FRONT_RIGHT_IR 1
#define BACK_LEFT_IR 2
#define BACK_RIGHT_IR 3

///////////////////////////
///    ENUMS/Globals    ///
///////////////////////////

int counter = 0;

typedef enum {
	false,
	true
} bool;

typedef enum {
	CCW = 1,
	CW = -1,
	FORWARD = 1,
	BACKWARD = -1
} Direction;

typedef enum {
	CLAW_GRAB = 1460,
	CLAW_RELEASE = 980, //1590
	CLAW_START = 1655,
	CLAW_OPEN = 550
} ClawPosition;

typedef enum {
	LEFT_IR_LIMIT = 3500,
	RIGHT_IR_LIMIT = 3500
} IRSensorsMaximumValues;

typedef enum {
	LEFT_WHITE = 2000,
	RIGHT_WHITE = 1500,
	RIGHT_WHITE_DUCT = 2000
} WhiteReading;

typedef enum {
	FRONT_LEFT_BLACK = 2000,
	FRONT_RIGHT_BLACK = 1500,
	BACK_LEFT_BLACK = 3500,
	BACK_RIGHT_BLACK = 3000
} BlackReading; // black > BlackReading



///////////////////////////////////
///    Function Declarations    ///
///////////////////////////////////

/***** Movement *****/

void move(int leftSpeed, int rightSpeed, int time); // manual movement
void quick_burst(int time, Direction direction);
void align_with_tape(Direction direction, bool useFrontSensors);
void move_along_main_street();
void move_out_with_cube();
//void move_when_both_sensors_see_black();

/***** Rotation *****/

void pivot(int pivot, double degrees, int direction);
void rotate(double degrees, int direction);

/***** Servos *****/

void set_claw_to(ClawPosition endPos);
void set_claw_slowly_to(int endPos);
void restart();



///////////////////
///    Main    ////
///////////////////

int main()

{

	restart();
	msleep(2000);
	// Preconditions: Closed claw (1959), upper-right most position in box possible, fingers slightly over 45 degrees from the line of sight
	// Positions: Ambulance left (somewhere that's not touching fingers), Firefighter right (flush with right finger)



    //////////////////// Phase 1A: Main Street ////////////////////

	printf("R CW 90\n");
	pivot(RIGHT_MOTOR, 100, CW);// a little over 90 deg. just so left IR sensor isn't so close to edge of box

	printf("out of box\n");
	quick_burst(2450, FORWARD);

	printf("align with tape\n");
	align_with_tape(FORWARD, true);

	printf("Pivot ccw around left wheel\n");
	pivot(LEFT_MOTOR, 90, CCW);

	printf("Go to main street\n");
	quick_burst(1350, FORWARD);

	printf("Align with tape\n");
	align_with_tape(FORWARD, true);

	printf("Pivot ccw around left wheel\n");
	pivot(LEFT_MOTOR, 90, CCW);


    //////////////////// Phase 1B: Little Items //////////////////// move into far med building

	printf("Move to med center\n");
	move_along_main_street(); // aligning with tape included

	move(85,40,1500);
	quick_burst(200, FORWARD);
	pivot(RIGHT_MOTOR, 20, CW);
	quick_burst(2000, BACKWARD);

	printf("Pivot cw around right wheel\n");
	pivot(RIGHT_MOTOR, 35, CW);

	printf("Align with tape\n");
	align_with_tape(FORWARD, true);

	printf("Move forward a bit\n");
	quick_burst(50, FORWARD);

	printf("Pivot cw around left wheel\n");
	pivot(LEFT_MOTOR, 90, CCW);

	quick_burst(600, FORWARD);

	align_with_tape(BACKWARD, true);


	printf("Back up fr this time\n");
	quick_burst(500, BACKWARD);

	printf("Open claw\n");
	set_claw_to(CLAW_OPEN);

	printf("Pivot cw > 90 around right wheel\n");
	pivot(RIGHT_MOTOR, 107, CW);//112 // manual pivot slightly over 90 cw so we can back up straight into prism without align-tape

	printf("Move to fire truck\n");
	quick_burst(2100, BACKWARD);

	printf("Grab fire truck\n");
	set_claw_slowly_to(CLAW_GRAB);

	printf("Pivot cw around right wheel\n");
	pivot(RIGHT_MOTOR, 180, CW);

	printf("Move back \n");
	quick_burst(500,BACKWARD);

	printf("Align with tape backwards \n");
	align_with_tape(BACKWARD, false);

	printf("Pivot ccw around right wheel \n");
	pivot(RIGHT_MOTOR, 42, CCW);

	printf("Move back \n");
	quick_burst(1200, BACKWARD);

	set_claw_to(CLAW_OPEN);//yes

	printf("Move forward \n");
	quick_burst(1500, FORWARD);//2300

	printf("Rotate ccw \n");
	rotate(125, CCW);

	quick_burst(500, FORWARD);

	printf("Align with tape\n");
	align_with_tape(BACKWARD, true);

	printf("Move forward \n");
	quick_burst(2100, BACKWARD);

	printf("pivot 90 CW \n");
	rotate(85, CW);
    ///////////////////////
	printf("align with tape back \n");
	align_with_tape(BACKWARD, false);

	printf(Pivot cw around left wheel \n");
	pivot(LEFT_MOTOR, 4 , CW);

	printf("Open claw \n");
	set_claw_to(CLAW_OPEN);

	printf("Go to firefighter pole \n");
	quick_burst(720, BACKWARD);

	printf("Grab firefighter \n");
	set_claw_to(CLAW_GRAB);

	printf("Move out a bit while holding cube so that it won’t hit PVC \n");
	quick_burst(100, FORWARD);

	printf("Rotate ccw \n");
	rotate(60,CCW);

	printf("Move out from cube \n");
	quick_burst(1000, FORWARD);

	rotate(113,CW);


	quick_burst(1700, BACKWARD);

	printf("Drop firefighter \n");
	set_claw_to(CLAW_RELEASE);

    //////////////////// Phase 4: Firefighter 2 ////////////////////

	quick_burst(1700, FORWARD);

	printf("Pivot ccw around left wheel\n");
	pivot(LEFT_MOTOR, 95 , CCW);

	printf("Align with tape backwards \n");
	align_with_tape(BACKWARD, false);

	printf("Pivot cw \n");
	pivot(LEFT_MOTOR, 5, CW);

	printf("Open claw \n");
	set_claw_to(CLAW_OPEN);

	printf("Go to firefighter pole \n");
	quick_burst(710, BACKWARD);

	printf("Grab firefighter \n");
	set_claw_to(CLAW_GRAB);

	printf("Move out a bit while holding cube so that it won’t hit PVC \n");
	quick_burst(200, FORWARD);

	printf("Pivot ccw around left wheel\n");
	pivot(LEFT_MOTOR, 20, CCW);

	printf("Move forward a bit \n");
	quick_burst(500, FORWARD);

    //////////////////// Phase 5: Firefighter 3 ////////////////////

	printf("Align with tape backwards \n");
	align_with_tape(BACKWARD, false);

	printf("Pivot cw around left wheel\n");
	pivot(LEFT_MOTOR, 3, CW);

	printf("Open claw \n");
	set_claw_to(CLAW_OPEN);

	printf("Go to firefighter pole \n");
	quick_burst(720, BACKWARD);

	printf("Grab second firefighter \n");
	set_claw_to(CLAW_GRAB);

	printf("Move out a bit while holding cube so that it won’t hit PVC \n");
	quick_burst(300, FORWARD);

	printf("Pivot cw around left wheel\n");
	pivot(LEFT_MOTOR, 46, CW);

	printf("Move back to deliver cubes \n");
	quick_burst(1000, BACKWARD);

	printf("Open claw \n");
	set_claw_to(CLAW_OPEN);

    //////////////////// Phase 6: Firefighter 4 ////////////////////
	quick_burst(800, FORWARD);

	printf("Pivot ccw around right wheel\n");
	pivot(RIGHT_MOTOR, 90, CCW);



	return 0;
}

///////////////////////
///    Movements    ///
///////////////////////

/* Set velocities of the wheels for a specific amount of time */
void move(int L_speed, int R_speed, int time) {
	motor(LEFT_MOTOR, L_speed);
	motor(RIGHT_MOTOR, R_speed);
	msleep(time);
	ao();
}

/* Move at maximum velocity in a certain direction for a specified period of time */
void quick_burst(int time, Direction direction) {
	motor(LEFT_MOTOR, 100 * direction);
	motor(RIGHT_MOTOR, 96 * direction);
	msleep(time);
	ao();
	msleep(175);
}

/* Align with a line of tape horizontally with front or back sensors */
void align_with_tape(Direction direction, bool useFrontSensors) {
	// align with the tape going *across* the robot, not from back to front
	// normal align_with_tape uses front

	int leftBlack = useFrontSensors ? FRONT_LEFT_BLACK : BACK_LEFT_BLACK;
	int rightBlack = useFrontSensors ? FRONT_RIGHT_BLACK : BACK_RIGHT_BLACK;
	int leftIR = useFrontSensors ? FRONT_LEFT_IR : BACK_LEFT_IR;
	int rightIR = useFrontSensors ? FRONT_RIGHT_IR : BACK_RIGHT_IR;

	int left = analog(leftIR);
	int right = analog(rightIR);
	// int i = 0;

	while ( left <= leftBlack || right <= rightBlack ) {
		if (left > leftBlack) {
			motor(LEFT_MOTOR, -10 * direction);
			motor(RIGHT_MOTOR, 28 * direction);
		} else if (right > rightBlack) {
			motor(RIGHT_MOTOR, -10 * direction);
			motor(LEFT_MOTOR, 28 * direction);
		} else {
			motor(LEFT_MOTOR, 31 * direction);
			motor(RIGHT_MOTOR, 28 * direction);
		}

		//i++;
		left = analog(leftIR);
		right = analog(rightIR);
		msleep(2);
	}
	ao();
}

/* Move along the long strip of tape that spans most of the board until both front sensors see black */
void move_along_main_street() {
	int left = analog(FRONT_LEFT_IR);
	int right = analog(FRONT_RIGHT_IR);

	// left white = 2000
	// right white = 1500

	while ((left <= FRONT_LEFT_BLACK || right <= FRONT_RIGHT_BLACK) || counter < 1400) { // 250 250
		/*if (left < FRONT_LEFT_BLACK && right < FRONT_RIGHT_BLACK) {
			motor(LEFT_MOTOR, 98);
			motor(RIGHT_MOTOR, 100);

		}*/ if (left > FRONT_LEFT_BLACK) {
			motor(LEFT_MOTOR, 80);
			motor(RIGHT_MOTOR, 100);
			//printf("yup left sees black\n");
		} else if (right > FRONT_RIGHT_BLACK) { // right > RIGHT_WHITE
			motor(LEFT_MOTOR, 100);
			motor(RIGHT_MOTOR, 75);
			//printf("yup right sees black\n");

		} else {
			motor(LEFT_MOTOR, 98);
			motor(RIGHT_MOTOR, 100);
		}

		msleep(1);
		counter++;
		//if (counter % 5 == 0) printf("%d\n", counter);
		left = analog(FRONT_LEFT_IR);
		right = analog(FRONT_RIGHT_IR);
	}

	ao();
}


//////////////////////
///    Rotation    ///
//////////////////////

/* Pivot a certain angle in a specified direction around a specified wheel */
void pivot(int pivot, double degrees, Direction direction) {
	// slightly more powerful than should be for the specified angle to be achieved
	if (pivot == LEFT_MOTOR) {
		motor(RIGHT_MOTOR, 70 * direction);
	} else {
		motor(LEFT_MOTOR, -70 * direction);//22
	}

	double revolutionTime = pivot == 0 ? 6400.0 : 6300.0;
	msleep(revolutionTime * degrees / 360.0);
	ao();
}

/* Rotate a certain angle in a specified direction about the center */
void rotate(double degrees, Direction direction) {
	motor(RIGHT_MOTOR, 33 * direction);
	motor(LEFT_MOTOR, -33 * direction);

	msleep(6850.0 * degrees / 360.0);
	ao();
}


////////////////////
///    Servos    ///
////////////////////

/* Set claw to a position */
void set_claw_to(ClawPosition endPos) {
	enable_servo(CLAW);
	set_servo_position(CLAW, endPos);
	msleep(300);
}

/* Set claw to a position using a slower motion */
void set_claw_slowly_to(int endPos) {
	int currentPos = get_servo_position(port);
	bool testGreaterThan = currentPos > endPos; // continue loop while startPos is greater than
	int increment = testGreaterThan ? -2 : 2;

	while ((testGreaterThan && currentPos > endPos) || (!testGreaterThan && currentPos < endPos)) {
		currentPos += increment;
		set_servo_position(port, currentPos);
		msleep(3);
	}
	// for a good measure of how quick the servo moves, use the ratio of increment to msleep (in this case, 3/3) to compare to other speeds
}

/* Reset claw position */
void restart() {
	enable_servo(CLAW);
	set_servo_position(CLAW, CLAW_START);
}
