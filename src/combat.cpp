#include "combat.h"
#include <iostream>
#include <time.h>
#include <random>
#include <fstream>
#include <vector>
#include <map>
#include <boost/tokenizer.hpp>
#include <dlib/matrix.h>  // http://dlib.net/matrix_ex.cpp.html


int threeD6 () {
    return rand() % 6 + rand()%6 + rand()%6 +3;
}

int rollDie(int n_die, int n_sides) {
  int r = 0;
  for (int k = 0; k < n_die; k++) {
    r += rand() % n_sides + 1;
  }
  return r;
}

int abilityCheck(int playerAbility) {
  int base = rollDie(1,20);
  return base + abilityMods[playerAbility];
}

/*
  I had to write it this way because doing string split is C++ is stupid hard
 */
void loadAbilityModifiers() {
  abilityMods[1] = -5;
  abilityMods[2] = -4;
  abilityMods[3] = -4;
  abilityMods[4] = -3;
  abilityMods[5] = -3;
  abilityMods[6] = -2;
  abilityMods[7] = -2;
  abilityMods[8] = -1;
  abilityMods[9] = -1;
  abilityMods[10] = 0;
  abilityMods[11] = 0;
  abilityMods[12] = 1;
  abilityMods[13] = 1;
  abilityMods[14] = 2;
  abilityMods[15] = 2;
  abilityMods[16] = 3;
  abilityMods[17] = 3;
  abilityMods[18] = 4;
  abilityMods[19] = 4;
  abilityMods[20] = 5;
  abilityMods[21] = 5;
  abilityMods[22] = 6;
  abilityMods[23] = 6;
  abilityMods[24] = 7;
  abilityMods[25] = 7;
  abilityMods[26] = 8;
  abilityMods[27] = 8;
  abilityMods[28] = 9;
  abilityMods[29] = 9;
  abilityMods[30] = 10;
}

Player::Player(int id, std::string n) {
  name = n;
  ssn = id;
  str=rollDie(3,6);
  con=rollDie(3,6);
  dex=rollDie(3,6);
  wis=rollDie(3,6);
  itl=rollDie(3,6);  // can't use "int"
  cha=rollDie(3,6);

  ac = 10 + abilityMods[dex];
  hp = rollDie(1,10) + abilityMods[con];
  //combatLabels.set_size(1,0);
}

std::vector<int> Player::getFeatures() {
  std::vector<int> v;
  v.push_back(str);
  v.push_back(con);
  v.push_back(dex);
  v.push_back(wis);
  v.push_back(itl);
  v.push_back(cha);
  return v;
}

void Player::sleep() {

  std::cout
    << name
    << " (m,n): " << combatLog.size() <<  " " << combatLog[0].size() << std::endl;
  int m, n;
  m = combatLog.size();
  n = combatLog[0].size();

  //typedef dlib::matrix<double> sample_type(n,1);
  //typedef dlib::radial_basis_kernel<sample_type> kernel_type;

  dlib::matrix<int> M(m,n);
  for (int r = 0; r < m; r++) {
    for (int c = 0; c < n; c++) {
      /*
      std::cout << " r/c/v " << r
        << " / " << c
        << " / " << combatLog[r][c]
        << std::endl;
        */
      M(r,c) = combatLog[r][c];
    }
  }
  /*
  dlib::vector_normalizer<std::vector<double>> normalizer;
  std::vector<std::vector<double>> samples;
  */
}

double combatTraining(Player &player) {
  const int m = player.combatLog.size();
  //int m,n;
  //m = player.combatLog.size();
  //n = player.combatLog[0].size();
  const int n = player.combatLog[0].size();
  dlib::matrix<double, m,n> M;
  return 0.0;
}

/**
 The primary combat routine.  Based (obviously) on D&D rules but a bit simpler.
 */
int fight(Player &me, Player &you) {
  int meInit, youInit;
  meInit = abilityCheck(me.dex);
  youInit = abilityCheck(you.dex);
  if (meInit > youInit) {
    combatTurn(me, you);
  } else if (meInit < youInit) {
    combatTurn(you, me);
  } else if (rollDie(1,4) < 3){
    combatTurn(me, you);
  } else {
    combatTurn(you, me);
  }

  me.combatLog.push_back(you.getFeatures());
  you.combatLog.push_back(me.getFeatures());

  if (me.hp > you.hp) {
    //std::cout << "Me won..." << std::endl;
    me.combatLabels.push_back(1.0);
    you.combatLabels.push_back(0.);
  }   else if (me.hp < you.hp) {
    //std::cout << "You won..." << std::endl;
    me.combatLabels.push_back(0.);
    you.combatLabels.push_back(1.0);
  } else {
    //std::cout << "Nobody won..." << std::endl;
    me.combatLabels.push_back(0.);
    you.combatLabels.push_back(0.);
  }
  return 0;
}

int combatTurn(Player &first, Player &second) {
  if (rollDie(1,20) + abilityMods[first.str] > second.ac) {
    int damage = rollDie(1,damageDie) + abilityMods[first.str];
    second.hp -= damage;
    //std::cout << second.name << " takes " << damage << " damage, leaving " << second.hp << std::endl;
  }
  if (second.hp > 0 ) {
    if (rollDie(1,20) + abilityMods[second.str] > first.ac) {
        int damage = rollDie(1,damageDie) + abilityMods[second.str];
        first.hp -= damage;
        //std::cout << first.name << " takes " << damage << " damage, leaving " << first.hp << std::endl;
    }
  }
  return 0;
}

std::vector<Player> init() {
  // load ability modifiers
  loadAbilityModifiers();

  std::vector<std::string> v;
  std::vector<Player> players;

  // Populate names
  std::ifstream namefile;
  namefile.open("data/names.txt");
  std::string nm;
  while (std::getline(namefile, nm)) {
    v.push_back(nm);
  }

  // Initialize random generator
  std::random_device random_device;
  std::mt19937 engine{random_device()};
  std::uniform_int_distribution<int> dist(0, v.size() - 1);

  for (int i = 0; i < n_players; i++) {
    Player p(i, v[dist(engine)]);
    players.push_back(p);
  }
  return players;
}

int run_tournament(std::vector<Player> &players) {
  for (int t = 0; t < n_training_rounds; t++) {
    // This pairs everyone against each other for the moment.
    for (std::vector<Player>::iterator m = players.begin(); m != players.end(); ++m) {
      for (std::vector<Player>::iterator y = m; y != players.end(); ++y) {
        Player &me = *m;
        Player &you = *y;
        int winner = fight(me, you);
      }
    }
    for (std::vector<Player>::iterator m = players.begin(); m != players.end(); ++m) {
      Player &me = *m;
      m->sleep();
    }

  }
  return 0;
}

void report(std::vector<Player> players) {
  for (std::vector<Player>::iterator it = players.begin() ; it != players.end(); ++it) {
    int w = 0;
    for (auto& n : it->combatLabels) w += n;
    std::cout << it->name  << ": " << std::endl
      <<  " hp: " << it->hp
      << " str: " << it->str
      << " dex: " << it->dex
      << " con: " << it->con
      << " fights: " << it->combatLabels.size()
      << " wins: " << w
      << std::endl;
  }
}


int main(int argc, char* argv[]) {
  srand(time(NULL));
  std::cout << "Let's the game begin..." << std::endl;
  std::vector<Player> players = init();
  report(players);

  //int n = nada(players);
  std::cout << " *** *** ***" << std::endl;
  run_tournament(players);
  report(players);
  return 0;
}
