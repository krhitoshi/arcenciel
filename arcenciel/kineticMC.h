#ifndef ARC_KINETIC_MC_H
#define ARC_KINETIC_MC_H

class KineticMC{
 private:
  int    fileOutputInterval;
  int    displayOutputInterval;
  int    numStep;
  double temperature, time; /* ���١� ���� */

  void initialize();

 public:
  KineticMC();
  void loadInputFile();
  void printInputData();
};

#endif
