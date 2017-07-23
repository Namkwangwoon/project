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

//myfs를 생성하고 root를 1번에 할당 (배열 표현이므로 1번은 0번이다.)
int main(){
  FILE *fop;
  time_t now;
  now=time(NULL);
  fop=fopen("myfs","r");
	if(fop!=NULL){
		printf("error : myfs exists\n"); //myfs가 이미 있는 경우 error
		return 0;
	}
	else
		fop=fopen("myfs","wb"); //없는경우 생성
  file_st myfs={""}; //구조체에 0을 넣기 위함
  myfs.inode_ch[0]=128; //128은 ich[0]=1000 0000(2진수)넣은것 inode_check
  myfs.data_ch[0]=128;	//128은 dch[0]=1000 0000(2진수)넣은것 data_check
  myfs.inode[0].type='d'; //directory file
  myfs.inode[0].c_time=*(localtime(&now)); //생성 시간
  myfs.inode[0].size=0; //directory file 이므로 크기가 0
  myfs.inode[0].dir_b=0; //data blcok 0번 할당
  myfs.inode[0].in_b=-1; //없으므로 -1
  myfs.inode[0].indb_b=-1; //없으므로 -1
  myfs.data[0].dir.current=0; //data block의 현재 inode
  myfs.data[0].dir.parent=0;  //data block의 부모 inode
  myfs.data[0].dir.fcount=0;  //root폴더 밑에 파일 개수 현재는 0개
  fwrite(&myfs,sizeof(myfs),1,fop);
  fclose(fop);
  return 0;
}
