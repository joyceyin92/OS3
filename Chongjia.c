#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>
#include <ctype.h>


typedef struct Buff_List{
    struct Buff_List *next;
    char word[20];
    int count;
}Buff_List;

typedef struct S_Buff{
    char word[20];
}S_Buff;

typedef struct threadPara{
    Buff_List *head;
    int i;
    FILE *fp;
    int start;
    int end;
    int buff_count;
    int n;
    int b;
}threadPara;

char S_Buffer[10][100][20];
pthread_mutex_t mutex[10];

void *mapreader(void *para);
void *mapadder(void *para);


int StringCompare(const void *s1, const void *s2){
     Buff_List  *str1 = ( Buff_List  *)s1;
     Buff_List  *str2 = ( Buff_List  *)s2;
    return (str2->count - str1->count);
  }


int main(int argc, char *argv[]){
    struct timeval start_time;
    struct timeval finish_time;
    gettimeofday(&start_time,NULL);
  
    int n = atoi(argv[2]);
    int b = atoi(argv[3]);

    int p=0;
    

    pthread_t *thread_a=(pthread_t*)malloc(n*sizeof(pthread_t));
    pthread_t *thread_b=(pthread_t*)malloc(n*sizeof(pthread_t));
    
    threadPara *para=malloc(n*sizeof(threadPara));
 

    FILE *fp = fopen(argv[1], "r");	
 
    if (fp == NULL)
    {
        printf("File Open Fail!\n");
    }
    fseek(fp,0L,SEEK_END);
    int FileSize = ftell(fp);

    for (p=0;p<n;p++){
        FILE *fp = fopen(argv[1], "r");	
   	
     	para[p].fp = fp;
        para[p].i=p;
        para[p].start = p*FileSize/n;
        para[p].end = para[p].start+FileSize/n;  
        para[p].head = (Buff_List*)malloc(sizeof(Buff_List));
        para[p].n = n; 
        para[p].b = b; //printf("para[%d]->b=%d\n",p,para[p].b);
        pthread_mutex_init(&mutex[p],NULL);
      
        pthread_create(&thread_a[p],NULL,(void*)mapreader,&para[p]);
        pthread_create(&thread_b[p],NULL,(void*)mapadder,&para[p]);
    }
       for (p=0;p<n;p++){
        pthread_join(thread_a[p],NULL);
        pthread_join(thread_b[p],NULL);
       
        pthread_mutex_destroy(&mutex[p]);
    }
    

    Buff_List *List = (Buff_List*)calloc((1000),sizeof(Buff_List));
    int i;
    int Var = 0;
    for (i=0; i<n; i++) {
        Buff_List *loop = para[i].head;
        while (loop!=NULL && loop->next != NULL) {
            int distinct = 1;
            int q;
            for(q=0;q<Var;q++){
                if (strcmp(List[q].word, loop->word)==0) {
                    distinct = 0;
		            List[q].count+=loop->count;
                }
            }
            if (distinct==1) {
                strcpy(List[Var].word, loop->word);
                List[Var].count = loop->count;
                Var++;
            }
            loop=loop->next;
        }
    }
   
    qsort(List,Var,sizeof(Buff_List),StringCompare);
    int word_count = 0;
    FILE *file = fopen("output.txt","w");
    for (i=0; i<Var; i++) {
        word_count+= List[i].count;
        fprintf(file,"<%s> appear %d times.\n", List[i].word,List[i].count);
    }
    fprintf(file,"%d total words.\n",word_count);
    fprintf(file,"%d unique words.\n", Var);
    fclose(file);
    fclose(fp);
    gettimeofday(&finish_time,NULL);
    double  duration = 0;
    duration =((finish_time.tv_sec*1000000 + finish_time.tv_usec) - (start_time.tv_sec*1000000 + start_time.tv_usec))/1000000.0;
    printf("Running Time: %f\n",duration);
    return 0;
    }
    
    



void *mapreader(void *para){
    threadPara *pm;
    pm =(struct threadPara*) para;
    
    char tmp[20];
    
    int i = pm->i;
    int n = pm->n;
    int b = pm->b;
    
    fseek(pm->fp,pm->start,SEEK_SET);

    while ((ftell(pm->fp) < pm->end) && (ftell(pm->fp)!= EOF)) {
        //printf("%d\n",ftell(pm->fp));
        fscanf(pm->fp,"%s",tmp);

        //special char
        int l=0;
        int s=0;
        char str[20];
        while (tmp[l]) {
            if(!ispunct(tmp[l])){
		if(tmp[l] >= 'A' && tmp[l] <= 'Z'){
		    tmp[l]+=32;
		}
                str[s] = tmp[l];
                s++;
            }
            l++;
        }
        str[s]='\0';
      
        while(pm->buff_count >= b); //buffer is full
        pthread_mutex_lock(&mutex[pm->i]);
        strcpy(S_Buffer[pm->i][pm->buff_count],str);
        pm->buff_count++;
        pthread_mutex_unlock(&mutex[pm->i]);
    }
    
    printf("Reader %d success!\n",pm->i);
   
}

    

void *mapadder(void *para)
{
    threadPara *pm= para;
    int n = pm->n;
    int b = pm->b;
 
    while ((ftell(pm->fp) < pm->end) && (ftell(pm->fp)!= EOF) ||( pm->buff_count)) {
        while (pm->buff_count == 0); //buffer is empty
        pthread_mutex_lock(&mutex[pm->i]);
        int distinct = 1;
        Buff_List *temp = pm->head;
        if (temp == NULL) {
            strcpy(temp->word,S_Buffer[pm->i][pm->buff_count - 1]);
            pm->buff_count --;
            temp->count = 1;
            temp->next = NULL;
            distinct = 1;
        }
        else{
            while (temp!=NULL) {
                if (strcmp(temp->word,S_Buffer[pm->i][pm->buff_count-1])==0) {
                    pm->buff_count--;
                    temp->count++;
                    distinct = 0;
                    break;
                }
                temp = temp->next;
            }
            if (distinct==1) {
                Buff_List *new=(Buff_List*)malloc(sizeof(Buff_List));
                strcpy(new->word,S_Buffer[pm->i][pm->buff_count-1]);
                pm->buff_count--;
                new->count=1;
                new->next = pm->head;
                pm->head = new;
            }
        }
        pthread_mutex_unlock(&mutex[pm->i]);
    }
    printf("Adder %d success!\n",pm->i);
}


