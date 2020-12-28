#pragma once

#include <iostream>
#include <vector>

#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

namespace common {
namespace dock {

namespace msm = boost::msm;
namespace mpl = boost::mpl;

// events
struct list_room {};
struct create_room {};
struct enter_room {};
struct ready {};
struct set_game {};
struct start_game {};
struct exit_ {};

class DockFront : public msm::front::state_machine_def<DockFront>{
 public:
  DockFront() = default;
  virtual ~DockFront() = default;

  template <class Event,class FSM>
  void on_entry(Event const& ,FSM&) 
  {
      std::cout << "entering" << std::endl;
  }
  template <class Event,class FSM>
  void on_exit(Event const&,FSM& ) 
  {
      std::cout << "leaving" << std::endl;
  }

  // The list of FSM states
  struct Lobby : public msm::front::state<> 
  {
      // every (optional) entry/exit methods get the event passed.
      template <class Event,class FSM>
      void on_entry(Event const&,FSM& ) {std::cout << "entering: Lobby" << std::endl;}
      template <class Event,class FSM>
      void on_exit(Event const&,FSM& ) {std::cout << "leaving: Lobby" << std::endl;}
  };
  struct Room : public msm::front::state<> 
  {	 
      template <class Event,class FSM>
      void on_entry(Event const& ,FSM&) {std::cout << "entering: Room" << std::endl;}
      template <class Event,class FSM>
      void on_exit(Event const&,FSM& ) {std::cout << "leaving: Room" << std::endl;}
  };

  struct Playing : public msm::front::state<> 
  {	 
      template <class Event,class FSM>
      void on_entry(Event const& ,FSM&) {std::cout << "entering: Playing" << std::endl;}
      template <class Event,class FSM>
      void on_exit(Event const&,FSM& ) {std::cout << "leaving: Playing" << std::endl;}
  };

  // transition actions
  void ListRoom(list_room const&)       { std::cout << "dock::list_room\n"; }
  void CreateRoom(create_room const&)    { std::cout << "dock::create_room\n"; }
  void EnterRoom(enter_room const&)   { std::cout << "dock::enter_room\n"; }
  void ExitDock(exit_ const&) { std::cout << "dock::exit_\n"; }
  void SetGame(set_game const&) { std::cout << "dock::set_game\n"; }
  void StartGame(start_game const&)        { std::cout << "dock::start_game\n"; }
  void Ready(ready const&)      { std::cout << "dock::ready\n"; }
  void GotoLobby(exit_ const&)      { std::cout << "dock::exit_\n"; }
  void ExitPlaying(exit_ const&)  { std::cout << "dock::exit_\n"; }

  struct transition_table : boost::mpl::vector<
  //    Start     Event        Target      Action                      Guard 
  //   +---------+------------+-----------+---------------------------+----------------------------+ 
  a_row< Lobby   , list_room   , Lobby    , &DockFront::ListRoom                                 >,
  a_row< Lobby   , create_room , Lobby    , &DockFront::CreateRoom                               >,
  a_row< Lobby   , enter_room  , Room     , &DockFront::EnterRoom                                >,
  a_row< Lobby   , exit_       , Lobby    , &DockFront::ExitDock                                 >,
  //   +---------+------------+-----------+---------------------------+----------------------------+ 
  a_row< Room    , set_game    , Room     , &DockFront::SetGame                                  >,
  a_row< Room    , start_game  , Playing  , &DockFront::StartGame                                >,
  a_row< Room    , ready       , Room     , &DockFront::Ready                                    >,
  a_row< Room    , exit_       , Lobby    , &DockFront::GotoLobby                                >,
  //   +---------+------------+-----------+---------------------------+----------------------------+ 
  a_row< Playing , exit_       , Lobby    , &DockFront::ExitPlaying                             >
  //   +---------+------------+-----------+---------------------------+----------------------------+ 
  > {};

  template <class FSM,class Event>
  void no_transition(Event const& e, FSM&,int state)
  {
      std::cout << "no transition from state " << state
          << " on event " << typeid(e).name() << std::endl;
  }

  typedef Lobby initial_state;
};

typedef msm::back::state_machine<DockFront> DockFSM;

}  // namespace dock
}  // namespace common