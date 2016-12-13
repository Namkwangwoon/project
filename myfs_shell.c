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
             temp=strtok(route,"/");
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
          ws=change_now(name_list,count+1,root,*n2ow);
          *n2ow=ws;
        i=0;
        while(1){
            strcpy(stack[i],ws->fname);
            if(strncmp(stack[i],"/",1)==0)
              break;
            i++;
            ws=ws->parents;
        }
        for(int j=i-1;j>=0;j--){
          if(j==i-1)
            sprintf(now_dir,"/%s",stack[j]);
          else
            sprintf(now_dir+strlen(stack[j])+1,"/%s",stack[j]);
        }
    }
    else{ //상대 경로
      for(int j=1;j<10000;j++)
        now_dir[j]=0;
        int count=0;
        if(route[strlen(route)-1]=='/')
         route[strlen(route)-1]=0;
        for(int k=0;k<=(int)strlen(route);k++){
          if(route[k]=='/'||route[k]==0){
            count++;
          }
        }
        name_list=(char (*)[5])calloc(count*5,sizeof(char));
          temp=strtok(route,"/");
          name_list[0][0]=temp[0];
          name_list[0][1]=temp[1];
          name_list[0][2]=temp[2];
          name_list[0][3]=temp[3];
         for(int k=1;k<count;k++){
           temp=strtok(NULL,"/");
           name_list[k][0]=temp[0];
           name_list[k][1]=temp[1];
           name_list[k][2]=temp[2];
           name_list[k][3]=temp[3];
         }
         ws=change_now(name_list,count,root,*n2ow);
        *n2ow=ws;
         i=0;
         while(1){
             strcpy(stack[i],ws->fname);
             if(strncmp(stack[i],"/",1)==0)
               break;
             i++;
             ws=ws->parents;
         }
         for(int j=i-1;j>=0;j--){
           if(j==i-1)
             sprintf(now_dir,"/%s",stack[j]);
           else
             sprintf(now_dir+strlen(stack[j])+1,"/%s",stack[j]);
         }
    }
    now=fin;
  }
}
//경로를 해석하고 tree로 가서 경로명 출력

void mymkdir(char *order,file_st *myfs){
  char route[1000]="",dname[5]="",temp[1000]="";
  int fin,inum,dnum;
  time_t nowt;
  nowt=time(NULL);
  sscanf(order,"%*s %s",route);
  if(route[strlen(route)-1]=='/')
   route[strlen(route)-1]=0;
  if(route[0]=='/'){ //절대 경로
   int count=1,i=1;
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
     if(count!=1){
      for(int i=strlen(route)-1,j=0;;i--,j++){
        if(route[i]=='/')
          break;
        else{
          temp[j]=route[i];
          route[i]=0;
        }
      }
      for(int i=0;i<4;i++)
        dname[i]=temp[strlen(temp)-1-i];
      fin=find_rinode(route,myfs);
      }
      else{
         strncpy(dname,route+1,4);
         fin=0;
      }
      if(fin==-1){
        printf("error : no directory '%s'\n",route);
        return ;
      }
      else{
        int *dnum,rn,temp;
        dnum=find_dnum(fin,myfs);
        rn=count_dnum(fin,myfs);
        for(int i=0;i<rn;i++){
          temp=find_file(myfs->data[dnum[i]].dir,dname);
          if(temp!=-1) {
            printf("error : file already exists\n");
            return ;
          }
        }
        free(dnum);
      }
      inum=empty_ch(myfs->inode_ch,64);
      bit_mark(myfs->inode_ch,inum);
      dnum=empty_ch(myfs->data_ch,128);
      bit_mark(myfs->data_ch,dnum);
      myfs->inode[inum].type='d';
      myfs->inode[inum].c_time=*(localtime(&nowt));
      myfs->inode[inum].size=0;
      myfs->inode[inum].dir_b=dnum;
      myfs->inode[inum].in_b=-1;
      myfs->inode[inum].indb_b=-1;
      myfs->data[dnum].dir.current=inum;
      myfs->data[dnum].dir.parent=fin;
      myfs->data[dnum].dir.fcount=0;
      write_in_file(myfs,dname,inum,fin);
  }
  else{ //상대 경로
    int count=0;
    if(route[strlen(route)-1]=='/')
     route[strlen(route)-1]=0;
    for(int i=0;i<=(int)strlen(route);i++){
      if(route[i]=='/'||route[i]==0){
        count++;
      }
    }
    if(count!=1){
     for(int i=strlen(route)-1,j=0;;i--,j++){
       if(route[i]=='/')
         break;
       else{
         temp[j]=route[i];
         route[i]=0;
       }
     }
     for(int i=0;i<4;i++)
       dname[i]=temp[strlen(temp)-1-i];
     fin=find_rinode(route,myfs);
    }
    else {
       strncpy(dname,route,4);
       fin=now;
    }
    if(fin==-1){
      printf("error : no directory '%s'\n",route);
      return ;
    }
    else{
      int *dnum,rn,temp;
      dnum=find_dnum(fin,myfs);
      rn=count_dnum(fin,myfs);
      for(int i=0;i<rn;i++){
        temp=find_file(myfs->data[dnum[i]].dir,dname);
        if(temp!=-1) {
          printf("error : file already exists\n");
          return ;
        }
      }
      free(dnum);
    }
    inum=empty_ch(myfs->inode_ch,64);
    bit_mark(myfs->inode_ch,inum);
    dnum=empty_ch(myfs->data_ch,128);
    bit_mark(myfs->data_ch,dnum);
    myfs->inode[inum].type='d';
    myfs->inode[inum].c_time=*(localtime(&nowt));
    myfs->inode[inum].size=0;
    myfs->inode[inum].dir_b=dnum;
    myfs->inode[inum].in_b=-1;
    myfs->inode[inum].indb_b=-1;
    myfs->data[dnum].dir.current=inum;
    myfs->data[dnum].dir.parent=fin;
    myfs->data[dnum].dir.fcount=0;
    write_in_file(myfs,dname,inum,fin);
  }
}
//폴더를 생성

void myshowfile(char *order,file_st *myfs){
  int num1,num2,*dnum,fin,size,n1b,n2b,n1r,n2r;
  char route[1000];
  sscanf(order,"%*s %d %d %s",&num1,&num2,route);
  fin=find_rinode(route,myfs);
  if(fin==-1){
      printf("error : no file '%s'\n",route);
      return ;
  }
  size=myfs->inode[fin].size;
  if(num1>size||num2>size){
    printf("error : file size is %d\n",size);
    return;
  }
  dnum=find_dnum(fin,myfs);
  if(num1%128==0&&num1!=0)
    n1b=num1/128-1;
  else
    n1b=num1/128;
  if(num2%128==0&&num2!=0)
    n2b=num2/128-1;
  else
    n2b=num2/128;
  n1r=num1%128;
  if(n1r==0&&num1!=0)
    n1r=128;
  n2r=num2%128;
  if(n2r==0&&num2!=0)
      n2r=128;
  if(n1b==n2b){
    for(int i=n1r-1;i<n2r;i++)
    printf("%c",myfs->data[dnum[n1b]].reg.save[i]);
    return ;
  }
  for(int i=n1r-1;i<128;i++){
    printf("%c",myfs->data[dnum[n1b]].reg.save[i]);
  }
  for(int i=n1b+1;i<=n2b-1;i++){
    for(int j=0;j<128;j++)
      printf("%c",myfs->data[dnum[i]].reg.save[j]);
  }
  for(int i=0;i<n2r;i++){
    printf("%c",myfs->data[dnum[n2b]].reg.save[i]);
  }
}
//파일의 원하는 byte부터 원하는 byte를 출력

void mystate(char *order,file_st *myfs){
  int data_c;
  data_c=empty_count(myfs->inode_ch,64);
  printf("free inode : %d\n",data_c);
  data_c=empty_count(myfs->data_ch,128);
  printf("free data block : %d\n",data_c);
}
//현재 상태를 알려줌

void mycpfrom(char *order,file_st *myfs){
  char my_file_name[5]="";
	char file_name[1000];
  int file_size,i_empty,*db_empty,size_c,data_c,wstation=now;
	FILE *fop;
	time_t ct;
	ct=time(NULL);
  sscanf(order,"%*s %s %c %c %c %c ",file_name,&my_file_name[0],&my_file_name[1],&my_file_name[2],&my_file_name[3]);
  fop=fopen(file_name,"rb");
  if(fop==NULL){
    printf("error : no file '%s'\n",file_name);
    return ;
  }
  {
    int *dnum,rn,temp;
    dnum=find_dnum(wstation,myfs);
    rn=count_dnum(wstation,myfs);
    for(int i=0;i<rn;i++){
      temp=find_file(myfs->data[dnum[i]].dir,my_file_name);
      if(temp!=-1) {
        printf("error : file already exists\n");
        return ;
      }
    }
    free(dnum);
  }
  fseek(fop,0,SEEK_END);
	file_size=ftell(fop);
	fseek(fop,0,SEEK_SET);
  size_c=cal_sizec(file_size);
  data_c=empty_count(myfs->data_ch,128);

  if(size_c<=data_c){
		i_empty=empty_ch(myfs->inode_ch,64);
		bit_mark(myfs->inode_ch,i_empty);

		myfs->inode[i_empty].type='-';
		myfs->inode[i_empty].c_time=*(localtime(&ct));
		myfs->inode[i_empty].size=file_size;
		db_empty=make_list_emdb(myfs->data_ch,size_c);
		write_in_file(myfs,my_file_name,i_empty,wstation);

		if(size_c==1){
			myfs->inode[i_empty].dir_b=db_empty[0];
			myfs->inode[i_empty].in_b=-1;
			myfs->inode[i_empty].indb_b=-1;
			fread(myfs->data[db_empty[0]].reg.save,file_size,1,fop);
		}
		else if(size_c>=3&&size_c<=104){
			myfs->inode[i_empty].dir_b=db_empty[0];
			myfs->inode[i_empty].in_b=db_empty[size_c-1];
			myfs->inode[i_empty].indb_b=-1;
      for(int i=1;i<size_c-1;i++)
        itob(myfs->data[db_empty[size_c-1]].reg.save,db_empty[i]);
      for(int i=0;i<size_c-2;i++)
        fread(myfs->data[db_empty[i]].reg.save,128,1,fop);
      {
        int temp;
        temp=file_size%128;
        if(file_size/128!=0&&temp==0)
          temp=128;
        fread(myfs->data[db_empty[size_c-2]].reg.save,temp,1,fop);
      }
    }
	}
	else
		printf("error : can't save file (no space)\n");
  free(db_empty);
  fclose(fop);
}
void mycpto(char *order,file_st *myfs){
  char my_file_name[5]="",cpf_name[1000]="";
  int fin,rn,*dnum,size;
  FILE *fop;
  sscanf(order,"%*s %4s",my_file_name);
  sscanf(order,"%*s %*s %s",cpf_name);
  dnum=find_dnum(now,myfs);
  rn=count_dnum(now,myfs);
  for(int i=0;i<rn;i++){
    fin=find_file(myfs->data[dnum[i]].dir,my_file_name);
    if(fin!=-1) break;
  }
  if(fin==-1){
    printf("error : no file\n");
    return ;
  }
  free(dnum);
  dnum=find_dnum(fin,myfs);
  rn=count_dnum(fin,myfs);
  size=myfs->inode[fin].size;
  fop=fopen(cpf_name,"wb");
  for(int i=0;i<rn;i++){
    if(i==rn-1){
      fwrite(myfs->data[dnum[i]].reg.save,size,1,fop);
    }
    else{
      size=size-128;
      fwrite(myfs->data[dnum[i]].reg.save,128,1,fop);
    }
  }
  fclose(fop);
}
//파일을 복사

void myls(char *order,file_st *myfs){
	char opt[1000]="",route[1000]="",ct_s[20];
	short dfn,wstation;
  int dnum;
  fls_st *srt;
	sscanf(order,"%*s %s %s",opt,route);
  if(opt[0]=='-'){
    if(route[0]==0)
      wstation=now;
    else
      wstation=find_rinode(route,myfs);
  }
  else if(opt[0]==0){
    wstation=now;
  }
  else
    wstation=find_rinode(opt,myfs);
  if(myfs->inode[wstation].type=='-'){
    printf("error : not file\n");
    return ;
  }
  dfn=myfs->data[myfs->inode[wstation].dir_b].dir.fcount;
  srt=(fls_st *)calloc(dfn,sizeof(fls_st));
  if(dfn<=20){
    for(int i=0;i<dfn;i++){
       strncpy(srt[i].fname,myfs->data[myfs->inode[wstation].dir_b].dir.fset[i].fname,4);
       srt[i].fname[4]=0;
       srt[i].finode=myfs->data[myfs->inode[wstation].dir_b].dir.fset[i].finode;
    }
  }
  else if(dfn>20){
    for(int i=0;i<20;i++){
      strncpy(srt[i].fname,myfs->data[myfs->inode[wstation].dir_b].dir.fset[i].fname,4);
      srt[i].fname[4]=0;
      srt[i].finode=myfs->data[myfs->inode[wstation].dir_b].dir.fset[i].finode;
    }
    for(int i=0;i<dfn/20-1;i++){
      dnum=btoi(myfs->data[myfs->inode[wstation].in_b].reg.save,i+1);
      for(int j=0;j<20;j++){
        strncpy(srt[(i+1)*20+j].fname,myfs->data[myfs->inode[dnum].dir_b].dir.fset[j].fname,4);
        srt[(i+1)*20+j].fname[4]=0;
        srt[(i+1)*20+j].finode=myfs->data[myfs->inode[dnum].dir_b].dir.fset[j].finode;
      }
    }
    dnum=btoi(myfs->data[myfs->inode[wstation].in_b].reg.save,dfn/20);
    {
      int temp;
      temp=dfn%20;
      if(dfn/20!=0&&temp==0)
        temp=20;
      for(int i=0;i<temp;i++){
        strncpy(srt[(dfn/20)*20+i].fname,myfs->data[dnum].dir.fset[i].fname,4);
        srt[(dfn/20)*20+i].fname[4]=0;
        srt[(dfn/20)*20+i].finode=myfs->data[dnum].dir.fset[i].finode;
      }
    }
  }
  qsort(srt,dfn,sizeof(fls_st),mycmp);
  if(opt[0]==0){ //option이 없는 경우
    printf(".\n..\n");
    for(int i=0;i<dfn;i++)
      printf("%s\n",srt[i].fname);
  }
  else if(opt[0]=='-'&&opt[1]=='l'&&opt[2]==0){ //-l 옵션
    int fin;
    fin=myfs->data[myfs->inode[wstation].dir_b].dir.current;
    ct_string(myfs->inode[fin].c_time,ct_s);
    printf("d %6d %s .\n",0,ct_s);
    fin=myfs->data[myfs->inode[wstation].dir_b].dir.parent;
    ct_string(myfs->inode[fin].c_time,ct_s);
    printf("d %6d %s ..\n",0,ct_s);
    for(int i=0;i<dfn;i++){
      fin=srt[i].finode;
      ct_string(myfs->inode[fin].c_time,ct_s);
      printf("%c %6d %s %s\n",myfs->inode[fin].type,myfs->inode[fin].size,ct_s,srt[i].fname);
    }
  }
  else if(opt[0]=='-'&&opt[1]=='i'&&opt[2]==0){//-i 옵션
    int fin;
    fin=myfs->data[myfs->inode[wstation].dir_b].dir.current;
    printf("%3d .\n",fin+1);
    fin=myfs->data[myfs->inode[wstation].dir_b].dir.parent;
    printf("%3d ..\n",fin+1);
    for(int i=0;i<dfn;i++){
      fin=srt[i].finode;
      printf("%3d %s\n",fin+1,srt[i].fname);
    }
  }
  else if((opt[0]=='-'&&opt[1]=='l'&&opt[2]=='i')||(opt[0]=='-'&&opt[1]=='i'&&opt[2]=='l')){ //-i 옵션과 -l 옵션 둘다일 경우
    int fin;
    fin=myfs->data[myfs->inode[wstation].dir_b].dir.current;
    ct_string(myfs->inode[fin].c_time,ct_s);
    printf("%3d d %6d %s .\n",fin+1,0,ct_s);
    fin=myfs->data[myfs->inode[wstation].dir_b].dir.parent;
    ct_string(myfs->inode[fin].c_time,ct_s);
    printf("%3d d %6d %s ..\n",fin+1,0,ct_s);
    for(int i=0;i<dfn;i++){
      fin=srt[i].finode;
      ct_string(myfs->inode[fin].c_time,ct_s);
      printf("%3d %c %6d %s %s\n",fin+1,myfs->inode[fin].type,myfs->inode[fin].size,ct_s,srt[i].fname);
    }
  }
  else {
    printf(".\n..\n");
    for(int i=0;i<dfn;i++)
      printf("%s\n",srt[i].fname);
  }
  free(srt);
}
//directory 파일로부터 정보를 읽어서 출력

void mycat(char *order,file_st *myfs){
  char f1name[1000]="",f2name[1000]="",f3name[1000]="",f4name[1000]="",*name;
  short wstation;
  int rn,rn2,rn3,*dnum,*dnum2,*dnum3,fin,file_size,temp,*str,newi;
  time_t nowt;
  nowt=time(NULL);
  sscanf(order,"%*s %s %s %s %s",f1name,f2name,f3name,f4name);

  if(f1name[0]!=0&&f2name[0]==0&&f3name[0]==0&&f4name[0]==0){
    wstation=find_rinode(f1name,myfs);
    fin=wstation;
    if(fin==-1){
      printf("error : no file '%s'\n",f1name);
      return ;
    }
    if(myfs->inode[fin].type=='d'){
      printf("error : directory file '%s'\n",f1name);
      return ;
    }
    file_size=myfs->inode[fin].size;
    temp=file_size;
    str=find_dnum(fin,myfs);
    rn=count_dnum(fin,myfs);
    for(int i=0;i<rn;i++){
      for(int j=0;j<128;j++)
        printf("%c",myfs->data[str[i]].reg.save[j]);
    }
  }
  if(f1name[0]!=0&&f2name[0]=='>'&&f3name[0]!=0&&f4name[0]==0){
    if(find_rinode(f1name,myfs)==-1 ){
      printf("error : no file '%s'\n",f1name);
      return ;
    }
    if(myfs->inode[find_rinode(f1name,myfs)].type=='d'){
      printf("error : '%s' is directory file\n",f1name);
      return ;
    }
    if(myfs->inode[find_rinode(f3name,myfs)].type=='d'){
      printf("error : '%s' is directory file\n",f3name);
      return ;
    }
    mycp(f1name,f3name,myfs);
  }
  if(f1name[0]!=0&&f2name[0]!=0&&f3name[0]==0&&f4name[0]==0){
    wstation=find_rinode(f1name,myfs);
    fin=wstation;
    if(fin==-1){
      printf("error : no file '%s'\n",f1name);
      return ;
    }
      file_size=myfs->inode[fin].size;
      temp=file_size;
      str=find_dnum(fin,myfs);
      rn=count_dnum(fin,myfs);
      for(int i=0;i<rn;i++){
        if(file_size-128>=0){
          temp=128;
          file_size=file_size-128;
        }
        else
          temp=file_size;
        for(int j=0;j<temp;j++)
          printf("%c",myfs->data[str[i]].reg.save[j]);
      }
      free(str);
      wstation=find_rinode(f2name,myfs);
      fin=wstation;
      if(fin==-1){
        printf("error : no file '%s'\n",f2name);
        return ;
      }
        file_size=myfs->inode[fin].size;
        temp=file_size;
        str=find_dnum(fin,myfs);
        rn=count_dnum(fin,myfs);
        for(int i=0;i<rn;i++){
          if(file_size-128>=0){
            temp=128;
            file_size=file_size-128;
          }
          else
            temp=file_size;
          for(int j=0;j<temp;j++)
            printf("%c",myfs->data[str[i]].reg.save[j]);
        }
        free(str);
  }
  if(f1name[0]!=0&&f2name[0]!=0&&f3name[0]=='>'&&f4name[0]!=0){
    if(find_rinode(f1name,myfs)==-1 ){
      printf("error : no file '%s'\n",f1name);
      return ;
    }
    if(find_rinode(f2name,myfs)==-1 ){
      printf("error : no file '%s'\n",f2name);
      return ;
    }
    if(myfs->inode[find_rinode(f1name,myfs)].type=='d'){
      printf("error : '%s' is directory file\n",f1name);
      return ;
    }
    if(myfs->inode[find_rinode(f2name,myfs)].type=='d'){
      printf("error : '%s' is directory file\n",f2name);
      return ;
    }
    if(find_rinode(f1name,myfs)==find_rinode(f3name,myfs)){
      printf("error : input file is same output file\n");
      return ;
    }
    if(find_rinode(f2name,myfs)==find_rinode(f3name,myfs)){
      printf("error : input file is same output file\n");
      return ;
    }
    dnum=find_dnum(find_rinode(f1name,myfs),myfs);
    dnum2=find_dnum(find_rinode(f2name,myfs),myfs);
    rn=count_dnum(find_rinode(f1name,myfs),myfs);
    rn2=count_dnum(find_rinode(f2name,myfs),myfs);
    if(find_rinode(f4name,myfs)==-1){ //파일이 없어서 새로 만들어야 함
      int size_c;
      newi=empty_ch(myfs->inode_ch,64);
      bit_mark(myfs->inode_ch,newi);
      file_size=myfs->inode[find_rinode(f1name,myfs)].size+myfs->inode[find_rinode(f2name,myfs)].size;
      size_c=cal_sizec(file_size);
      str=make_list_emdb(myfs->data_ch,size_c);
      myfs->inode[newi].type='-';
      myfs->inode[newi].c_time=*(localtime(&nowt));
      myfs->inode[newi].size=myfs->inode[find_rinode(f1name,myfs)].size+myfs->inode[find_rinode(f2name,myfs)].size;
      myfs->inode[newi].dir_b=str[0];
      myfs->inode[newi].in_b=str[size_c-1];
      myfs->inode[newi].indb_b=-1;
      for(int i=1;i<size_c-1;i++){
        itob(myfs->data[myfs->inode[newi].in_b].reg.save,str[i]);
      }
      dnum3=find_dnum(newi,myfs);
      rn3=count_dnum(newi,myfs);
      int pos=0;
      for(int i=0;i<rn;i++){
        if(i==rn-1){
          for(int j=0;j<myfs->inode[find_rinode(f1name,myfs)].size-128*(rn-1);j++){
            myfs->data[dnum3[pos/128]].reg.save[pos%128]=myfs->data[dnum[i]].reg.save[j];
            pos++;
          }
        }
        else
          for(int j=0;j<128;j++){
            myfs->data[dnum3[pos/128]].reg.save[pos%128]=myfs->data[dnum[i]].reg.save[j];
            pos++;
          }
      }
      for(int i=0;i<rn2;i++){
        if(i==rn2-1){
          for(int j=0;j<myfs->inode[find_rinode(f2name,myfs)].size-128*(rn2-1);j++){
            myfs->data[dnum3[pos/128]].reg.save[pos%128]=myfs->data[dnum2[i]].reg.save[j];
            pos++;
          }
        }
        else
          for(int j=0;j<128;j++){
            myfs->data[dnum3[pos/128]].reg.save[pos%128]=myfs->data[dnum2[i]].reg.save[j];
            pos++;
          }
      }
      name=change_route(f4name);
      if(name==NULL) {name=f4name; wstation=now;}
      else if(name==(char *)1) {name=f4name+1; wstation=0;}
      else  {wstation=find_rinode(f4name,myfs);}
      write_in_file(myfs,name,newi,wstation);
      free(dnum3);
      free(str);
    }
    else if(find_rinode(f4name,myfs)!=-1){ //파일이 이미 있어서 수정 해야 함
      dnum3=find_dnum(find_rinode(f4name,myfs),myfs);
      rn3=count_dnum(find_rinode(f4name,myfs),myfs);
      if(rn3==1){
        myfs->inode[find_rinode(f4name,myfs)].in_b=empty_ch(myfs->data_ch,128);
        bit_mark(myfs->data_ch,myfs->inode[find_rinode(f4name,myfs)].in_b);
        str=make_list_emdb(myfs->data_ch,rn+rn2-1);
        for(int i=0;i<rn+rn2-1;i++)
          itob(myfs->data[myfs->inode[find_rinode(f4name,myfs)].in_b].reg.save,str[i]);
        free(str);
      }
      else if(rn3>rn+rn2){
        rm_bit(myfs->data[myfs->inode[find_rinode(f4name,myfs)].in_b].reg.save,rn3-rn-rn2,myfs);
      }
      else if(rn3<rn+rn2){
        str=make_list_emdb(myfs->data_ch,rn+rn2-rn3);
        for(int i=0;i<rn+rn2-rn3;i++)
          itob(myfs->data[myfs->inode[find_rinode(f4name,myfs)].in_b].reg.save,str[i]);
        free(str);
      }
      myfs->inode[find_rinode(f4name,myfs)].c_time=*(localtime(&nowt));
      myfs->inode[find_rinode(f4name,myfs)].size=myfs->inode[find_rinode(f1name,myfs)].size+myfs->inode[find_rinode(f2name,myfs)].size;
      dnum3=find_dnum(find_rinode(f4name,myfs),myfs);
      for(int i=0;i<rn;i++){
        for(int j=0;j<128;j++)
          myfs->data[dnum3[i]].reg.save[j]=myfs->data[dnum[i]].reg.save[j];
      }
      for(int i=0;i<rn2;i++){
        for(int j=0;j<128;j++)
          myfs->data[dnum3[i+rn]].reg.save[j]=myfs->data[dnum2[i]].reg.save[j];
      }
      free(dnum3);
    }
    free(dnum);
    free(dnum2);
  }
}
//리다이렉션과 파일 출력

void myshowinode(char *order,file_st *myfs){
  int num;
  char ct_s[20];
  sscanf(order,"%*s %d",&num);
  if(myfs->inode[num-1].type=='d')
    printf("file type : directory file\n");
  else if(myfs->inode[num-1].type==0){
    printf("error : empty inode '%d'\n",num);
    return;
  }
  else if(myfs->inode[num-1].type=='-')
    printf("file type : regular file\n");
  printf("file size : %d byte\n",myfs->inode[num-1].size);
  ct_string(myfs->inode[num-1].c_time,ct_s);
  printf("modified time : %s\n",ct_s);
  printf("data block list : %d, %d, %d\n",myfs->inode[num-1].dir_b+1,myfs->inode[num-1].in_b+1,myfs->inode[num-1].indb_b+1);
}
//inode의 정보 출력

void myshowblock(char *order,file_st *myfs){
  int num;
  sscanf(order,"%*s %d",&num);
  for(int i=0;i<128;i++)
    printf("%c",myfs->data[num-1].reg.save[i]);
}
//data block을 출력

void mytouch(char *order,file_st *myfs){
  time_t nowt;
  nowt=time(NULL);
  int fin;
  char *name,route[1000]="";
  sscanf(order,"%*s %s",route);
  fin=find_rinode(route,myfs);
  if(route[strlen(route)-1]=='/')
    route[strlen(route)-1]=0;
  if(fin==-1){
    int i_empty,d_empty;
    name=change_route(route);
    fin=find_rinode(route,myfs);
    if(name==NULL || name==(char *)1) ;
    else if(fin==-1){printf("error : no directory\n");return;}
    //파일 생성
    i_empty=empty_ch(myfs->inode_ch,64);
    bit_mark(myfs->inode_ch,i_empty);
    myfs->inode[i_empty].type='-';
    myfs->inode[i_empty].c_time=*(localtime(&nowt));
    myfs->inode[i_empty].size=0;
    d_empty=empty_ch(myfs->data_ch,128);
    bit_mark(myfs->data_ch,d_empty);
    myfs->inode[i_empty].dir_b=d_empty;
    myfs->inode[i_empty].in_b=-1;
    myfs->inode[i_empty].indb_b=-1;
    if(name==NULL)
      write_in_file(myfs,route,i_empty,now);
    else if(name==(char *)1){
      write_in_file(myfs,route+1,i_empty,0);
    }
    else {
      int ch;
      ch=find_rinode(route,myfs);
      write_in_file(myfs,name,i_empty,ch);
      free(name);
    }
  }
  else{
    myfs->inode[fin].c_time=*(localtime(&nowt));
  }
}
//파일을 생성 및 수정 시간을 바꿈

void mycp(char *f1route,char *f2route,file_st *myfs){
  int f1in,f2in,wstation,newi,size_c,*emdb,*dnum1,*dnum2,rn,size,rn2,*temp;
  char *name;
  time_t nowt;
  nowt=time(NULL);
  f1in=find_rinode(f1route,myfs);
  if(f1in==-1){
    printf("error : %s is no file\n",f1route);
    return ;
  }
  if(myfs->inode[f1in].type=='d'){
    printf("error : %s is directory file\n",f1route);
    return ;
  }
  f2in=find_rinode(f2route,myfs); //파일이 없는 경우 or 있는 경우
  if(f2in==-1){ //파일이 없는경우
    name=change_route(f2route);
    if(name==NULL) {name=f2route; wstation=now;}
    else if(name==(char *)1) {name=f2route+1; wstation=0;}
    else  {wstation=find_rinode(f2route,myfs);}
    newi=empty_ch(myfs->inode_ch,64);
    bit_mark(myfs->inode_ch,newi);
    myfs->inode[newi].type='-';
    myfs->inode[newi].c_time=*(localtime(&nowt));
    myfs->inode[newi].size=myfs->inode[f1in].size;
    size=myfs->inode[f1in].size;
    size_c=cal_sizec(size);
    emdb=make_list_emdb(myfs->data_ch,size_c);
    myfs->inode[newi].dir_b=emdb[0];
    if(size_c>1){
      myfs->inode[newi].in_b=emdb[size_c-1];
      for(int i=1;i<size_c-1;i++)
        itob(myfs->data[emdb[size_c-1]].reg.save,emdb[i]);
      myfs->inode[newi].indb_b=-1;
    }
    else {
      myfs->inode[newi].in_b=-1;
      myfs->inode[newi].indb_b=-1;
    }
    free(emdb);
    dnum1=find_dnum(f1in,myfs);
    rn=count_dnum(f1in,myfs);
    dnum2=find_dnum(newi,myfs);
    for(int i=0;i<rn;i++){
      if(i==rn-1){
        for(int j=0;j<size;j++)
          myfs->data[dnum2[i]].reg.save[j]=myfs->data[dnum1[i]].reg.save[j];
      }
      else{
        for(int j=0;j<128;j++)
          myfs->data[dnum2[i]].reg.save[j]=myfs->data[dnum1[i]].reg.save[j];
        size=size-128;
      }
    }
    write_in_file(myfs,name,newi,wstation);
    free(dnum1);
    free(dnum2);
  }
  else { //파일이 이미 있는 경우
    char backup[1000]="";
    strcpy(backup,f1route);
    if(myfs->inode[f2in].type=='d'){ //directory일 경우 거기안에 파일 만들기
      wstation=f2in;
      name=change_route(backup);
      if(name==NULL) {name=backup;}
      else if(name==(char *)1) {name=backup+1;}
      newi=empty_ch(myfs->inode_ch,64);
      bit_mark(myfs->inode_ch,newi);
      myfs->inode[newi].type='-';
      myfs->inode[newi].c_time=*(localtime(&nowt));
      myfs->inode[newi].size=myfs->inode[f1in].size;
      size=myfs->inode[f1in].size;
      size_c=cal_sizec(size);
      emdb=make_list_emdb(myfs->data_ch,size_c);
      myfs->inode[newi].dir_b=emdb[0];
      if(size_c>1){
        myfs->inode[newi].in_b=emdb[size_c-1];
        for(int i=1;i<size_c-1;i++)
          itob(myfs->data[emdb[size_c-1]].reg.save,emdb[i]);
        myfs->inode[newi].indb_b=-1;
      }
      else {
        myfs->inode[newi].in_b=-1;
        myfs->inode[newi].indb_b=-1;
      }
      free(emdb);
      dnum1=find_dnum(f1in,myfs);
      rn=count_dnum(f1in,myfs);
      dnum2=find_dnum(newi,myfs);
      for(int i=0;i<rn;i++){
        if(i==rn-1){
          for(int j=0;j<size;j++)
            myfs->data[dnum2[i]].reg.save[j]=myfs->data[dnum1[i]].reg.save[j];
        }
        else{
          for(int j=0;j<128;j++)
            myfs->data[dnum2[i]].reg.save[j]=myfs->data[dnum1[i]].reg.save[j];
          size=size-128;
        }
      }
      write_in_file(myfs,name,newi,wstation);
      free(dnum1);
      free(dnum2);
    }
    else {  //regular 파일일 경우 수정
      dnum1=find_dnum(f1in,myfs);
      rn=count_dnum(f1in,myfs);
      dnum2=find_dnum(f2in,myfs);
      rn2=count_dnum(f2in,myfs);
      if(rn<rn2){
        if(rn==1){
          for(int i=1;i<rn2;i++){
              reset_db(dnum2[i],myfs);
              bit_unmark(myfs->data_ch,dnum2[i]);
          }
          reset_db(myfs->inode[f2in].in_b,myfs);
          bit_unmark(myfs->data_ch,myfs->inode[f2in].in_b);
          myfs->inode[f2in].in_b=-1;
        }
        else
          rm_bit(myfs->data[myfs->inode[f2in].in_b].reg.save,rn2-rn,myfs);
      }
      else if(rn>rn2){
        if(rn2==1){
          temp=make_list_emdb(myfs->data_ch,rn-rn2+1);
          myfs->inode[f2in].in_b=temp[rn-rn2];
          bit_mark(myfs->data_ch,temp[rn-rn2]);
          for(int i=0;i<rn-rn2;i++){
            bit_mark(myfs->data_ch,temp[i]);
            itob(myfs->data[myfs->inode[f2in].in_b].reg.save,temp[i]);
          }
          free(temp);
        }
        else{
          temp=make_list_emdb(myfs->data_ch,rn-rn2);
          for(int i=0;i<rn-rn2;i++){
            bit_mark(myfs->data_ch,temp[i]);
            itob(myfs->data[myfs->inode[f2in].in_b].reg.save,temp[i]);
          }
          free(temp);
        }
      }
      free(dnum2);
      dnum2=find_dnum(f2in,myfs);
      for(int i=0;i<rn;i++){
        reset_db(dnum2[i],myfs);
        for(int j=0;j<128;j++)
          myfs->data[dnum2[i]].reg.save[j]=myfs->data[dnum1[i]].reg.save[j];
      }
      myfs->inode[f2in].c_time=*(localtime(&nowt));
      myfs->inode[f2in].size=myfs->inode[f1in].size;
    }
  }
}
//파일 복사

void mymv(char *order,file_st *myfs){
    // 시간, 파일 이름
    //  mv fn dir
    //  mv fn dir/fn2
    //  mv fn fn2
    //  mv fn already_f    (already_f은 이미 있는 파일,이미 있는 파일은 삭제)
    char temp[1000]="",f1route[1000]="",f2route[1000]="",*name2,*name1;;
    int f1in,f2in,wstation,nwstation;
    time_t nowt;
    nowt=time(NULL);
    sscanf(order,"%*s %s %s",f1route,f2route);
    f1in=find_rinode(f1route,myfs);
    if(f1in==-1){
      printf("error : no file\n");
      return ;
    }
    name1=change_route(f1route);
    if(name1==NULL) {name1=f1route; nwstation=now;}
    else if(name1==(char *)1) {name1=f1route+1; nwstation=0;}
    else {nwstation=find_rinode(f1route,myfs);}
    f2in=find_rinode(f2route,myfs);
    if(f2in==-1){ //dir 디렉토리안에 새로 이름 바꿀때, 파일 이름이 없을 때 바꾸기->같은 경우
      name2=change_route(f2route);
      if(name2==NULL) {name2=f2route; wstation=now;}
      else if(name2==(char *)1) {name2=f2route+1; wstation=0;}
      else {wstation=find_rinode(f2route,myfs);}
      if(wstation==nwstation){
        change_finfo(myfs,name1,name2,wstation);
      }
      else{
        remove_in_file(myfs,name1,nwstation);
        write_in_file(myfs,name2,f1in,wstation);
      }
      myfs->inode[f1in].c_time=*(localtime(&nowt));
    }
    else { //파일이 디렉토리 파일일 경우, 이미 있는 파일을 삭제하고 이름 바꾸기
      if(f1in==f2in){
        printf("error : same file\n");
        return ;
      }
      if(myfs->inode[f2in].type=='d'){
        if(f2in==nwstation){
          printf("error : same file\n");
          return ;
        }
        else{
          int *fin_temp,rn_temp,fch;
          fin_temp=find_dnum(f2in,myfs);
          rn_temp=count_dnum(f2in,myfs);
          for(int i=0;i<rn_temp;i++){
            fch=find_file(myfs->data[fin_temp[i]].dir,name1);
            if(fch!=-1) break;
          }
          if(fch!=-1){
            sprintf(temp,"myrm %s/%s",f2route,name1);
            myrm(temp,myfs);
          }
          remove_in_file(myfs,name1,nwstation);
          write_in_file(myfs,name1,f1in,f2in);
          free(fin_temp);
        }
      }
      else {
        sprintf(temp,"myrm %s",f2route);
        myrm(temp,myfs);
        name2=change_route(f2route);
        if(name2==NULL) {name2=f2route; wstation=now;}
        else if(name2==(char *)1) {name2=f2route+1; wstation=0;}
        else {wstation=find_rinode(f2route,myfs);}
        if(wstation==nwstation){
          change_finfo(myfs,name1,name2,wstation);
        }
        else{
          remove_in_file(myfs,name1,nwstation);
          write_in_file(myfs,name2,f1in,wstation);
        }
      }
      myfs->inode[f1in].c_time=*(localtime(&nowt));
    }
}
//파일의 이름을 바꾸거나 경로를 바꿈 수정 시간도 바뀜
