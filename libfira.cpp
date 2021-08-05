#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "../FIRAClient/clients/vision/visionclient.h"
#include "../FIRAClient/clients/referee/refereeclient.h"
#include "../FIRAClient/clients/actuator/actuatorclient.h"
#include "../FIRAClient/clients/replacer/replacerclient.h"

#define NUM_BOTS 3
#define CONECTION_TRIES 100

typedef struct {
    double x, y, angle, vx, vy, vangle;
} object_t;
typedef struct {
    object_t ball;
    object_t yellow_bots[NUM_BOTS];
    object_t blue_bots[NUM_BOTS];
} field_t;

VisionClient *vision = NULL;
fira_message::sim_to_ref::Environment environment;
field_t field;

RefereeClient *referee = NULL;
ReplacerClient *replacer = NULL;
ActuatorClient *actuator = NULL;

extern "C"
{
    // function name starts with client function
    // name_init for constructor
    // name_term for deconstructor

    ////////////////////// Vision //////////////////////// 

    // constructor for client
    // initializes both vision client and environment
    void vision_init(char *addr, uint16_t port)
    {
        std::cout << "Vision initialized on " << addr << ":" << port << "\n";
        vision = new VisionClient(addr, port);
        vision->run();
        environment = vision->getLastEnvironment();
    }

    // returns (bool) frame exists
    int vision_has_frame()
    {
        vision->run();
        environment = vision->getLastEnvironment();            
        return environment.has_frame();
    }

    // checks if frame exists and returns if it does
    // tries to get one and returns if successfull or not
    int vision_get_frame()
    {

        vision->run();
        environment = vision->getLastEnvironment();

        // If already conected, returns
        if (environment.has_frame()) return 1;

        // Try to conect for some repetitions
        int i = 0;
        while(!environment.has_frame() && i++ < CONECTION_TRIES){
            usleep(10000); // 10 milliseconds
            vision->run();
            environment = vision->getLastEnvironment();
        }

        return i < CONECTION_TRIES;
    }

    // update field struct with frame data
    int vision_update_field()
    {
        if (!vision_get_frame()) {
            return 0;
        }

        // gets a frame
        fira_message::Frame frame = environment.frame();

        // gets frame ball data
        fira_message::Ball ball = frame.ball();

        field.ball.x = ball.x();
        field.ball.y = ball.y();
        field.ball.vx = ball.vx();
        field.ball.vy = ball.vy();

        field.ball.angle = 0;
        field.ball.vangle = 0;

        // get both teams position and speed data
        for (int i = 0; i < NUM_BOTS; i++) {
            fira_message::Robot bot = frame.robots_blue(i);
            
            // blue team data
            // positions
            field.blue_bots[i].x = bot.x();
            field.blue_bots[i].y = bot.y();
            field.blue_bots[i].angle = bot.orientation();
            // speeds
            field.blue_bots[i].vx = bot.vx();
            field.blue_bots[i].vy = bot.vy();
            field.blue_bots[i].vangle = bot.vorientation();

            // yellow team data
            bot = frame.robots_yellow(i);
            // positions
            field.yellow_bots[i].x = bot.x();
            field.yellow_bots[i].y = bot.y();
            field.yellow_bots[i].angle = bot.orientation();
            // speeds
            field.yellow_bots[i].vx = bot.vx();
            field.yellow_bots[i].vy = bot.vy();
            field.yellow_bots[i].vangle = bot.vorientation();

        }

        return 1;
    }

    // returns team color and indexed bot x position
    double vision_robot_x(int index, bool get_yellow_bots)
    {
        return get_yellow_bots 
                ? field.yellow_bots[index].x 
                : field.blue_bots[index].x;
    }

    // returns team color and indexed bot y position
    double vision_robot_y(int index, bool get_yellow_bots)
    {
        return get_yellow_bots 
                ? field.yellow_bots[index].y
                : field.blue_bots[index].y;
    }

    // returns team color and indexed bot angle
    double vision_robot_angle(int index, bool get_yellow_bots)
    {
        return get_yellow_bots 
                ? field.yellow_bots[index].angle
                : field.blue_bots[index].angle;
    }

    // returns team color and indexed bot x speed
    double vision_robot_vx(int index, bool get_yellow_bots)
    {
        return get_yellow_bots 
                ? field.yellow_bots[index].vx 
                : field.blue_bots[index].vx;
    }

    // returns team color and indexed bot y speed
    double vision_robot_vy(int index, bool get_yellow_bots)
    {
        return get_yellow_bots 
                ? field.yellow_bots[index].vy
                : field.blue_bots[index].vy;
    }

    // returns team color and indexed bot angle speed
    double vision_robot_vangle(int index, bool get_yellow_bots)
    {
        return get_yellow_bots 
                ? field.yellow_bots[index].vangle
                : field.blue_bots[index].vangle;
    }

    // returns ball x position
    double vision_get_ball_x()
    {
        return field.ball.x;
    }

    // returns ball y position
    double vision_get_ball_y()
    {
        return field.ball.y;
    }

    // returns ball x speed
    double vision_get_ball_vx()
    {
        return field.ball.vx;
    }

    // returns ball y speed
    double vision_get_ball_vy()
    {
        return field.ball.vy;
    }

    // closes network and terminates class
    void vision_term()
    {
        vision->close();
    }

    ////////////////////// Referee //////////////////////// 

    // intializes and runs referee client
    void referee_init(char *addr, uint16_t port)
    {
        std::cout << "Referee initialized on " << addr << ":" << port << "\n";
        referee = new RefereeClient(addr, port);
        referee->run();
    }

    // update referee data
    void referee_update()
    {
        referee->run();
    }
    
    // return referee info from:
    // enum Foul : int {
    //     FREE_KICK = 0,
    //     PENALTY_KICK = 1,
    //     GOAL_KICK = 2,
    //     FREE_BALL = 3,
    //     KICKOFF = 4,
    //     STOP = 5,
    //     GAME_ON = 6,
    //     HALT = 7,
    // };
    int referee_get_interrupt_type()
    {
        return referee->getLastFoul();
    }

    // return referee interupt info color from:
    // enum Color : int {
    //     BLUE = 0,
    //     YELLOW = 1,
    //     NONE = 2,
    // };
    int referee_interrupt_color()
    {
        return referee->getLastFoulColor();
    }

    // return referee quadrant info from:
    // enum Quadrant : int {
    //     NO_QUADRANT = 0,
    //     QUADRANT_1 = 1,
    //     QUADRANT_2 = 2,
    //     QUADRANT_3 = 3,
    //     QUADRANT_4 = 4,
    // };
    int referee_get_interrupt_quadrant()
    {
        return referee->getLastFoulQuadrant();
    }

    // descontructor cuts network conection
    void referee_term()
    {
        referee->close();
    }

    ////////////////////// Actuator //////////////////////// 

    // initialize and run constructor
    // init with string address and integer port
    // set team color
    // color is bool my_robots_are_yelloy
    void actuator_init(char *addr, uint16_t port, bool mray)
    {
        std::cout << "Actuator initialized on " << addr << ":" << port << "\n";
        actuator = new ActuatorClient(addr, port);
        actuator->setTeamColor(mray
                                ? VSSRef::Color::YELLOW 
                                : VSSRef::Color::BLUE );
    }

    // send a command to team with preset color
    // robot index (0, 1, 2) with left and right motor speed
    void actuator_send_command(int index, double left, double right)
    {
        actuator->sendCommand(index, left, right);
    }

    // closes network and terminates class
    void actuator_term()
    {
        actuator->close();
    }

    ////////////////////// Replacer //////////////////////// 
    
    // initialize class with address, port number and color 
    void replacer_init(char *addr, uint16_t port, bool mray)
    {
        std::cout << "Replacer initialized on " << addr << ":" << port << "\n";
        replacer = new ReplacerClient(addr, port);
        replacer->setTeamColor(mray
                                ? VSSRef::Color::YELLOW 
                                : VSSRef::Color::BLUE );
    }

    // place our team robot with index to x and y position and angle
    void replacer_place_robot(int index, 
                                double pos_x, 
                                double pos_y, 
                                double angle )
    {
        replacer->placeRobot(index, pos_x, pos_y, angle);
    }

    // actualy sends the frame of placed robots to the simulator
    void replacer_send_frame()
    {
        replacer->sendFrame();
    }

    // descontructor cuts network conection
    void replacer_term()
    {
        replacer->close();
    }

}
