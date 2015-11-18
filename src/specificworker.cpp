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
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.<
 * 
 * 
 * 	CONTROLLER
 * 
 */
#include "specificworker.h"
#include <cmath>

const float ANCHO_ROBOT = 400;
const float MARGEN = 100;

/**
* \brief Default constructor
*/
SpecificWorker::SpecificWorker(MapPrx& mprx) : GenericWorker(mprx)
{
  this->inner= new InnerModel("/home/salabeta/robocomp/files/innermodel/simpleworld.xml");
  state = State::IDLE;
  distancia=0;
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	
}

bool SpecificWorker::heLlegado()
{
    QVec t = inner->transform("rgbd", marca, "world");
    float distancia = t.norm2();
    qDebug() << "Distancia: " << distancia;
    if( distancia < 0.4) return true;
    else return false;
    
}


bool SpecificWorker::hayCaminoLibre()
{
  float x, z;   
  for(auto p: ldata)
  {
    x = p.dist*sin(p.angle);
    z = p.dist*cos(p.angle);
    if((fabs(x) < (ANCHO_ROBOT/2 + MARGEN)) && (z < (ANCHO_ROBOT/2  + 200))){
      
      return false;
    }
  }
  return true;
}


bool SpecificWorker::siHaySubOBjetivo()
{
  return true;
}

void SpecificWorker::crearObjetivo()
{

}

void SpecificWorker::irSubobjetivo()
{

}

void SpecificWorker::crearSubObjetivo()
{

}

void SpecificWorker::avanzar()
{
  //qDebug("Avanzando");
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
  timer.start(Period);
  return true;
}

void SpecificWorker::compute()
{
  try{
    differentialrobot_proxy->getBaseState(tbase);
    ldata = laser_proxy->getLaserData();
    inner->updateTransformValuesS("base",tbase.x,0,tbase.z,0,tbase.alpha,0);
    
    if(state == State::WORKING){
      if(!heLlegado()) {
	qDebug("No he llegado");
	/*if(hayCaminoLibre()) {
	  avanzar();
	}
	else if (siHaySubOBjetivo()) {
	  irSubobjetivo();
	} else { crearSubObjetivo(); }*/
      } else {
	qDebug("Ya he llegado");
      }
    }
  } catch(const Ice::Exception &e){
    std::cout << "Error leyendo de la camara" << e << std::endl;  
  }
  
}

float SpecificWorker::go(const TargetPose &target)
{
  //QMutexLocker ml(&mutex);
  marca = QVec::vec3(target.x, target.y, target.z);
  state=State::WORKING;
}

NavState SpecificWorker::getState()
{
   NavState estado;
   QMutexLocker ml(&mutex);
   switch(state){
	  case State::IDLE:
	    qDebug("-----ESTADO IDLE----");
	    estado.state="IDLE";
	    break;
	  case State::WORKING:
	    //qDebug("-----ESTADO WORKING---");
	    estado.state="WORKING";
	    break;
	  case State::FINISH:
	    qDebug("-----ESTADO FINISH---");
	    estado.state="FINISH";
	    break;
	}
	return estado;
}

void SpecificWorker::stop()
{
  //qDebug("Parado");
}






