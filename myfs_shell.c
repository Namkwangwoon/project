#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<math.h>
typedef struct data_st{  //data 구조
	unsigned char save[128];
}data_st;
typedef struct inode_st{ //inode 구조체
	char type;
	short dir_b;
	struct tm ctime;
	int size;
	short in_b;
	short indb_b;
}inode_st;
typedef struct file{  //file system구조체
	char boot[2];
	unsigned char inode_ch[64];
	unsigned char data_ch[128];
	short inode_c,data_c;
	inode_st inode[512];
	data_st data[1024];
}file_st;
void file_input(FILE *fop,file_st *myfs);
void file_output(FILE *fcp,file_st *myfs);
int order_ch(char *order,FILE *fop,file_st *myfs);
int main(){
	FILE *fop=fopen("myfs","rb"),*fcp;
	char order[1000],now_dir[10000]="/";
	if(fop==NULL){
		printf("error : no myfs\n");
		return 1;
	}
	file_st myfs;
	file_input(fop,&myfs); //myfs로부터 입력
	
	while(1)  //명령 받는 부분
	{
		printf("[%s ]$ ",now_dir);
		fgets(order,sizeof(order),stdin);
		if(order_ch(order,fop,&myfs)==-1)
			break;
	
	}
	fcp=fopen("myfs","wb");
	file_output(fcp,&myfs); //myfs로 출력
	fclose(fcp);
	fclose(fop);
	return 0;
}
void file_input(FILE *fop,file_st *myfs){ //파일 입력
	short temp;
	fread(myfs->boot,sizeof(myfs->boot),1,fop);
	fread(myfs->inode_ch,sizeof(myfs->inode_ch),1,fop);
	fread(myfs->data_ch,sizeof(myfs->data_ch),1,fop);
	fread(&myfs->inode_c,sizeof(myfs->inode_c),1,fop);
	fread(&myfs->data_c,sizeof(myfs->data_c),1,fop);
	for(int i=0;i<myfs->inode_c;i++){
		fread(&temp,sizeof(temp),1,fop);
		fread(&myfs->inode[temp],sizeof(myfs->inode[temp]),1,fop);	
	}
	for(int i=0;i<myfs->data_c;i++){
		fread(&temp,sizeof(temp),1,fop);
		fread(&myfs->data[temp],sizeof(myfs->data[temp]),1,fop);	
	}
}
void file_output(FILE *fcp,file_st *myfs){ //파일 출력
	short temp;
	unsigned char bit_temp,bit_ch=128;
	fwrite(myfs->boot,sizeof(myfs->boot),1,fcp);
	fwrite(myfs->inode_ch,sizeof(myfs->inode_ch),1,fcp);
	fwrite(myfs->data_ch,sizeof(myfs->data_ch),1,fcp);
	fwrite(&myfs->inode_c,sizeof(myfs->inode_c),1,fcp);
	fwrite(&myfs->data_c,sizeof(myfs->data_c),1,fcp);
	for(int i=0;i<64;i++){
		for(int j=0;j<8;j++){
			bit_ch=pow(2,7-j);
			bit_temp=myfs->inode_ch[i]&bit_ch;
			if(bit_temp!=0){
				temp=i*8+j;
				fwrite(&temp,sizeof(temp),1,fcp);
				fwrite(&myfs->inode[temp],sizeof(myfs->inode[temp]),1,fcp);
			}
		}
	}
	for(int i=0;i<128;i++){
		for(int j=0;j<8;j++){
			bit_ch=pow(2,7-j);
			bit_temp=myfs->data_ch[i]&bit_ch;
			if(bit_temp!=0){
				temp=i*8+j;
				fwrite(&temp,sizeof(temp),1,fcp);
				fwrite(&myfs->data[temp],sizeof(myfs->data[temp]),1,fcp);
			}
		}
	}
}
int order_ch(char *order,FILE *fop,file_st *myfs){ //명령에 따라 실행
	if(strncmp(order,"byebye",6)==0)
		return -1;
	if(strncmp(order,"my",2)!=0)
		system(order);

	return 0;
}
