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
#include "specificworker.h"

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




bool SpecificWorker::he_Llegado()
{
  QVec r = QVec::vec3(tbase.x, 0, tbase.z);
  float  distancia= (r-marca).norm2();
  qDebug() << "marca" << marca;
  cout<<"la distancia es: "<<distancia<<endl;
  if( distancia < 300)
    return true;
  else
    return false;
  
}


bool SpecificWorker::hayCaminoLibre()
{
    
     float distMarca = inner->transform("rgbd", marca, "world").norm2();
     
     for(auto x: ldata)
     {
	cout<<x.dist<<endl;
	cout<<x.angle<<endl;
	
	if( x.dist =<  distMarca )
	{
	  cout<<"puedo ir directo"<<endl;
	  return true;
	}
    }
    return false;
}

void SpecificWorker::crearObjetivo()
{

}

bool SpecificWorker::siHaySubOBjetivo()
{

}


void SpecificWorker::MinionCurrando(){

    if(he_Llegado()){
      state=State::FINISH;
    }else if(hayCaminoLibre()){
      cout<<"subnormal"<<endl;
    }else if(siHaySubOBjetivo()){
    }else{
      crearObjetivo();
    }
  
  
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
  timer.start(Period);

	return true;
}

void SpecificWorker::compute()
{
 
  differentialrobot_proxy->getBaseState(tbase);
  ldata = laser_proxy->getLaserData();
  inner->updateTransformValuesS("base",tbase.x,0,tbase.z,0,tbase.alpha,0);
  
  
	switch(state){
	  case State::IDLE:
	    break;
	  case State::WORKING:
	    this->MinionCurrando();
	    break;
	  case State::FINISH:
	    state=State::IDLE;
	      break;
	  
	
	}
// 	try
// 	{
// 		camera_proxy->getYImage(0,img, cState, bState);
// 		memcpy(image_gray.data, &img[0], m_width*m_height*sizeof(uchar));
// 		searchTags(image_gray);
// 	}
// 	catch(const Ice::Exception &e)
// 	{
// 		std::cout << "Error reading from Camera" << e << std::endl;
// 	}
}

//////////////////////////////////////////777
////////////////////////////////////////////


float SpecificWorker::go(const TargetPose &target)
{
      cout<<"entrando marca"<<endl;
      QMutexLocker ml(&mutex);
      marca = QVec::vec3(target.x, target.y, target.z);
      if(state==State::IDLE)
      {
	state=State::WORKING;
      }
      return 0;
}

NavState SpecificWorker::getState()
{
  NavState estado;
   QMutexLocker ml(&mutex);
   switch(state){
	  case State::IDLE:
	    cout<<"-----ESTADO IDLE----"<<endl;
	    estado.state="IDLE";
	    break;
	  case State::WORKING:
	    cout<<"-----ESTADO WORKING---"<<endl;
	    estado.state="WORKING";
	    break;
	  case State::FINISH:
	    estado.state="FINISH";
	    break;
	
	}
	return estado;
    
}

void SpecificWorker::stop()
{
  cout<<"hola desde el segundo componente"<<endl;
}






