/**
* IZP Proj3
* Autor: Iveta Strnadova, xstrna14
* Datum 9.12.2018
*
* Jednoducha shlukova analyza: 2D nejblizsi soused.
* Single linkage
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

#define LINE_LENGHT 100

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
/**
* Initialization of a cluster with allocating size for oblejcts based on parametr cap.
* @param c initialized cluster
* @param cap capacity of initiliazed cluster
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    if(cap==0) c->obj=NULL;
    else c->obj = malloc ( cap * sizeof(struct obj_t));
    c->size=0;
    c->capacity=cap;
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
 /**
 * Function frees objects of cluster and sets the rest of variables in *c to 0.
 * @param c cluster to be cleared
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);
    c->capacity = 0;
    c->size = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
 /**
 * Function resizes cluster to new capacity.
 * @param c cluster to be resized
 * @param new_cap new capicity of *c
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
 /**
 * Function appends obj at the end of cluster. Resizes cluster if needed.
 * @param c cluster to which the object is appended
 * @param obj object to be appended
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    assert(c!=NULL);

    if(c->capacity==c->size) c=resize_cluster(c, c->capacity+1);
    assert(c!=NULL);
    c->obj[c->size]=obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
 /**
 * Function merges two clusters together by appending all objects from second cluster to first one.
 * @param c1 Cluster to which objects of second cluster are appended.
 * @param c2 Cluster which objects are appended to first cluster.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    for(int i=0;i<c2->size;i++) {
        append_cluster(c1,c2->obj[i]);
    }
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
/**
* Function clears cluster to be removed and moves the rest of clusters in cluster array
* @param carr Array of clusters
* @param narr Number of clusters in carr
* @param idx Index at which in carr is cluster to be removed
* @return narr-1 size of cluster array minus one for removed cluster
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);

    clear_cluster(&carr[idx]);
    for(int i=idx; i < (narr-1) ;i++) {
        carr[i] = carr[i + 1];
    }
    return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
 /**
 * Function calculates distance between two objects.
 * @param o1 first object
 * @param o2 second object
 * @return distance distance between two objects
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    float distance;
    distance=sqrt(((o1->x-o2->x)*(o1->x-o2->x))+((o1->y-o2->y)*(o1->y-o2->y)));
    return distance;
}

/*
 Pocita vzdalenost dvou shluku.
*/
/**
* Function calculates distances between each objects from first and second clusters and returns the shortest as "cluster distance".
* @param c1 first cluster
* @param c2 second cluster
* @return distance distance between two clusters
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float distance, shortest;
    for(int i=0;i<c1->size;i++)
        for(int j=0;j<c2->size;j++){
            distance=obj_distance(&c1->obj[i], &c2->obj[j]);
            if((i==0&&j==0)||(distance<shortest)) shortest=distance;
        }
    return shortest;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
/**
* Function finds the two closest clusters.
* @param carr array of clusters
* @param narr number of clusters in carr
* @param c1 index in carr of first neighbour
* @param c2 index in carr of second neighbour
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    float distance, shortest;
    for(int i=0;i<narr;i++)
        for(int j=i+1;j<narr;j++) {
            distance=cluster_distance(&carr[i], &carr[j]);
            if((i==0&&j==1)||(distance<shortest)){
                    shortest=distance;
                    *c1=i;
                    *c2=j;
            }
        }
}

// pomocna funkce pro razeni shluku
/**
* Help function for sorting objects in cluster.
*/
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
/**
* Fuction sorts objects in cluster based on their ID.
* @param c cluster to be sorted
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
/**
* Function prints cluster
* @param c cluster to be printed
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
/**
* Function opens file, loads clusters from it one by one and adds them to array of clusters.
* @param filename name of file from which the clusters are loaded
* @param arr Array of clusters to which they are loaded
* @return numberOfClusters number of loaded clusters
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    ///Loading file
    FILE* f;
    f=fopen(filename, "r");
    if(f==NULL){
        fprintf(stderr, "Error while loading file.\n");
        *arr=NULL;
        return -1;
    }

    ///Loading first line of file
    char line[LINE_LENGHT];
    if(fgets(line,LINE_LENGHT,f)==NULL) {
        *arr=NULL;
        fclose(f);
        return -1;
    }

    ///Finding number of clusters from first line
    char *pointerOnExpectedEnd;
    int numberOfClusters=0;
    numberOfClusters=strtol(&line[6], &pointerOnExpectedEnd, 10);
    if(((pointerOnExpectedEnd[0]!='\n')&&(pointerOnExpectedEnd[0])!='\0')||(numberOfClusters<0)) {
        fprintf(stderr, "Failed to convert number of clusters.\n");
        *arr=NULL;
        fclose(f);
        return 1;
    }

    ///Allocating enough space for found number of clusters
    *arr=(struct cluster_t*) malloc(numberOfClusters*sizeof(struct cluster_t));
    if(*arr==NULL) {
            fclose(f);
            return 1;
    }

    ///Loading objects to clusters to arr[0], one by one
    int id, idCheck[numberOfClusters], idColision=0;
    float x, y;
    for(int i=0; i<numberOfClusters; i++) {
        ///Checking if there is still a line with object to read (if not, numberOfClusters is edited, cycle ends
        if(fgets(line,LINE_LENGHT,f)==NULL) {
            numberOfClusters=i;
            break;
        }
        ///Loading id, x and y from line
        id=strtol(line, &pointerOnExpectedEnd, 10);
        if(pointerOnExpectedEnd[0]==' ') {
            x=strtod(&pointerOnExpectedEnd[1], &pointerOnExpectedEnd);
            if(pointerOnExpectedEnd[0]==' ') y=strtod(&pointerOnExpectedEnd[1], &pointerOnExpectedEnd);
        }
        ///Should the numbers be in wrong format, the whole line is skipped and not counted in numberOfClusters
        if(pointerOnExpectedEnd[0]!='\n'&&pointerOnExpectedEnd[0]!='\0') {
            numberOfClusters--;
            i--;
            fprintf(stderr, "Error in line %s. It was skipped.\n", line);
            continue;
        }
        ///Should the x and y be outside <0,1000> or should the id repeat, the whole line is skipped and not counted in numberOfClusters
        ///Should there already exist object with same id, the line is skipped too
        for(int j=0;j<i; j++) if(idCheck[j]==id) idColision++;
        if((x<0||x>1000)||(y<0||y>1000)||idColision!=0)
        {
           idColision=0;
           fprintf(stderr, "Error in number values in %s. It was skipped.\n", line);
           numberOfClusters--;
           i--;
           continue;
        }

        ///Cluster allocated, initiliazed and checked if all went well
        struct cluster_t *cluster = malloc(sizeof(struct cluster_t));
        init_cluster(cluster, 1);
        if(cluster==NULL){
            fprintf(stderr, "Failed to alloc memory for cluster.\n");
            *arr=NULL;
            fclose(f);
            return 1;
        }

        ///Allocating object, setting its values, appending it to cluster
        struct obj_t *object = malloc(sizeof(struct obj_t));
        object->x = x;
        object->y = y;
        object->id = id;
        idCheck[i]=id;

        append_cluster(cluster, *object);

        ///Passing cluster to cluster array, freeing temporary used space
        arr[0][i] = *cluster;

        free(cluster);
        free(object);
    }

    fclose(f);
    return numberOfClusters;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
/**
* Function prints out all clusters in given array of clusters.
* @param carr array of clusters
* @param narr number of clusters in carr
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    ///Initial argument check
    if(argc!=3&&argc!=2) {
        fprintf(stderr, "Wrong number of arguments.\n");
        return 1;
    }

    char* getNumberCheck;
    int desiredClusterCount;
    if(argc==2) desiredClusterCount=1;
    else {
        desiredClusterCount=strtol(argv[2], &getNumberCheck, 10);
        if((getNumberCheck[0]!='\n'&&getNumberCheck[0]!='\0')||desiredClusterCount<1){
                desiredClusterCount=1;
                fprintf(stderr, "Failed to extract number from argument \"%s\" or value not bigger than 1 - value set to default 1.\n", argv[2]);
        }
    }

    ///Loading clusters
    struct cluster_t *clusters;
    int clusterCount;
    clusterCount=load_clusters(argv[1], &clusters);
    assert(clusters!=NULL);

    ///As long as the clusterCount is bigger than desiredClusterCount, loop will go on, each time finding two closest clusters and merging them
    int neighbour1, neighbour2;
    while(clusterCount > desiredClusterCount) {
        find_neighbours(clusters, clusterCount, &neighbour1, &neighbour2);
        merge_clusters(&clusters[neighbour1], &clusters[neighbour2]);
        clusterCount=remove_cluster(clusters, clusterCount, neighbour2);
    }

    print_clusters(clusters, clusterCount);

    ///Freeing the previously allocated space
    for(int i = 0; i < clusterCount; i++){
        free(clusters[i].obj);
    }
    free(clusters);

    return 0;
}
