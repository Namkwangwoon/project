#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdio.h>
#include<time.h>

typedef struct file_set {
  char fname[4]; //파일 이름
  short finode;  //파일의 아이노드 번호
}file_set;
//파일의 이름과 번호 세트

typedef struct di {
  short current;        //현재 디렉토리 inode 번호
  short parent;         //부모 디렉토리 inode 번호
  int fcount;           //파일 개수
  file_set fset[20];    //파일의 이름과 아이노드 번호
}di;
//data blcok의 directory 파일일 경우

typedef struct re {
	unsigned char save[128]; //128byte
}re;
 //data blcok의 regular 파일일 경우

typedef union data_un{
  di dir; //디렉토이 파일일경우
  re reg; //일반 파일일 경우
}data_un;
//데이타블록은 디렉토리로도 사용가능하고 일반 파일로도 사용하므로 유니온을 선택

typedef struct inode_st{ //inode 구조체
	char type;   //파일의 타입
	short dir_b; //직접 다이렉트 블록 번호
	struct tm c_time;
  /***********************
    사용법 : time_t now;
    now=time(NULL);
    c_time=*(localtime(&now));
   ************************/
	int size;   //파일 크기
	short in_b; //싱글 인다이렉트 블록 번호
	short indb_b; //더블 인다이렉트 블록 번호
}inode_st;
//아이노드

typedef struct file_st{
	char boot[2];  //boot 블록 2byte
	unsigned char inode_ch[64]; //super block inode의 super block
	unsigned char data_ch[128]; //super block data의 super block
	inode_st inode[512]; //inode 512개
	data_un data[1024];  //data block 1024개
}file_st;
//파일 시스템 구조체

typedef struct fls_st{
  char fname[5]; //파일 이름
  short finode;  //파일 번호
}fls_st;
//struct file_set와 달리 fname이 5칸이다 null값을 넣기 위함

typedef struct tree_node{
  char fname[5]; //파일 이름
  int finode;    //파일 아이노드 번호
  struct tree_node *parents;  //부모 디렉토리의 번호
  struct tree_node *siblings; //형제 디렉토리의 번호
  struct tree_node *children; //자식 디렉토리의 번호
}tree_node;
//트리 구조체

tree_node *create_node(char *fname,int finode);
int order_ch(char *order,FILE *fop,file_st *myfs,tree_node *root,tree_node *tnow,tree_node **n2ow);
void mycpfrom(char *order,file_st *myfs);
void bit_mark(unsigned char *check,int num);
void bit_unmark(unsigned char *check,int num);
int empty_ch(unsigned char *ch,int len);
int cal_sizec(int file_size);
int empty_count(unsigned char *check,int len);
int *make_list_emdb(unsigned char *check,int size_c);
void write_in_file(file_st *myfs,char *my_file_name,int i_empty,int finode);
int btoi(unsigned char *save,int num);
void itob(unsigned char *save,short num);
int indir_n(unsigned char *save);
void myls(char *order,file_st *myfs);
void ct_string(struct tm ct,char *ct_s);
int mycmp(const void *v1,const void *v2);
void mycat(char *order,file_st *myfs);
int find_file(di save,char *name);
int *find_dnum(int num,file_st *myfs);
void mystate(char *order,file_st *myfs);
int count_dnum(int num,file_st *myfs);
void myshowfile(char *order,file_st *myfs);
void mypwd(char *order,file_st *myfs);
void mycd(char *order,file_st *myfs,tree_node *root,tree_node **n2ow);
int find_rinode(char *route,file_st *myfs);
void mymkdir(char *order,file_st *myfs);
char *change_route(char *route);
void myshowinode(char *order,file_st *myfs);
void myshowblock(char *order,file_st *myfs);
void mytouch(char *order,file_st *myfs);
void rm_bit(unsigned char *save,short count,file_st *myfs);
void rm_datab(int num,file_st *myfs);
void mycp(char *f1route,char *f2route,file_st *myfs);
void mycpto(char *order,file_st *myfs);
void myrmdir(char *order,file_st *myfs);
void myrm(char *order,file_st *myfs);
void change_finfo(file_st *myfs,char *my_file_name,char *change_name,int wstation);
void remove_in_file(file_st *myfs,char *my_file_name,int wstation);
void reset_db(int num,file_st *myfs);
void reset_in(int num,file_st *myfs);
void mymv(char *order,file_st *myfs);
void recall_tree(file_st *myfs,tree_node *wstation);
tree_node *change_now(char (*name)[5],int num,tree_node *root,tree_node *now);
void recall_print(int deep,tree_node *wstation);

char now_dir[10000]="/";  //mycd와 mypwd와 쉘 시작 부분에서 사용 인자를 줄이기 위해 전역 선언
int now=0;                //mycd와 mypwd와 쉘 시작 부분에서 사용 인자를 줄이기 위해 전역 선언

int main(){
  FILE *fop=fopen("myfs","rb");
  char order[1000];
  tree_node *root,*tnow;
  root=create_node("/",0);
  root->parents=root; //트리의 뿌리를 할당
  tnow=root; //트리의 현재 경로 tree now

  if(fop==NULL){
		printf("error : no myfs\n");
		return 1;
	}

  file_st myfs; //파일이 있으므로 구조체 선언

  //파일 시스템 입력
  fread(&myfs,sizeof(myfs),1,fop);

  //명령 받는 부분
  while(1)
  {
    printf("[%s ]$ ",now_dir); //경로 표시
    fgets(order,sizeof(order),stdin); //공백 문자 포함 입력
    recall_tree(&myfs,root); //트리 구조를 만듬
    if(order_ch(order,fop,&myfs,root,tnow,&tnow)==-1) //명령에 해당되는 함수 실행 -1일 경우는 byebye
      break;
  }

  //ram상에 있는 파일 시스템을 출력
  fclose(fop);
  fop=fopen("myfs","wb");
  fwrite(&myfs,sizeof(myfs),1,fop);
  fclose(fop);
  return 0;
}
tree_node *change_now(char (*name)[5],int num,tree_node *root,tree_node *now){
  //무조건 directory 파일 형식이고 존재하는 파일만 넘겨준다
  tree_node *temp;

  if(strncmp(name[0],"/",1)==0){ //절대 경로
    if(num==1){
      return root;
    }
    temp=root->children;
    for(int i=1;i<num;i++){
      if(i!=1)
        temp=temp->children;
      while(1){
        if(strncmp("..",name[i],2)==0){
          temp=temp->parents->parents;
          break;
        }
        else if(strncmp(".",name[i],1)==0){
          temp=temp->parents;
          break;
        }
        else if(strncmp(temp->fname,name[i],4)==0){
          break;
        }
        else
          temp=temp->siblings;
      }
    }
    return temp;
  }
  else { //상대 경로
    temp=now->children;
    for(int i=0;i<num;i++){
      if(i!=0)
        temp=temp->children;
      while(1){
        if(strncmp("..",name[i],2)==0){
          temp=temp->parents->parents;
          break;
        }
        else if(strncmp(".",name[i],1)==0){
          temp=temp->parents;
          break;
        }
        else if(strncmp(temp->fname,name[i],4)==0)
          break;
        else
          temp=temp->siblings;
      }
    }
    return temp;
  }
}
//트리의 현재 위치를 바꾼다

void recall_print(int deep,tree_node *wstation){
  while(1){
    for(int i=1;i<deep*3;i++)
      printf("-");
    printf("* %s\n",wstation->fname);
    if(wstation->children!=NULL) //자식이 없을 경우 형제로 넘어간다
      recall_print(deep+1,wstation->children);
    if(wstation->siblings==NULL) //형제도 없을 경우 종료
      return;
    wstation=wstation->siblings;
  }
}
//재귀 함수로 트리 구조를 출력한다

void print_tree(char *order,tree_node *root,tree_node *now,file_st *myfs){
  char route[1000]="",backup[1000],(*name_list)[5],*temp;
  int i=1,deep=0;
  tree_node *wstation=NULL;
  sscanf(order,"%*s %s",route);
  strcpy(backup,route);
  if(route[0]==0){  //인자가 없는 경우 현재 디렉토리 출력
    printf("%s\n",now->fname);
    if(myfs->data[myfs->inode[now->finode].dir_b].dir.fcount==0)
      return ;
    deep=1;
    wstation=now->children;
    recall_print(deep,wstation);
  }
  else if(route[0]=='/'){  //절대경로
    if(find_rinode(route,myfs)==-1){
      printf("error : no file\n");  //파일이 없는 경우 error
      return ;
    }
    if(myfs->inode[find_rinode(route,myfs)].type=='-'){
      printf("error : not directory file\n"); //파일이 regular 파일일 경우 error
      return ;
    }
    int count=1;
     while(1)
       {
          if(route[2]==0)
          {
             count--;
             break;
          }
          if(route[i]=='/' && route[i+1]!=0)
             count++;
          else if(route[i]=='/'&&route[i+1]==0)
             break;
          if(route[i] == 0)
             break;
          i++;
       }
       name_list=(char (*)[5])calloc((count+1)*5,sizeof(char));
       name_list[0][0]='/';
       name_list[0][1]=0;
       name_list[0][2]=0;
       name_list[0][3]=0;
       if(count>=1){
         temp=strtok(backup,"/");
         name_list[1][0]=temp[0];
         name_list[1][1]=temp[1];
         name_list[1][2]=temp[2];
         name_list[1][3]=temp[3];
        for(int i=2;i<=count;i++){
          temp=strtok(NULL,"/");
          name_list[i][0]=temp[0];
          name_list[i][1]=temp[1];
          name_list[i][2]=temp[2];
          name_list[i][3]=temp[3];
        }
      }
      wstation=change_now(name_list,count+1,root,wstation);
      printf("%s\n",wstation->fname);
      if(wstation->children==NULL)
        return ;
      deep=1;
      wstation=wstation->children;
      recall_print(deep,wstation);

  }
  else{ //상대 경로
    if(find_rinode(route,myfs)==-1){
      printf("error : no file\n"); //파일이 없는 경우 error
      return ;
    }
    if(myfs->inode[find_rinode(route,myfs)].type=='-'){
      printf("error : not directory file\n"); //파일이 regular 파일일 경우 error
      return ;
    }
    int count=0;
    if(route[strlen(route)-1]=='/')
     route[strlen(route)-1]=0;
    for(int i=0;i<=(int)strlen(route);i++){
      if(route[i]=='/'||route[i]==0){
        count++;
      }
    }
    name_list=(char (*)[5])calloc(count*5,sizeof(char));
      temp=strtok(backup,"/");
      name_list[0][0]=temp[0];
      name_list[0][1]=temp[1];
      name_list[0][2]=temp[2];
      name_list[0][3]=temp[3];
     for(int i=1;i<count;i++){
       temp=strtok(NULL,"/");
       name_list[i][0]=temp[0];
       name_list[i][1]=temp[1];
       name_list[i][2]=temp[2];
       name_list[i][3]=temp[3];
     }
     wstation=change_now(name_list,count,root,now);
     printf("%s\n",wstation->fname);
     if(wstation->children==NULL)
       return ;
     deep=1;
     wstation=wstation->children;
     recall_print(deep,wstation);
  }
}
//mytree의 해당 되는 함수

void recall_tree(file_st *myfs,tree_node *wstation){
  int fcount,*dnum;
  tree_node *temp;
  if(myfs->inode[wstation->finode].type=='-')
    return;
  fcount=myfs->data[myfs->inode[wstation->finode].dir_b].dir.fcount;
  if(fcount==0)
    return;
  dnum=find_dnum(wstation->finode,myfs);
  wstation->children=create_node(myfs->data[dnum[0]].dir.fset[0].fname,myfs->data[dnum[0]].dir.fset[0].finode);
  wstation->children->parents=wstation;
  temp=wstation;
  wstation=wstation->children;
  recall_tree(myfs,wstation);
  for(int i=1;i<fcount;i++){
    wstation->siblings=create_node(myfs->data[dnum[i/20]].dir.fset[i%20].fname,myfs->data[dnum[i/20]].dir.fset[i%20].finode);
    wstation->siblings->parents=temp;
    wstation=wstation->siblings;
    recall_tree(myfs,wstation);
  }
  free(dnum);
}
//myfs를 보고 이진 트리를 구성

tree_node * create_node(char *fname,int finode){
  tree_node *temp;
  temp=(tree_node *)calloc(1,sizeof(tree_node));
  strncpy(temp->fname,fname,4);
  temp->finode=finode;
  temp->parents=NULL;
  temp->siblings=NULL;
  temp->children=NULL;
  return temp;
}
//트리를 할당 받는다

int order_ch(char *order,FILE *fop,file_st *myfs,tree_node *root,tree_node *tnow,tree_node **n2ow){
	if(strncmp(order,"byebye",6)==0) //byebye 입력시 종료 파일의 저장을위해 main 함수로 리턴
		return -1;
	else if(strncmp(order,"my",2)!=0) //my가 명령어에 안들어가면 cygwin 명령 실행
		system(order);
	else if(strncmp(order,"mycpfrom",8)==0) //mycpfrom 실행
		mycpfrom(order,myfs);
	else if(strncmp(order,"myls",4)==0) //myls 실행
		myls(order,myfs);
  else if(strncmp(order,"mycat",5)==0) //mycat 실행
  	mycat(order,myfs);
  else if(strncmp(order,"mystate",7)==0) //mystate 실행
    mystate(order,myfs);
  else if(strncmp(order,"myshowfile",10)==0) //myshowfile 실행
    myshowfile(order,myfs);
  else if(strncmp(order,"mypwd",5)==0) //mypwd 실행
    mypwd(order,myfs);
  else if(strncmp(order,"mycd",4)==0) //mycd 실행
    mycd(order,myfs,root,n2ow);
  else if(strncmp(order,"mymkdir",7)==0) //mymkdir 실행
    mymkdir(order,myfs);
  else if(strncmp(order,"myshowinode",11)==0) //myshowinode 실행
    myshowinode(order,myfs);
  else if(strncmp(order,"myshowblock",11)==0) //myshowblock 실행
    myshowblock(order,myfs);
  else if(strncmp(order,"mytouch",7)==0) //mytouch 실행
    mytouch(order,myfs);
  else if(strncmp(order,"mycpto",6)==0) //mycpto 실행
    mycpto(order,myfs);
  else if(strncmp(order,"mycp",4)==0){ //mycp 실행
    char f1route[1000]="",f2route[1000]="";
    sscanf(order,"%*s %s %s",f1route,f2route);
    mycp(f1route,f2route,myfs);
  }
  else if(strncmp(order,"myrmdir",7)==0) //myrmdir 실행
    myrmdir(order,myfs);
  else if(strncmp(order,"myrm",4)==0) //myrm 실행
    myrm(order,myfs);
  else if(strncmp(order,"mymv",4)==0) //mymv 실행
    mymv(order,myfs);
    else if(strncmp(order,"mytree",6)==0) //mytree 실행
      print_tree(order,root,tnow,myfs);
  else
    printf("error : no command\n");   //그 외의 명령어는 error 처리
	return 0;
}
//명령에 따라 실행

void mypwd(char *order,file_st *myfs){
  printf("%s\n",now_dir);
}
//now_dir를 출력 now_dir은 경로를 의미 mycd에서 자동 수정이므로 출력만 하면됨

void mycd(char *order,file_st *myfs,tree_node *root,tree_node **n2ow){
  char route[1000]="",(*name_list)[5],*temp,stack[1000][5]={""};
  int fin,i=1;
  tree_node *ws;
  sscanf(order,"%*s %s",route);
  if(route[strlen(route)-1]=='/')
    route[strlen(route)-1]=0;
  if(route[0]==0){
    now=0;
    for(int i=1;i<10000;i++)
      now_dir[i]=0;
    now_dir[0]='/';
    *n2ow=root;
  }
  else{
    fin=find_rinode(route,myfs);
    if(fin==-1){
      printf("error : no directory '%s'\n",route); //파일이 없는 경우 error
      return ;
    }
    if(myfs->inode[fin].type=='-'){
      printf("error : not directory file\n"); //파일이 regular 파일일 경우 error
      return ;
    }
    if (route[0]=='/') { //절대 경로
      for(int j=1;j<10000;j++)
        now_dir[j]=0;
        int count=1;
         while(1)
           {
              if(route[2]==0)
              {
                 count--;
                 break;
              }
