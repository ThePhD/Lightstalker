#include "RayTracerCommand.h"
#include <thread>

RayTracerCommand::RayTracerCommand( ) : multisampler( Fur::in_place, 4, 4, std::default_random_engine( ) ), imagesize( 640, 480 ), threadcount( std::thread::hardware_concurrency() ), multithreading( true ), displaywindow( true ) {

}
