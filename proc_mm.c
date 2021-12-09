#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


#define EXISTE 0  
#define MID 0
#define TAM_PART 1
#define BASE 2
#define TAM_PROC 3
#define ARCHIVO "memory.bin"
#define ORDEN_EJ3 0
#define ORDEN_EJ4 1



int idCounter = 0;
int tree[2049][5];
//tabla contiene
//en columna 0 la direccion base
//columna 1 el tamanho de particion
//columna 2 el mid
//columna 3 el indice del nodo al que hace referencia
int tabla[2049][4];

int alojar(int size, int nodo);
int desalojar(int size, int nodo);
void inicializarTree(int nodo, int size, int base);
void cargarTree();
void guardarTree();
void mostrarTree();
void mostrarTabla();
int crearTabla();
void gotoxy(int x, int y);
void ordenarTabla(int size);


int main(int argc, char * argv []){
    //===============IDEA DE MODELADO
    //planteamos un arbol binario para modelar el sistema companhero
    // cada fila representa un nodo del arbol
    //cada columna trae informacion con respecto al nodo

    //la columna 0 es el MID... este es mayor a 1
    //si MID vale -1 representa un nodo no ocupado y sin hijos
    //si MID es 0 representa un nodo ocupado con hijos

    //la columna 1 es el tamanho de la particion
    //la columna 2 es la direccion base de la particion
    //la columna 3 es el tamanho del proceso alojado

    //PREPARAR TODO
    int alojado;
    FILE * f;
    f = fopen(ARCHIVO, "r+");
    if(errno == EXISTE){
        fclose(f);
        cargarTree();
    }
    else{
        //se crea el archivo
        f = fopen(ARCHIVO, "w+");
        //se inicia el grafo
        idCounter=1;
        inicializarTree(1, 1024, 0);
        //cargar el primer proceso;
        alojado = alojar(60, 1);
        fclose(f);
    }
   
    //interfaz
    char option[3];
    char input [10];
    printf("\n");
    printf("-n : <size> \n");
    printf("-f : <MID> \n");
    printf("-m : \n");
    printf("-M : \n");
    printf("Opcion: ");
    scanf("%s", option);

    
    //CASOS
    if(strcmp(option,"-n")==0){
        scanf("%s", input);
        int n = atoi(input);
        alojado = alojar(n, 1);
        if(alojado == 0){
            printf("M E M O R Y A L L O C A T I O N – Process size: %i \n", n);
            printf("Resultado: Error catastrofico en la asignacion... No hay espacio \n");
        }
    }
    else if(strcmp(option,"-f")==0){
        scanf("%s", input);
        int n = atoi(input);
        alojado = desalojar(n, 1);
        if(alojado==0){
            printf("M E M O R Y D E A L L O C A T I O N* Procces MID: %i\n", n);
        }
        else{
            printf("NO SE PUDO DESALOJAR PROCESO CON EL MID DADO.\n");
        }
    }
    else if(strcmp(option,"-m")==0){
        mostrarTabla(ORDEN_EJ3);
    }
    else if(strcmp(option,"-M")==0){
        mostrarTabla(ORDEN_EJ4);
    }


    //mostrarTree();



    guardarTree();

    return 0;
}

int alojar(int size, int nodo){
    int alojado;
    //tiene hijos
    if(tree[nodo][MID]==0){
        alojado = alojar(size, nodo*2);
        if(alojado == 0){ //if no alojado
            alojado = alojar(size, nodo*2 + 1);
        }
        return alojado;
    }
    //esta ocupado
    if(tree[nodo][MID]>0){
        return 0;
    }


    //esto se ejecuta en caso de que este libre con -1
    if(size <= (tree[nodo][TAM_PART])/2 ){
        alojado = alojar(size, nodo*2);
        if(alojado == 0){ //if no alojado
            alojado = alojar(size, nodo*2 + 1);
        }
        if(alojado){
            tree[nodo][MID]=0;
        }
        return alojado;
    }
    else if(size <= tree[nodo][TAM_PART]){
        tree[nodo][MID]=idCounter;
        idCounter+=1;
        tree[nodo][TAM_PROC]=size;

        if(tree[nodo][MID] != 1){
            printf("M E M O R Y A L L O C A T I O N – Process size: %i\n", size);
            printf("Resultado: Asignanción existosa Buddy System MID: %i\n", tree[nodo][MID]);
        }
    }
    else{
        //es mayor que el espacio
        return 0;
    }
}

void inicializarTree(int nodo, int size, int base){
    if(size==1){
        tree[nodo][MID]=-1;
        tree[nodo][BASE]=base;
        tree[nodo][TAM_PART]=size;
    }
    else{
        tree[nodo][MID]=-1;
        tree[nodo][BASE]=base;
        tree[nodo][TAM_PART]=size;
        //iniciar la rama izq
        inicializarTree(nodo*2, size/2, base);
        //iniciar la rama der
        inicializarTree(nodo*2 + 1, size/2, size/2 + base);
    }
    return;
}

void cargarTree(){
    FILE * f;
    f = fopen(ARCHIVO, "r+");
        if(f == NULL){
        printf("Error de Carga\n");
    }
    int i, j;
    fread(&idCounter, sizeof(int), 1, f);
    for(i = 1; i<=2047; i+=1){
        for(j = 0; j<4; j+=1){
            fread(&tree[i][j], sizeof(int), 1, f);
        }
    }
    fclose(f);
    return;
}

void guardarTree(){
    FILE * f;
    f = fopen(ARCHIVO, "r+");
    if(f == NULL){
        printf("Error de Guardado\n");
    }
    int i, j;
    fwrite(&idCounter, sizeof(int), 1, f);
    for(i = 1; i<=2047; i+=1){
        for(j = 0; j<4; j+=1){
            fwrite(&tree[i][j], sizeof(int), 1, f);
        }
    }
    fflush(f);
    fclose(f);
    return;
}
void mostrarTree(){
    int i, j;

    printf("idCounter: %i\n", idCounter);
    printf("NODO   |    MID    |    TAM_PART   |   BASE   |   TAM_PROC  \n");
    for(i = 1; i<=2048; i+=1){
        printf("%i     ", i);
        for(j = 0; j<4; j+=1){
            printf("%i     ", tree[i][j]);
        }
        printf("\n");
        sleep(1);
    }

    return;
}

int desalojar(int mid, int nodo){
    int alojado;

    if(tree[nodo][MID]==-1){
        return 1;
    }

    //tiene hijos
    if(tree[nodo][MID]==0){
        alojado = desalojar(mid, nodo*2);
        if(alojado == 1){ //if alojado
            alojado = desalojar(mid, nodo*2 + 1);
        }

        //ambos hijos quedan desalojados completamente
        if(tree[nodo*2][MID] == -1 && tree[nodo*2 + 1][MID]==-1){
            tree[nodo][MID]=-1;
        }

        return alojado;
    }
    //esta ocupado por mid distinto
    if(tree[nodo][MID]>0 && tree[nodo][MID]!=mid){
        return 1;
    }

    if(tree[nodo][MID]==mid){
        tree[nodo][MID]=-1;
        tree[nodo][TAM_PROC]=0;
        return 0;
    }
}

void mostrarTabla(int a){
    int size = crearTabla();
    ordenarTabla(size);
    system("clear");
    printf("DIRECCION BASE    |    TAMANHO    |     MID     |\n");
    int y;
    for(y=2; y<=size+1; y+=1){
            gotoxy(0, y);
            printf("%i", tabla[y-2][0]);
            gotoxy(20, y);
            printf("%i", tabla[y-2][1]);
            gotoxy(36, y);
            
            if(a==ORDEN_EJ3){
                if(tabla[y-2][2]==-1){
                    printf("LIBRE");
                }
                else{
                    printf("%i", tabla[y-2][2]);
                }
            }
            if(a==ORDEN_EJ4){
                if(tabla[y-2][2]==-1){
                    printf("LIBRE");
                }
                else{
                    printf("%i (FI: %i)", tabla[y-2][2], tree[tabla[y-2][3]][TAM_PART]-tree[tabla[y-2][3]][TAM_PROC]);
                }
            }

    }
    printf("\n"); 
}

int crearTabla(){
    int i, j;
    j=0;
    for(i = 1; i<=2047; i+=1){
        if(tree[i][MID]>0){
            tabla[j][0]=tree[i][BASE];
            tabla[j][1]=tree[i][TAM_PART];
            tabla[j][2]=tree[i][MID];
            tabla[j][3]=i;
            j+=1;
        }
        if(tree[i][MID]==0 && tree[i*2][MID]==-1){
            tabla[j][0]=tree[i*2][BASE];
            tabla[j][1]=tree[i*2][TAM_PART];
            tabla[j][2]=tree[i*2][MID];
            tabla[j][3]=i*2;
            j+=1;
        }
        if(tree[i][MID]==0 && tree[i*2+1][MID]==-1){
            tabla[j][0]=tree[i*2+1][BASE];
            tabla[j][1]=tree[i*2+1][TAM_PART];
            tabla[j][2]=tree[i*2+1][MID];
            tabla[j][3]=i*2+1;
            j+=1;
        }
    }
    return j;
}

void gotoxy(int x,int y)
{
    printf("%c[%d;%df",0x1B,y,x);
}

void ordenarTabla(int size){
    //simple ordenamiento
    int j, i, k; 
    int aux[5];
 
    for (i = 0; i < size; i+=1)
    {
        for (j = 0; j < size-1; j+=1){
            if (tabla[j][0] > tabla[j+1][0]){
                
                for(k=0; k<4; k+=1) aux[k] = tabla[j][k];
                for(k=0; k<4; k+=1) tabla[j][k] = tabla[j+1][k];
                for(k=0; k<4; k+=1) tabla[j+1][k] = aux[k];
            }
            
        }

    }
}
