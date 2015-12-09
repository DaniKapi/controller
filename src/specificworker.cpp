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
  graphicsView->setScene(&scene);
  graphicsView->show();
  graphicsView->scale(3,3);
  subObjetivo.activo = false;
}

/**
* \brief Default destructor
*/
SpecificWorker::~SpecificWorker()
{
	
}

bool SpecificWorker::heLlegado()
{
    QVec t = inner->transform("base", marca, "world");
    float distancia = t.norm2();
    qDebug() << "Distancia: " << distancia;
    if( distancia < 400) return true;
    else return false;
}

bool SpecificWorker::hayCaminoLibrehaciaelObjetivo(){
  QVec t = inner->transform("rgbd", marca, "world");
  float alpha = atan2(t.x(), t.z() );
  float distancia = t.norm2();
  bool encontrado = false;
  int i;
  for(i = 5; i < (int) ldata.size() - 5; i++)
  {
    if(ldata[i].angle < alpha){
      encontrado = true;
      break;
    }
  }
  
  if(encontrado){
    if(ldata[i].dist < distancia){
      return false;
    }
  } else {
    return false;
  }
  return true;      
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


bool SpecificWorker::HaySubOBjetivo()
{
  return subObjetivo.activo;
}

void SpecificWorker::irSubobjetivo()
{
  
  

}

void SpecificWorker::crearSubObjetivo()
{
	int i,j;
  	parar();
	buscarPuntos(i,j);
	subObjetivo.SubObjetivo = inner->laserTo("world","laser",ldata[i].dist,ldata[i].angle);
	qDebug() << subObjetivo.SubObjetivo;
	subObjetivo.activo = true;

}

void SpecificWorker::buscarPuntos(int &i, int &j)
{
	//Search the first increasing step from the center to the right
	const float R = 400; //Robot radius
	for(i=(int)ldata.size()/2; i>0; i--)
	{
		if( (ldata[i].dist - ldata[i-1].dist) < -R )
		{
			int k=i-2;
			while( (k > 0) and (fabs( ldata[k].dist*sin(ldata[k].angle - ldata[i-1].angle)) < R ))
			{ k--; }
			i=k;
			break;
		}
	}
	for(j=(int)ldata.size()/2; j<(int)ldata.size()-1; j++)
	{
		if( (ldata[j].dist - ldata[j+1].dist) < -R )
		{
			int k=j+2;
			while( (k < (int)ldata.size()-1) and (fabs( ldata[k].dist*sin(ldata[k].angle - ldata[j+1].angle)) < R ))
			{ k++; }
			j=k;
			break;
		}
	}  
}

void SpecificWorker::histogram()
{	
	static QGraphicsPolygonItem *p;
	static QGraphicsLineItem *l, *sr, *sl, *safety;

	const float SAFETY = 600;

	scene.removeItem(p);
	scene.removeItem(l);
	scene.removeItem(sr);
	scene.removeItem(sl);
	scene.removeItem(safety);

	int i, j;
	buscarPuntos(i,j);
	
	safety = scene.addLine(QLine(QPoint(0,-SAFETY/100),QPoint(ldata.size(),-SAFETY/100)), QPen(QColor(Qt::yellow)));
	sr = scene.addLine(QLine(QPoint(i,0),QPoint(i,-40)), QPen(QColor(Qt::blue)));
	sl = scene.addLine(QLine(QPoint(j,0),QPoint(j,-40)), QPen(QColor(Qt::magenta)));
	
	//DRAW		
	QPolygonF poly;
	int x=0;
	poly << QPointF(0, 0);
	
	for(auto d : ldata)
		poly << QPointF(++x, -d.dist/100); // << QPointF(x+5, d.dist) << QPointF(x+5, 0);
	poly << QPointF(x, 0);

	l = scene.addLine(QLine(QPoint(ldata.size()/2,0),QPoint(ldata.size()/2,-20)), QPen(QColor(Qt::red)));
	p = scene.addPolygon(poly, QPen(QColor(Qt::green)));
	
	scene.update();
	
	//select the best subtarget and return coordinates
}

void SpecificWorker::avanzar()
{
  qDebug("Avanzando");
  differentialrobot_proxy->setSpeedBase(100,0);
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
  timer.start(Period);
  return true;
}

void SpecificWorker::compute()
{
  try
  {
    differentialrobot_proxy->getBaseState(tbase);
    ldata = laser_proxy->getLaserData();
    inner->updateTransformValuesS("base",tbase.x,0,tbase.z,0,tbase.alpha,0);
    
    histogram();
    
    switch(state)
    {
      case State::INIT:
	state = State::IDLE;
	break;
	
      case State::IDLE:
	break;
	
      case State::WORKING:
	if(heLlegado()){
	  qDebug() << "Ya he llegado";
	  parar();
	  state = State::FINISH;
	  return;
	}
	if(hayCaminoLibre()){
	  avanzar();
	  return;
	}
	if (HaySubOBjetivo()){
	  irSubobjetivo();
	  return;
	} 
	crearSubObjetivo();
	break;
      case State::FINISH:
	  sleep(2);
	  state=State::IDLE;
	break;
      
    }
  } catch(const Ice::Exception &e){
    std::cout << "Error leyendo de la camara" << e << std::endl;  
  }
 
}

float SpecificWorker::go(const TargetPose &target)
{
  marca = QVec::vec3(target.x, target.y, target.z);
  marca.print("Marca");
  state=State::WORKING;
}

NavState SpecificWorker::getState()
{
   NavState estado;
   QMutexLocker ml(&mutex);
   switch(state){
      case State::INIT:
	estado.state="INIT";
	break;
      case State::IDLE:
	estado.state="IDLE";
	break;
      case State::WORKING:
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
  qDebug("Parado");
  differentialrobot_proxy->setSpeedBase(0,0); 
  state=State::IDLE;
}

void SpecificWorker::parar()
{
  try
  {
    differentialrobot_proxy->setSpeedBase(0,0); 
  } catch(Ice::Exception &ex) 
  {
    std::cout<<ex.what()<<std::endl;
   
  };

}
