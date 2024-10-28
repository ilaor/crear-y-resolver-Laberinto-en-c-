/**
 *
 * @file laberinto_hugo_ivan.cpp
 *
 * @brief 
 *
 * Programa para la generacion y resolucion de un laberinto.
 *
 * @version 1
 * @authors Ivan Lara y Hugo Bustamante
 * @date 23 Enero 2022
 *
 * @copyright Universidad de Valencia
 */
#include <iostream>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <thread>      
#include <chrono> 
#include <string>
 
using namespace std;

const int TAM = 10; 

/** 
 * 
 * @struct Estado
 * Registro para guardar la estado actual al recorrer el laberinto: fila, columna y orientacion.
 *
 * @var Estado::f
 * Almacena la fila
 * @var Estado::c
 * Almacena la columna
 * @var Estado::mira
 * Almacena la orientacion hacia donde se mira: N, S, E, O
 */
 
struct Estado
{
	unsigned int f;
	unsigned int c; // fila y columna
	char mira; // orientacion
};
/** 
 * 
 * @struct Nodo
 * Registro para guardar informacion de un Nodo
 *
 * @var Nodo::v
 * Almacena si la celda ya ha sido visitada
 * @var Nodo::idN
 * Almacena el identificador del vecino al Norte
 * @var Nodo::idE
 * Almacena el identificador del vecino al Este
 * @var Nodo::idS
 * Almacena el identificador del vecino al Sur
 * @var Nodo::idO
 * Almacena el identificador del vecino al Oeste
 */
struct Nodo
{
	bool v;
	int idN, idE, idS, idO;
};

/**
*
* @brief Definicion de tipo de dato MNodos de tamanyo 10x10 elementos de tipo Nodo
*
* @typedef MNodos
*/
typedef Nodo MNodos [TAM+2] [TAM+2];
typedef unsigned int camino[TAM*TAM];

/**
*
* @brief Definicion de tipo de dato MParedes de tamanyo 100x100 elementos de tipo bool,
* matriz de adyacencias
*
* @typedef MParedes
*/
typedef bool MParedes [TAM * TAM][TAM * TAM]; 

void Inicializar (MNodos, MParedes);
void Crear (unsigned int, MNodos, MParedes, HANDLE, camino, int );
void NoVisitada (camino, MNodos, MParedes, int);
void EjecutarMov(MNodos, MParedes, Estado &, string, HANDLE);
void Resolver (Estado &, MNodos, MParedes, HANDLE);
void Mostrar (MParedes, HANDLE);
void Leer (ifstream &, MParedes);
void Guardar (ofstream &, MParedes);
char Menu();
void IrA (unsigned int, unsigned int, HANDLE);



int main ()
{
	HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = FALSE;
	SetConsoleCursorInfo (h, &cursor);
	system("MODE CON COLS=42 LINES=26");
    //system ("color F0");
    
	MNodos n; 
	MParedes a; 
	char op;
	string nombre;
	ifstream f_in;
	ofstream f_out;
	camino c;
	int anyadir=0;
					
	do
	{	
		op = Menu();
		switch (op)
		{
			case 'A':
				Inicializar (n, a);
				Crear ((TAM - 1) * TAM, n, a, h, c, anyadir); //id de la Entrada
				Mostrar (a, h); 
				break;
			case 'B':
				cout << "Introduce el nombre: ";
				cin >> nombre;
				f_in.open (nombre.c_str());
				if (!f_in) cout << "Error al abrir el fichero " << nombre;
				else
				{
					Leer (f_in, a);
					f_in.close ();
				}
				break;	
			case 'C':
				cout << "Introduce el nombre: ";
				cin >> nombre;
				f_out.open (nombre.c_str());
				if (!f_out) cout << "Error al abrir el fichero " << nombre;
				else 
				{
					Guardar (f_out, a);
					f_out.close ();
				}
				break;	
			case 'D':
				Mostrar (a, h);
				break;
			case 'E':
				{
					Estado e = {TAM - 1, 0, 'N'}; //posicion actual en la E
					Resolver (e, n, a, h);	//siempre encuentra salida
					IrA (TAM, 0, h);
				}
		}
		system("pause");
	} 
	while (op != 'F');

	return 0;
}
/**
 *
 * Inicializa la matriz de nodos y adyacencias.
 *
 * @param [out] n matriz de nodos 
 * @param [out] v matriz de adyacencias
 *
 */
void Inicializar (MNodos n, MParedes a)
{
	for(int x=0; x<TAM*TAM; x++)	//PONER LAS CASILLAS +DARLES UNA ID A CADA UNA DE ESAS
	{
		for(int y=0; y<TAM*TAM; y++)
		{
			a[x][y]=false; 			//false es que se queda la pared
			
		}
	}
	for(int x=0; x<TAM; x++)
	{
		for(int y=0; y<TAM; y++)
		{
					
			n[x][y].idE= x*TAM+y+1;
			n[x][y].idS= (x+1)*TAM+y;
			n[x][y].idO= x*TAM-1+y;
			n[x][y].idN= (x-1)*TAM+y;
		}
	}
	
}
/**
 *
 * Crea el laberinto rellenando la matriz de adyacencias y eliminano tabiques usando 
 * una busqueda en profundida, usa retroceso.
 *
 * @param [in] id identificador de la celda actual
 * @param [out] n matriz de nodos 
 * @param [out] a matriz de adyacencias
 * @param [in] h handle de la ventana donde se muestra el laberinto
 *
 */
void Crear (unsigned int id, MNodos n, MParedes a, HANDLE h, camino c, int anyadir)
{
int i=id/TAM, j=id%TAM, random=0;
srand(time(NULL));
	do
	{
		if(i==0 ){									//limites (marca como vistada)
			n[i-1][j].v = true;
		}
		if(j==0 ){
			n[i][j-1].v = true;
		}
		if(j==9 ){
			n[i][j+1].v = true;
		}
		if(i==9 ){
			n[i+1][j].v = true;
		}
		Mostrar(a, h);
		Sleep(15);									//cronos
		n[i][j].v = true;
		random = rand()%4;
		switch (random)
		{
			case 0: 								//derecha
			if (n[i][j+1].v == false && j != 9)		//si no esta visitada y si no está en el límite
			{
				a[id] [n[i][j].idE]=true;			//quita la pared que hay entre la de el este(idE) y la id
				a[n[i][j].idE] [id]=true;			//preciso
				n[i][j].v = true; 					//marcar visitada (n)	
				j++;	
				id=j+(i*TAM);	
				anyadir++;
				c[anyadir]=id;					
			}break;
			
			case 1: 								//izquierda
			if (n[i][j-1].v == false && j != 0)
			{
				a[n[i][j].idO] [id]=true;
				a[id] [n[i][j].idO]=true;
				n[i][j].v = true;	
				j--;		
				id=j+(i*TAM);	
				anyadir++;
				c[anyadir]=id;
			}break;
			
			case 2: 								//subir
			if (n[i-1][j].v == false && i != 0)		//cuando la de arriba no esta por esta por visitar y hay pared
			{
				a[n[i][j].idN] [id]=true;
				a[id] [n[i][j].idN]=true;
				n[i][j].v = true;
				i--;								//tiene que bajar la i para darle el valor		
				id=j+(i*TAM);
				anyadir++;
				c[anyadir]=id;
			}break;
			
			case 3: 								//bajar
			if (n[i+1][j].v == false && i != 9)
			{
				a[id] [n[i][j].idS]=true;
				a[n[i][j].idS] [id]=true;
				n[i][j].v = true; 
				i++;
				id=j+(i*TAM);	
				anyadir++;	
				c[anyadir]=id;					//por ej. cuando anyadir sea 2 el camino tmb es do, significa que lleva dos movimientos	
				
			}break;
		}
	
	}while(n[i+1][j].v == false || n[i][j-1].v == false || n[i][j+1].v == false || n[i-1][j].v == false); 							//mientras que no se quede atrapado

	n[i][j].v=true;								//marca la casilla como visitada
	NoVisitada(c, n, a, anyadir);	

}
/**
 *
 * Devuelve una celda vecina que no ha sido aun visitada.
 * durante la construccion del lab.
 *
 * @param [in] id identificador de la celda actual
 * @param [in] n matriz de nodos 
 * @param [out] a matriz de adyacencias
 * @param [out] vecinaId identificador de la siguiente celda a visitar
 *
 * @return verdadero si aun quedan vecinas sin visitar, falso en caso contrario
 */
void NoVisitada (camino c, MNodos n, MParedes a, int anyadir)
{
	HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);		
	for(int q=anyadir; q>=0; q--)
	{
		int i=c[q]/TAM;
		int j=c[q]%TAM;
		if(n[i+1][j].v == false || n[i][j-1].v == false || n[i][j+1].v == false || n[i-1][j].v == false)
		{
			Crear(c[q], n, a, h, c, anyadir);
		}
		
		
	}
}
/**
 *
 * Actualiza el valor de mira y avanza si es posible 
 * usando un orden en las direcciones establecidas en funcion de la orientacion actual
 *
 * @param [in] n matriz de nodos 
 * @param [in] a matriz de adyacencias
 * @param [in, out] p estado actual
 * @param [in] ori cadena con el orden de evaluacion de los movimientos: ENOS, NOSE, OSEN, SENO
 * @param [in] h handle de la ventana donde se muestra el laberinto
 *
 */
void EjecutarMov(MNodos n, MParedes a, Estado & p, string ori, HANDLE h)
{
	char drc=16;
	char esq=17;
	char dalt=30;
	char baix=31;
	

	if(ori=="ENOS")
	{
		if(p.c==9 && p.f==0)
		{
			cout<<4;
			Sleep(1000);
			main();	
		}
		
		if(a[p.f * TAM + p.c][n[p.f][p.c].idE]  == true)
		{
			p.mira = 'E';
			cout<<4;
			Sleep(300);
			p.c++;
			Resolver(p,n,a,h);	
		}
		
		if(a[n[p.f][p.c].idN] [p.f * TAM + p.c] == true)
		{
			p.mira = 'N';
			cout<<4;
			Sleep(300);
			p.f--;
			Resolver(p,n,a,h);	
		}
		
		if(a[n[p.f][p.c].idO][p.f * TAM + p.c] == true)
		{
			p.mira = 'O';
			cout<<4;
			Sleep(300);
			p.c--;
			Resolver(p,n,a,h);	
		}
		
		if(a[p.f * TAM + p.c][n[p.f][p.c].idS]  == true)
		{
			p.mira = 'S';
			cout<<4;
			Sleep(300);
			p.f++;
			Resolver(p,n,a,h);	
		}
	}
		
	if (ori=="OSEN")
	{
		if(p.c==9 && p.f==9)
		{
			if(a[n[p.f][p.c].idO][p.f * TAM + p.c]  == true)
			{
				p.mira = 'O';
				cout<<4;
				Sleep(300);
				p.c--;
				Resolver(p,n,a,h);	
			}
			
			if(a[n[p.f][p.c].idN][p.f * TAM + p.c]  == true)
			{
				p.mira = 'N';
				cout<<4;
				Sleep(300);
				p.f--;
				Resolver(p,n,a,h);	
			}
		}

	
		else 
		{
			if(a[n[p.f][p.c].idO] [p.f * TAM + p.c] == true)
			{
				p.mira = 'O';
				cout<<4;
				Sleep(300);
				p.c--;
				Resolver(p,n,a,h);	
			}
			
			if(a[p.f * TAM + p.c][n[p.f][p.c].idS]  == true)
			{
				p.mira = 'S';
				cout<<4;
				Sleep(300);
				p.f++;
				Resolver(p,n,a,h);	
			}
			if(a[p.f * TAM + p.c][n[p.f][p.c].idE]  == true)
			{
				p.mira = 'E';
				cout<<4;
				Sleep(300);
				p.c++;
				Resolver(p,n,a,h);	
			}
			
			if(a[n[p.f][p.c].idN] [p.f * TAM + p.c] == true)
			{
				p.mira = 'N';
				cout<<4;
				Sleep(300);
				p.f--;
					Resolver(p,n,a,h);	
			}	
		}
	}

	if (ori=="NOSE")
	{
		if(a[n[p.f][p.c].idN] [p.f * TAM + p.c] == true)
		{
			p.mira = 'N';
			cout<<4;
			Sleep(300);
			p.f--;
			Resolver(p,n,a,h);	
		}	
		
		if(a[n[p.f][p.c].idO] [p.f * TAM + p.c] == true)
		{
			p.mira = 'O';
			cout<<4;
			Sleep(300);
			p.c--;
			Resolver(p,n,a,h);	
		}
		
		if(a[p.f * TAM + p.c][n[p.f][p.c].idS]  == true)
		{
			p.mira = 'S';
			cout<<4;
			Sleep(300);
			p.f++;
			Resolver(p,n,a,h);	
		}
		
		if(a[p.f * TAM + p.c][n[p.f][p.c].idE]  == true)
		{
			p.mira = 'E';
			cout<<4;
			Sleep(300);
			p.c++;
			Resolver(p,n,a,h);	
		}
	}		
	if (ori=="SENO")
	{
		
		if(p.f==9 &&p.c==9)
		{
			if(a[n[p.f][p.c].idN] [p.f * TAM + p.c] == true)
			{
				p.mira = 'N';
				cout<<4;
				Sleep(300);
				p.f--;
				Resolver(p,n,a,h);	
			}
				
			if(a[n[p.f][p.c].idO] [p.f * TAM + p.c] == true)
			{
				p.mira = 'O';
				cout<<4;
				Sleep(300);
				p.c--;
				Resolver(p,n,a,h);	
			}	
		}
		
		if(p.f==9)
		{
			if(a[p.f * TAM + p.c][n[p.f][p.c].idE]  == true)
			{
				p.mira = 'E';
				cout<<4;
				Sleep(300);
				p.c++;
				Resolver(p,n,a,h);	
			}
					
			if(a[n[p.f][p.c].idN] [p.f * TAM + p.c] == true)
			{
				p.mira = 'N';
				cout<<4;
				Sleep(300);
				p.f--;
				Resolver(p,n,a,h);	
			}
				
			if(a[n[p.f][p.c].idO] [p.f * TAM + p.c] == true)
			{
				p.mira = 'O';
				cout<<4;
				Sleep(300);
				p.c--;
				Resolver(p,n,a,h);	
			}	
		}
		
		else
		{
			if(a[p.f * TAM + p.c][n[p.f][p.c].idS]  == true)
			{
				p.mira = 'S';
				cout<<4;
				Sleep(300);
				p.f++;
				Resolver(p,n,a,h);	
			}
			
			if(p.c==9 && p.f==0)
			{
				cout<<4;
				Sleep(1000);
				main();
			}
			
			if(a[p.f * TAM + p.c][n[p.f][p.c].idE]  == true)
			{
				p.mira = 'E';
				cout<<4;
				Sleep(300);
				p.c++;
				Resolver(p,n,a,h);	
			}
					
			if(a[n[p.f][p.c].idN] [p.f * TAM + p.c] == true)
			{
				p.mira = 'N';
				cout<<4;
				Sleep(300);
				p.f--;
				Resolver(p,n,a,h);	
			}
				
			if(a[n[p.f][p.c].idO] [p.f * TAM + p.c] == true)
			{
				p.mira = 'O';
				cout<<4;
				Sleep(300);
				p.c--;
				Resolver(p,n,a,h);	
			}
		}
	}
}
/**
 *
 * Resuelve el laberinto.
 *
 * @param [in, out] p estado actual
 * @param [in] n matriz de nodos 
 * @param [in] a matriz de adyacencias
 * @param [in] h handle de la ventana donde se m uestra el laberinto
 *
 */
void Resolver (Estado & p, MNodos n, MParedes a, HANDLE h)
{
	Mostrar(a,h);
	IrA(p.f, p.c, h);
	do
	{
		if(p.mira=='N')
			EjecutarMov(n,a,p, "ENOS", h);
			
		if(p.mira=='S')
			EjecutarMov(n,a,p, "OSEN", h);
			
		if(p.mira=='O')
			EjecutarMov(n,a,p, "NOSE", h);
		
		if(p.mira=='E')
			EjecutarMov(n,a,p, "SENO", h);
			
	}while (p.f != 0 && p.c !=9);								//sale del laberinto
}

/**
 *
 * Muestra en la reticula la fila, la columna y la orientacion actual
 *
 * @param [in] p estado actual
 * @param [in] h handle de la ventana donde se muestra el laberinto
 *
 */
char Menu ()
{
	char op;
	
	system("cls");
        
        // Texto del menú
        cout << "Laberinto\n" << endl;
        cout << "----------------------------\n";
        cout << "| A. Generar.              |" << endl;
        cout << "| B. Leer desde fichero.   |" << endl;
        cout << "| C. Guardar a fichero.    |" << endl;
        cout << "| D. Mostrar.              |" << endl;
        cout << "| E. Resolver.             |" << endl;
        cout << "| F. Salir.                |" << endl;
        cout << "----------------------------\n";
        
        cout << "\nIngrese una opcion: ";
        cin >> op;
		return op;
}
/**
 *
 * Lee el laberinto (matriz de adyacencias) desde un fichero
 *
 * @param [out] f fichero
 * @param [in] v matriz de adyacencias
 *
 */ 
void Leer (ifstream & f, MParedes a)
{
	int x, y;
	for(y=0; x<100;x++)
	{
		for(y=0;y<100;y++)
		{
			f>>a[x][y];
		}
	
	}	
}
/**
 *
 * Guarda el laberinto (matriz de adyacencias) en un fichero
 *
 * @param [out] f fichero
 * @param [in] a matriz de adyacencias
 *
 */ 
void Guardar (ofstream & f, MParedes a)
{
	int x, y;
	for(y=0; x<100;x++)
	{
		for(y=0;y<100;y++)
		{
			f<<a[x][y]<<" ";
		}
	
	}
}
/**
 *
 *  Muestra la reticula, el laberinto
 *
 *  @param [in] a matriz de adyacencias
 *  @param [in] h handle de la ventana donde se muestra el laberinto
 *
 */ 
void Mostrar (MParedes a, HANDLE h)
{
	unsigned int i, j;
	COORD p = {0, 0};
	SetConsoleCursorPosition (h, p);  
	
	for (j = 0; j < TAM - 1; j++) cout << "+---";
    cout << "+   +" << endl; // Salida
	for (i = 0; i < TAM; i++) 
	{   
		if (i == TAM-1) cout << " "; // Entrada
		else  cout << "|";
		for (j = 0; j < TAM - 1; j++) 
    		if (a [i * TAM + j] [i * TAM + j + 1] == false) cout << "   |";
            else cout << "    ";   
        cout << "   |" << endl; 
        
        if (i < TAM-1)
		{
			for (j = 0; j < TAM; j++) 
	        	if (a [i * TAM + j] [(i + 1) * TAM + j] == false) cout <<"+---";
	       		else cout <<"+   ";
			cout << "+" << endl;
		}	
    }
    for (j = 0; j < TAM; j++) cout << "+---";
    cout << "+" << endl; 		
}
/**
 *
 * Establece una correspondencia entre filas y columnas
 * y las coordenas para colocar el cursor en la pantalla
 *
 * @param [in] fil fila de la celda
 * @param [in] col columna de la celda
 * @param [in] h handle de la ventana donde se muestra el laberinto
 *
 */ 
void IrA (unsigned int fil, unsigned int col, HANDLE h) 
{ 
    COORD p = { short(4 * col + 2), short(2 * fil + 1)};
	SetConsoleCursorPosition (h, p);   
}
