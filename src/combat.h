#include <iostream>
#include <map>
#include <dlib/matrix.h>  // http://dlib.net/matrix_ex.cpp.html
#include <dlib/svm.h>

const int n_rounds = 5;
const int n_training_rounds = 3;
const int n_players = 5;
const int damageDie = 4;
const int nFeatures = 8;

// For the DLib SVM routines
//typedef dlib::matrix<double, 2, 1> sample_type;
//typedef dlib::radial_basis_kernel<sample_type> kernel_type;

class Player {
public:
  Player(int, std::string);
  std::string name;
  int ssn;  // the player id, will be serial at first
  int str, con, dex, wis, itl, cha;
  int ac, hp;
  std::vector<double> combatLabels;
  std::vector<std::vector<int>> combatLog;
  std::vector<int> getFeatures();
  void sleep();
};


std::map<int, int> abilityMods;

std::vector<Player> init();
void loadAbilityModifiers();
int run_tournament(std::vector<Player> &players);
int fight(Player &me, Player &you);
int threeD6();
int rollDie(int, int);
int abilityCheck(int);

int combatTurn(Player &first, Player &second);
void report();
double combatTraining(Player &player);
