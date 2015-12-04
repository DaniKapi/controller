/*
 *    Copyright (C) 2015 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
       \brief
       @author authorname
*/







#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#include <genericworker.h>
#include <innermodel/innermodel.h>

class SpecificWorker : public GenericWorker
{
  
  
Q_OBJECT
public:
	SpecificWorker(MapPrx& mprx);	
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);

	float go(const TargetPose &target);
	NavState getState();
	void stop();

public slots:
	void compute(); 
	

private:
  struct MarcaSubObjetivo{
    QVec SubObjetivo;
    bool activo;
  };
  
  MarcaSubObjetivo subObjetivo;
  QGraphicsScene scene;
  QVec marca;
  float distancia;
  QMutex mutex;
  bool hayObj;
  float calcularDist(float x,float y);
  bool heLlegado();
  bool HaySubOBjetivo();
  void histogram();
  void crearObjetivo();
  void irSubobjetivo();
  void crearSubObjetivo();
  void buscarPuntos(int &i, int &j);
  void avanzar();
  void parar();
  void pararFinish();
  bool hayCaminoLibre();
  bool hayCaminoLibrehaciaelObjetivo();
	void hayCaminoLibreha();
  enum class State {INIT, IDLE, WORKING, FINISH};
  State state;
  TBaseState tbase;
  TLaserData ldata;
  InnerModel *inner;
	
};

#endif

