#include <iostream>
#include <cmath>
#include <unistd.h>

#include "vrpn_Text.h"
#include "vrpn_Tracker.h"
#include "vrpn_Analog.h"
#include "vrpn_Button.h"
#include "vrpn_Connection.h"

// Initially based on http://www.vrgeeks.org/vrpn/tutorial---vrpn-server

class simTracker : public vrpn_Tracker
{
public:
  simTracker( vrpn_Connection *c = 0 );
  virtual ~simTracker() {};

  virtual void mainloop();
  
protected:
  struct timeval _timestamp;
};

simTracker::simTracker( vrpn_Connection *c /*= 0 */ ) :
  vrpn_Tracker( "PPT0", c )
{
}

void simTracker::mainloop()
{
  vrpn_gettimeofday(&_timestamp, NULL);

  vrpn_Tracker::timestamp = _timestamp;

  // We will just put a fake data in the position of our tracker
  static float angle = 0; angle += 0.001f;

  // the pos array contains the position value of the tracker
  // XXX Set your values here
  pos[0] = sinf( angle ); 
  pos[1] = 0.0f;
  pos[2] = cosf( angle );

  // the d_quat array contains the orientation value of the tracker, stored as a quaternion
  // XXX Set your values here
  d_quat[0] = 0.0f;
  d_quat[1] = 0.0f;
  d_quat[2] = 0.0f;
  d_quat[3] = 1.0f;

  char msgbuf[1000];

  d_sensor = 0;

  int  len = vrpn_Tracker::encode_to(msgbuf);

  if (d_connection->pack_message(len, _timestamp, position_m_id, d_sender_id, msgbuf,
				 vrpn_CONNECTION_LOW_LATENCY))
    {
      fprintf(stderr,"can't write message: tossing\n");
    }

  server_mainloop();
}

int main(int argc, char *argv[])
{
  // Creating the network server
  // Deprecated
  vrpn_Connection* m_Connection = vrpn_create_server_connection();
  
  // Creating the tracker
  simTracker* serverTracker = new simTracker(m_Connection );
  //  myAnalog*  serverAnalog  = new myAnalog(m_Connection );
  //  myButton*  serverButton  = new myButton(m_Connection );

  std::cout << "Created VRPN server." << std::endl;

  while(true)
    {
      serverTracker->mainloop();
      //serverAnalog->mainloop();
      //serverButton->mainloop();

      m_Connection->mainloop();

      // Calling Sleep to let the CPU breathe.
      usleep(1000);
    }
}
