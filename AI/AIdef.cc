#include "Player.hh"
#define PLAYER_NAME Knybel

struct PLAYER_NAME : public Player {

static Player* factory () {
  return new PLAYER_NAME;
}
/*modificar desde aquí:

Propiedades del juego:
Moria (terreno peligroso) = 56X56 -> Cave
Battlefield (terreno total)= 60x60 -> Outside+Cave
Cave (zona segura)
Tesoros: hay 80, solo los pueden coger duendes
Roca: contiene Cave, Tesoro o Abismo (5 hits y cae)
Granito: pared NO atravesable
Abismo: muerte semi-segura; si no está vacío hay un monstruo que no puedo matar
Orcos: max hay 20; 75Hp, 15-30 damage
Trolls: max hay 4; 500Hp,50-150 damage
Balrog: muerte semi-segura; huir

Características del juego: 
unidades malas -> orco,troll,balrog
unidades buenas -> unit(cell(p_aux).id).player==me()
unidades enemigas -> unit(cell(pos).id).player!=me()
unidades mias ->unit(id).owner = me()s

Direcciones: Para atacar movemos en esa direccion
Bottom == 0 (v)
BR == 1 (v >)
Right == 2 (>)
RT == 3 (> ^)
Top == 4 (^)
TL == 5 (< ^)
Left == 6 (<)
LB == 7 (< v)
None (no se mueve)

*/

typedef vector<vector<int>> matint;
typedef vector<vector<bool>> matbool;
typedef vector<Dir> direcciones;
typedef pair<Pos,Dir> pos_dir;
const int MAX_INT = 1e9;

direcciones Vdir ={Bottom,Top,Right,Left,BR,RT,TL,LB};
//posibles direcciones (hasta 4 mago, hasta 8 enano)
//0 4 2 6 1 3 5 7


vector<int> peloton_de_tierra;
//dwarves: 100Hp, 20-40 damage (h, v y d)

vector<int> wizards_army;
//wizards: 50 Hp, heals adyacentes (h y v)
//wizards no cogen tesoros

bool pos_valida(Pos p){
	return pos_ok(p) and cell(p).type != Abyss and cell(p).type != Granite and cell(p).type != Rock;
}
bool pos_tesoro(Pos p){
	return pos_ok(p) and cell(p).type != Abyss and cell(p).type != Granite;
}
bool ocupat(Pos p){
	return cell(p).id!=-1 and unit(cell(p).id).player==me();
}

Dir huye(Dir d, Pos p){
	//encontrar la dirección opuesta a d
	//{Bottom,Top,Right,Left,BR,RT,TL,LB};
	if(d == Bottom){
		if(pos_valida(p+Top)) return Top;
		else if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+TL)) return TL;
		else if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+Left)) return Left;
		else if(pos_valida(p+BR)) return BR;
		else if(pos_valida(p+LB)) return LB;
	}
	else if(d == Top){
		if(pos_valida(p+Bottom)) return Bottom;
		else if(pos_valida(p+BR)) return BR;
		else if(pos_valida(p+LB)) return LB;
		else if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+Left)) return Left;
		else if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+TL)) return TL;
	}
	else if(d == Left){
		if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+BR)) return BR;
		else if(pos_valida(p+Top)) return Top;
		else if(pos_valida(p+Bottom)) return Bottom;
		else if(pos_valida(p+LB)) return LB;
		else if(pos_valida(p+TL)) return TL;
	}
	else if(d == Right){
		if(pos_valida(p+Left)) return Left;
		else if(pos_valida(p+LB)) return LB;
		else if(pos_valida(p+TL)) return TL;
		else if(pos_valida(p+Top)) return Top;
		else if(pos_valida(p+Bottom)) return Bottom;
		else if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+BR)) return BR;
	}
	else if(d == BR){
		if(pos_valida(p+TL)) return TL;
		else if(pos_valida(p+Top)) return Top;
		else if(pos_valida(p+Left)) return Left;
		else if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+LB)) return LB;
		else if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+Bottom)) return Bottom;
	}
	else if(d == RT){
		if(pos_valida(p+LB)) return LB;
		else if(pos_valida(p+Bottom)) return Bottom;
		else if(pos_valida(p+Left)) return Left;
		else if(pos_valida(p+BR)) return BR;
		else if(pos_valida(p+TL)) return TL;
		else if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+Top)) return Top;
	}
	else if(d == TL){
		if(pos_valida(p+BR)) return BR;
		else if(pos_valida(p+Bottom)) return Bottom;
		else if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+LB)) return LB;
		else if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+Top)) return Top;
		else if(pos_valida(p+Left)) return Left;
	}
	else if(d == LB){
		if(pos_valida(p+RT)) return RT;
		else if(pos_valida(p+Top)) return Top;
		else if(pos_valida(p+Right)) return Right;
		else if(pos_valida(p+TL)) return TL;
		else if(pos_valida(p+BR)) return BR;
		else if(pos_valida(p+Left)) return Left;
		else if(pos_valida(p+Bottom)) return Bottom;
	}
	return d;
}

//enemigos
int Operacion_Urano(Pos p, Dir& d){
	matbool visitat(60,vector<bool>(60,false));
	matint distancias(60,vector<int>(60,MAX_INT));
	distancias[p.i][p.j] = 0;
	visitat[p.i][p.j] = true;
	queue<Dir> qdir;
	queue<Pos> qpos;
	for(int i=0;i<8;++i){
		Pos aux = p+Vdir[i];
		if(pos_valida(aux) and not ocupat(aux)){
			qpos.push(aux);
			qdir.push(Vdir[i]);
			distancias[aux.i][aux.j] = 1;
		}
	}//llenado inicial de las adyacentes (distanica inicial = 1)
	while(not qpos.empty()){
		Pos p_aux = qpos.front();
		qpos.pop();
		Dir d_aux = qdir.front();
		qdir.pop();
		if(not visitat[p_aux.i][p_aux.j]){
			visitat[p_aux.i][p_aux.j] = true;
			if(cell(p_aux).id!=-1 and unit(cell(p_aux).id).player!=me() and (unit(cell(p_aux).id).type == Dwarf or unit(cell(p_aux).id).type == Wizard or unit(cell(p_aux).id).type == Orc) ){
				d = d_aux;
				return distancias[p_aux.i][p_aux.j];
			}
			
			for(int j = 0; j<8;++j){
				if(pos_valida(p_aux+Vdir[j])){
					Pos p_aux2 = p_aux+Vdir[j];
						qpos.push(p_aux2);
						qdir.push(d_aux);
						distancias[p_aux2.i][p_aux2.j] = distancias[p_aux.i][p_aux.j]+1;
				}
			}
		}
	}
	return -1;
}
//tesoros
int Operacion_Surgeon(Pos p, Dir& d){
	matbool visitat(60,vector<bool>(60,false));
	matint distancias(60,vector<int>(60,MAX_INT));
	distancias[p.i][p.j] = 0;
	visitat[p.i][p.j] = true;
	queue<Dir> qdir;
	queue<Pos> qpos;
	for(int i=0;i<8;++i){
		Pos aux = p+Vdir[i];
		if(pos_tesoro(aux) and not ocupat(aux)){
			qpos.push(aux);
			qdir.push(Vdir[i]);
			distancias[aux.i][aux.j] =1;
			if(cell(p).type == Rock) distancias[aux.i][aux.j]+=cell(aux).turns;
		}
	}
	while(not qpos.empty()){
		Pos p_aux = qpos.front();
		qpos.pop();
		Dir d_aux = qdir.front();
		qdir.pop();
		if(not visitat[p_aux.i][p_aux.j]){
			visitat[p_aux.i][p_aux.j] = true;
			if(cell(p_aux).treasure){
				d = d_aux;
				return distancias[p_aux.i][p_aux.j];
			}
			
			for(int j = 0; j<8;++j){
				if(pos_tesoro(p_aux+Vdir[j])){
					Pos p_aux2 = p_aux+Vdir[j];
						qpos.push(p_aux2);
						qdir.push(d_aux);
						if((distancias[p_aux.i][p_aux.j]+1+cell(p_aux2).turns) < distancias[p_aux2.i][p_aux2.j]) distancias[p_aux2.i][p_aux2.j] = distancias[p_aux.i][p_aux.j]+cell(p_aux2).turns+1;
				}
			}
		}
	}
	return -1;
}
//aliados
int Operacion_Windsor(Pos p, Dir& d){
	matbool visitat(60,vector<bool>(60,false));
	matint distancias(60,vector<int>(60,MAX_INT));
	distancias[p.i][p.j] = 0;
	visitat[p.i][p.j] = true;
	queue<Dir> qdir;
	queue<Pos> qpos;
	for(int i=0;i<4;++i){
		Pos aux = p+Vdir[i];
		if(pos_valida(aux) and not ocupat(aux)){
			qpos.push(aux);
			qdir.push(Vdir[i]);
			distancias[aux.i][aux.j] =1;
		}
	}
	while(not qpos.empty()){
		Pos p_aux = qpos.front();
		qpos.pop();
		Dir d_aux = qdir.front();
		qdir.pop();
		if(not visitat[p_aux.i][p_aux.j]){
			visitat[p_aux.i][p_aux.j] = true;
			if(cell(p_aux).id!=-1 and cell(p_aux).owner==me() and unit(cell(p_aux).id).type == Dwarf){
				d = d_aux;
				return distancias[p_aux.i][p_aux.j];
			}
			
			for(int j = 0; j<4;++j){
				if(pos_valida(p_aux+Vdir[j])){
					Pos p_aux2 = p_aux+Vdir[j];
						qpos.push(p_aux2);
						qdir.push(d_aux);
						distancias[p_aux2.i][p_aux2.j] = distancias[p_aux.i][p_aux.j]+1;
					
					
				}
			}
		}
	}
	return -1;
}

//distancia a balrog
int retirese_kowalski(Pos p, Dir& d, bool mago){
	int max = 8;
	if(mago) max = 4;
	matbool visitat(60,vector<bool>(60,false));
	matint distancias(60,vector<int>(60,MAX_INT));
	distancias[p.i][p.j] = 0;
	visitat[p.i][p.j] = true;
	queue<Dir> qdir;
	queue<Pos> qpos;
	for(int i=0;i<max;++i){
		Pos aux = p+Vdir[i];
		if(pos_ok(aux)){
			qpos.push(aux);
			qdir.push(Vdir[i]);
			distancias[aux.i][aux.j] =1;
		}
	}
	while(not qpos.empty()){
		Pos p_aux = qpos.front();
		qpos.pop();
		Dir d_aux = qdir.front();
		qdir.pop();
		if(not visitat[p_aux.i][p_aux.j]){
			visitat[p_aux.i][p_aux.j] = true;
			if(cell(p_aux).id!=-1 and cell(p_aux).id == balrog_id()){
				d = huye(d_aux,p);
				return distancias[p_aux.i][p_aux.j];
			}
			
			for(int j = 0; j<max;++j){
				if(pos_ok(p_aux+Vdir[j])){
					Pos p_aux2 = p_aux+Vdir[j];
					distancias[p_aux2.i][p_aux2.j] = distancias[p_aux.i][p_aux.j]+1;
					if(distancias[p_aux2.i][p_aux2.j]<6){
						qpos.push(p_aux2);
						qdir.push(d_aux);
					}
				}
			}
		}
	}
	return -1;
}

void pico_y_pala(){
	peloton_de_tierra = dwarves(me());
	for(int soldado : peloton_de_tierra){
	  	Pos p = unit(soldado).pos;
	  	int manhattan=-1, Long_john_silver=-1, black_hole=-1;
	  	Dir www,html,no_way_back;
	  	black_hole=retirese_kowalski(p,no_way_back,false);
	  	Dir rumbo;
	  	//manhattan==distancia a enemigo; long_john_silver == distancia a tesoro
	  	//www==direccion a enemigo; html == direccion a tesoro
	  	if(black_hole!=-1 and black_hole<=4) rumbo = no_way_back;
	  	//huyo
		else if((peloton_de_tierra.size()<30 and cell(p).type == Outside) or (nb_treasures(me())>peloton_de_tierra.size())){
			manhattan = Operacion_Urano(p,www);
			rumbo = www;
		}
		//solo ataco
	  	else{
	  		manhattan = Operacion_Urano(p,www);
	  		Long_john_silver = Operacion_Surgeon(p,html);
	  		if((manhattan>Long_john_silver) or (peloton_de_tierra.size()>50 and manhattan>2)) rumbo = html;
	  	//si un tesoro está más cerca que un enemigo, voy a por el tesoro
	  		else rumbo=www;
	  	}
	  	command(soldado,rumbo);
	}
}

void avada_kedabra(){
	wizards_army = wizards(me());
	for(int mortifago : wizards_army){
		Pos p = unit(mortifago).pos;
		int abeja_reina =-1,black_hole=-1;
		Dir rumbo,no_way_back;
	black_hole=retirese_kowalski(p,no_way_back,true);
	if(black_hole!=-1 and black_hole<=4) rumbo = no_way_back;
	else{
		abeja_reina = Operacion_Windsor(p,rumbo);
		if(abeja_reina != -1) command(mortifago,rumbo);
		}
	}
}



//no modificar a partir de aquí.
virtual void play (){
	pico_y_pala();
	avada_kedabra();
}
};
RegisterPlayer(PLAYER_NAME);