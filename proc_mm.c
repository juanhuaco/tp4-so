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


int idCounter = 0;
int tree[2049][5];

int alojar(int size, int nodo);
int desalojar(int size, int nodo);
void inicializarTree(int nodo, int size, int base);
void cargarTree(FILE * f);
void guardarTree(FILE * f);
void mostrarTree();

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
    FILE *f;
    f = fopen("memory.bin", "r+");
    if(errno == EXISTE){
        cargarTree(f);
    }
    else{
        //se crea el archivo
        f = fopen("memory.bin", "w+");
        //se inicia el grafo
        idCounter=1;
        inicializarTree(1, 1024, 0);
        //cargar el primer proceso;
        alojado = alojar(60, 1);
    }

    
    
    //CASOS
    if(strcmp(argv[1], "-n") == 0){
        int n = atoi(argv[2]);
        alojado = alojar(n, 1);
        if(alojado == 0){
            printf("M E M O R Y A L L O C A T I O N – Process size: %i \n", n);
            printf("Resultado: Error catastrofico en la asignacion... No hay espacio \n");
        }
    }
    else if(strcmp(argv[1], "-f") == 0){
        int n = atoi(argv[2]);
        alojado = desalojar(n, 1);
        if(alojado==0){
            printf("M E M O R Y D E A L L O C A T I O N* Procces MID: %i\n", n);
        }
        else{
            printf("NO SE PUDO DESALOJAR PROCESO CON EL MID DADO.\n");
        }
    }

    mostrarTree();





    guardarTree(f);
    fclose(f);
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

void cargarTree(FILE * f){
    int i, j;
    fread(&idCounter, sizeof(int), 1, f);
    for(i = 1; i<=2048; i+=1){
        for(j = 0; j<4; j+=j){
            fread(&tree[i][j], sizeof(int), 1, f);
        }
    }
}

void guardarTree(FILE * f){
    int i, j;
    fwrite(&idCounter, sizeof(int), 1, f);
    for(i = 1; i<=2048; i+=1){
        for(j = 0; j<4; j+=j){
            fwrite(&tree[i][j], sizeof(int), 1, f);
        }
    }
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